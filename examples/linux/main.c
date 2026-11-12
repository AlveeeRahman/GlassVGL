/*
 * GlassVGL on Linux: one program that runs the GlassVGL demo or an app sample screen in a desktop
 * window (SDL2, which works under both X11 and Wayland sessions), straight on the screen with
 * DRM/KMS, or on the old /dev/fb0 framebuffer. Touchscreens, mice and keyboards are read through
 * evdev when there is no window system. Which backends exist depends on how it was built; pick one
 * at run time:
 *
 *     glassvgl_linux --backend sdl --screen smart-home --size 1024x600
 *     glassvgl_linux --backend fbdev --screen demo          (Jetson Nano console, no desktop)
 *     glassvgl_linux --list
 *
 * Run with --help for every option. Environment overrides that LVGL's own Linux port also uses:
 * LV_LINUX_FBDEV_DEVICE, LV_LINUX_DRM_CARD and LV_LINUX_EVDEV_POINTER_DEVICE.
 */
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "lvgl.h"
#include "lv_glass.h"
#include "demo/lv_glass_demo.h"
#include "glassvgl_showcase.h"
#if LV_USE_SDL
#include LV_SDL_INCLUDE_PATH
#endif

typedef enum { BACKEND_NONE, BACKEND_SDL, BACKEND_DRM, BACKEND_FBDEV } backend_t;

typedef struct {
    backend_t backend;
    int32_t w, h;
    bool fullscreen;
    bool perf;
    const char * screen;
    const char * shot;
    uint32_t quit_ms;
    uint32_t demo_period_ms;
    lv_glass_config_t glass;
} options_t;

/* lv_linux_drm_set_file() and lv_linux_fbdev_set_file() return a result from LVGL 9.5 on; before
 * that they return nothing, and a failed open leaves the display with no draw buffer. */
#if LVGL_VERSION_MAJOR > 9 || (LVGL_VERSION_MAJOR == 9 && LVGL_VERSION_MINOR >= 5)
#define SET_FILE_OK(call) ((call) == LV_RESULT_OK)
#else
#define SET_FILE_OK(call) ((call), lv_display_get_buf_active(disp) != NULL)
#endif

static volatile sig_atomic_t quit_requested;

static void on_signal(int sig)
{
    (void)sig;
    quit_requested = 1;
}

static uint32_t now_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

static const char * backend_name(backend_t b)
{
    switch(b) {
        case BACKEND_SDL: return "sdl";
        case BACKEND_DRM: return "drm";
        case BACKEND_FBDEV: return "fbdev";
        default: return "none";
    }
}

static backend_t backend_from_name(const char * s)
{
    if(!strcmp(s, "sdl")) return BACKEND_SDL;
    if(!strcmp(s, "drm")) return BACKEND_DRM;
    if(!strcmp(s, "fbdev")) return BACKEND_FBDEV;
    return BACKEND_NONE;
}

static bool backend_built(backend_t b)
{
    switch(b) {
        case BACKEND_SDL: return LV_USE_SDL;
        case BACKEND_DRM: return LV_USE_LINUX_DRM;
        case BACKEND_FBDEV: return LV_USE_LINUX_FBDEV;
        default: return false;
    }
}

/* A desktop session gets a window; a bare console gets DRM, then the framebuffer. */
static backend_t backend_default(void)
{
    bool desktop = getenv("DISPLAY") || getenv("WAYLAND_DISPLAY");
    if(desktop && LV_USE_SDL) return BACKEND_SDL;
    if(LV_USE_LINUX_DRM && access("/dev/dri/card0", R_OK | W_OK) == 0) return BACKEND_DRM;
    if(LV_USE_LINUX_FBDEV) return BACKEND_FBDEV;
    if(LV_USE_SDL) return BACKEND_SDL;
    return LV_USE_LINUX_DRM ? BACKEND_DRM : BACKEND_NONE;
}

static void print_backends(void)
{
    printf("backends built in:");
    for(int b = BACKEND_SDL; b <= BACKEND_FBDEV; b++)
        if(backend_built((backend_t)b)) printf(" %s", backend_name((backend_t)b));
    printf("%s\n", LV_USE_EVDEV ? " (+ evdev input)" : "");
}

static void usage(const char * prog)
{
    printf("usage: %s [options]\n"
           "  --backend sdl|drm|fbdev   where to draw (default: a window on a desktop, else drm, else fbdev)\n"
           "  --list                    print the backends this build has and exit\n"
           "  --size WxH                window size for sdl (default 800x480); drm and fbdev use the screen\n"
           "  --fullscreen              full-screen sdl window\n"
           "  --screen NAME             demo (default), pager, or a sample: player, smart-home, settings,\n"
           "                            music, weather, watch, charts\n"
           "  --cycle MS                with --screen demo, move to the next demo screen every MS ms\n"
           "  --scheme light|dark       colour scheme (default dark)\n"
           "  --accent NAME             azure, mint, coral, violet, amber or rose\n"
           "  --wallpaper NAME          graphite (default), aurora, sunset, ocean, blush or meadow\n"
           "  --mode auto|full|lite|flat glass quality (default auto)\n"
           "  --perf                    show LVGL's frame rate and CPU overlay\n"
           "  --shot FILE.ppm           save the screen to a PPM file once it has drawn, then exit\n"
           "  --quit-after MS           exit after MS milliseconds\n",
           prog);
    print_backends();
}

static int pick(const char * s, const char * const * names, int count)
{
    for(int i = 0; i < count; i++)
        if(!strcmp(s, names[i])) return i;
    return -1;
}

static int parse_args(int argc, char ** argv, options_t * o)
{
    static const char * const schemes[] = {"light", "dark"};
    static const char * const accents[] = {"azure", "mint", "coral", "violet", "amber", "rose"};
    static const char * const walls[] = {"aurora", "sunset", "ocean", "blush", "meadow", "graphite"};
    static const char * const modes[] = {"auto", "full", "lite", "flat"};
    static const lv_glass_mode_t mode_values[] = {LV_GLASS_MODE_AUTO, LV_GLASS_MODE_FULL, LV_GLASS_MODE_LITE,
                                                  LV_GLASS_MODE_FLAT};

    for(int i = 1; i < argc; i++) {
        const char * a = argv[i];
        const char * v = i + 1 < argc ? argv[i + 1] : NULL;
        int k;
        if(!strcmp(a, "--help") || !strcmp(a, "-h")) {
            usage(argv[0]);
            exit(0);
        }
        else if(!strcmp(a, "--list")) {
            print_backends();
            exit(0);
        }
        else if(!strcmp(a, "--fullscreen")) o->fullscreen = true;
        else if(!strcmp(a, "--perf")) o->perf = true;
        else if(!v) {
            fprintf(stderr, "%s needs a value (see --help)\n", a);
            return -1;
        }
        else if(!strcmp(a, "--backend")) {
            o->backend = backend_from_name(v);
            if(o->backend == BACKEND_NONE) {
                fprintf(stderr, "unknown backend %s\n", v);
                return -1;
            }
            i++;
        }
        else if(!strcmp(a, "--size")) {
            if(sscanf(v, "%dx%d", &o->w, &o->h) != 2 || o->w < 64 || o->h < 64) {
                fprintf(stderr, "--size wants WIDTHxHEIGHT, like 1024x600\n");
                return -1;
            }
            i++;
        }
        else if(!strcmp(a, "--screen")) o->screen = argv[++i];
        else if(!strcmp(a, "--shot")) o->shot = argv[++i];
        else if(!strcmp(a, "--quit-after")) o->quit_ms = (uint32_t)strtoul(argv[++i], NULL, 10);
        else if(!strcmp(a, "--cycle")) o->demo_period_ms = (uint32_t)strtoul(argv[++i], NULL, 10);
        else if(!strcmp(a, "--scheme") && (k = pick(v, schemes, 2)) >= 0) o->glass.scheme = (lv_glass_scheme_t)k, i++;
        else if(!strcmp(a, "--accent") && (k = pick(v, accents, 6)) >= 0) o->glass.accent = (lv_glass_accent_t)k, i++;
        else if(!strcmp(a, "--wallpaper") && (k = pick(v, walls, 6)) >= 0)
            o->glass.wallpaper = (lv_glass_wallpaper_t)k, i++;
        else if(!strcmp(a, "--mode") && (k = pick(v, modes, 4)) >= 0) o->glass.mode = mode_values[k], i++;
        else {
            fprintf(stderr, "unknown option or value: %s %s (see --help)\n", a, v);
            return -1;
        }
    }
    return 0;
}

#if LV_USE_EVDEV
static void add_evdev_input(void)
{
    const char * path = getenv("LV_LINUX_EVDEV_POINTER_DEVICE");
    if(path) {
        if(!lv_evdev_create(LV_INDEV_TYPE_POINTER, path)) fprintf(stderr, "could not open %s\n", path);
        return;
    }
    /* Picks up every mouse, touchscreen and keyboard, including ones plugged in later. Reading
     * /dev/input needs the input group: sudo usermod -aG input $USER */
    if(lv_evdev_discovery_start(NULL, NULL) != LV_RESULT_OK)
        fprintf(stderr, "evdev discovery failed; set LV_LINUX_EVDEV_POINTER_DEVICE=/dev/input/eventN\n");
}
#endif

/* Says why a display device couldn't be opened, since LVGL only logs a bare errno. */
static void device_hint(const char * path, const char * if_missing, const char * if_busy)
{
    if(access(path, F_OK) != 0)
        fprintf(stderr, "%s doesn't exist here (%s)\n", path, if_missing);
    else if(access(path, R_OK | W_OK) != 0)
        fprintf(stderr, "no permission for %s: sudo usermod -aG video $USER, then log in again\n", path);
    else
        fprintf(stderr, "could not use %s (%s)\n", path, if_busy);
}

static lv_display_t * open_display(const options_t * o)
{
    lv_display_t * disp = NULL;
    switch(o->backend) {
#if LV_USE_SDL
        case BACKEND_SDL:
            disp = lv_sdl_window_create(o->w, o->h);
            if(!disp) break;
            lv_sdl_window_set_title(disp, "GlassVGL");
            lv_sdl_window_set_resizeable(disp, true);
            lv_sdl_mouse_create();
            lv_sdl_mousewheel_create();
            lv_sdl_keyboard_create();
            break;
#endif
#if LV_USE_LINUX_DRM
        case BACKEND_DRM: {
            const char * card = getenv("LV_LINUX_DRM_CARD");
            disp = lv_linux_drm_create();
            if(!disp) break;
            if(!SET_FILE_OK(lv_linux_drm_set_file(disp, card ? card : "/dev/dri/card0", -1))) {
                device_hint(card ? card : "/dev/dri/card0", "no DRM driver loaded; try --backend fbdev",
                            "another program, like the desktop, may own it; stop it or use --backend sdl");
                return NULL;
            }
            break;
        }
#endif
#if LV_USE_LINUX_FBDEV
        case BACKEND_FBDEV: {
            const char * dev = getenv("LV_LINUX_FBDEV_DEVICE");
            disp = lv_linux_fbdev_create();
            if(!disp) break;
            if(!SET_FILE_OK(lv_linux_fbdev_set_file(disp, dev ? dev : "/dev/fb0"))) {
                device_hint(dev ? dev : "/dev/fb0", "JetPack 5 has no framebuffer console; try --backend drm",
                            "another program is drawing to it");
                return NULL;
            }
            break;
        }
#endif
        default:
            fprintf(stderr, "backend %s is not in this build\n", backend_name(o->backend));
            return NULL;
    }
#if LV_USE_EVDEV
    if(disp && o->backend != BACKEND_SDL) add_evdev_input();
#endif
    return disp;
}

static int show_screen(const char * screen, uint32_t period_ms)
{
    if(!screen || !strcmp(screen, "demo")) {
        lv_glass_demo_start(period_ms); /* 0: stay on the first screen */
        return 0;
    }
    if(!strcmp(screen, "pager")) {
        lv_glass_demo_pager_start(NULL);
        return 0;
    }
    for(int i = 0; i < GLASSVGL_SHOWCASE_COUNT; i++) {
        if(!strcmp(screen, glassvgl_showcase_name((glassvgl_showcase_t)i))) {
            glassvgl_showcase_show((glassvgl_showcase_t)i);
            return 0;
        }
    }
    fprintf(stderr, "unknown screen %s (see --help)\n", screen);
    return -1;
}

/* Writes the active screen as a binary PPM, from a snapshot in 24-bit colour. */
static int save_ppm(const char * path)
{
    lv_draw_buf_t * snap = lv_snapshot_take(lv_screen_active(), LV_COLOR_FORMAT_RGB888);
    if(!snap) return -1;
    FILE * f = fopen(path, "wb");
    if(!f) {
        lv_draw_buf_destroy(snap);
        return -1;
    }
    uint32_t w = snap->header.w, h = snap->header.h;
    fprintf(f, "P6\n%u %u\n255\n", (unsigned)w, (unsigned)h);
    for(uint32_t y = 0; y < h; y++) {
        const uint8_t * row = snap->data + (size_t)y * snap->header.stride;
        for(uint32_t x = 0; x < w; x++) {
            uint8_t rgb[3] = {row[x * 3 + 2], row[x * 3 + 1], row[x * 3]}; /* LVGL stores BGR */
            fwrite(rgb, 1, 3, f);
        }
    }
    lv_draw_buf_destroy(snap);
    return fclose(f);
}

int main(int argc, char ** argv)
{
    options_t o = {.backend = BACKEND_NONE, .w = 800, .h = 480};
    lv_glass_config_init(&o.glass);
    o.glass.scheme = LV_GLASS_SCHEME_DARK;
    o.glass.accent = LV_GLASS_ACCENT_AZURE;
    if(parse_args(argc, argv, &o) != 0) return 2;
    if(o.backend == BACKEND_NONE) o.backend = backend_default();
    if(!backend_built(o.backend)) {
        fprintf(stderr, "no usable backend; ");
        print_backends();
        return 2;
    }

    signal(SIGINT, on_signal);
    signal(SIGTERM, on_signal);

    lv_init();
    lv_tick_set_cb(now_ms);
    lv_display_t * disp = open_display(&o);
    if(!disp) return 1;
#if LV_USE_SDL
    if(o.backend == BACKEND_SDL && o.fullscreen) {
        /* LVGL's SDL driver only goes full screen from lv_conf.h, so ask SDL directly. */
        SDL_Window * w = NULL;
#if SDL_VERSION_ATLEAST(2, 0, 22)
        SDL_Renderer * r = lv_sdl_window_get_renderer(disp);
        if(r) w = SDL_RenderGetWindow(r);
#else
        /* SDL before 2.0.22 (Ubuntu 18.04 has 2.0.8) can't map a renderer to its window; ours is the only one */
        for(Uint32 id = 1; id < 16 && !w; id++) w = SDL_GetWindowFromID(id);
#endif
        if(w) SDL_SetWindowFullscreen(w, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
#endif
    if(!o.perf) lv_sysmon_hide_performance(disp);

    lv_glass_theme_init(disp, &o.glass);
    if(show_screen(o.screen, o.demo_period_ms) != 0) return 2;

    lv_glass_stats_t st;
    lv_glass_get_stats(&st);
    printf("GlassVGL %d.%d.%d on LVGL %d.%d.%d, %s backend, %dx%d, glass mode %s, bake %u ms\n",
           LV_GLASS_VERSION_MAJOR, LV_GLASS_VERSION_MINOR, LV_GLASS_VERSION_PATCH, LVGL_VERSION_MAJOR,
           LVGL_VERSION_MINOR, LVGL_VERSION_PATCH, backend_name(o.backend),
           (int)lv_display_get_horizontal_resolution(disp), (int)lv_display_get_vertical_resolution(disp),
           lv_glass_mode_name(st.mode), (unsigned)st.bake_ms);
    fflush(stdout);

    uint32_t start = now_ms();
    uint32_t shot_at = o.shot ? 500 : 0; /* let the first frames and any animations settle */
    while(!quit_requested) {
        uint32_t idle = lv_timer_handler();
        uint32_t elapsed = now_ms() - start;
        if(o.shot && elapsed >= shot_at) {
            int r = save_ppm(o.shot);
            printf(r == 0 ? "saved %s\n" : "could not save %s\n", o.shot);
            return r == 0 ? 0 : 1;
        }
        if(o.quit_ms && elapsed >= o.quit_ms) break;
        if(idle == LV_NO_TIMER_READY || idle > 10) idle = 10;
        usleep(idle * 1000);
    }
    return 0;
}

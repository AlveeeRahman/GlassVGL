/*
 * GlassVGL on a PC with no display attached: renders the demo's home screen, or one of the app
 * sample screens in ../showcase, into memory and writes it to a PPM file. Built by both
 * CMakeLists.txt and Makefile in this folder.
 *
 *     ./glassvgl_desktop [out.ppm] [width] [height] [screen]
 *
 * The size defaults to 800x480. screen is one of player, smart-home, settings, music, weather,
 * watch or charts, drawn with the showcase look; leave it out for the demo's home screen.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "lvgl.h"
#include "lv_glass.h"
#include "demo/lv_glass_demo.h"
#include "glassvgl_showcase.h"

static int32_t W = 800, H = 480;
static uint8_t * frame; /* the whole screen, RGB565 */

static void flush_cb(lv_display_t * disp, const lv_area_t * a, uint8_t * px)
{
    int32_t w = lv_area_get_width(a);
    for(int32_t y = a->y1; y <= a->y2; y++) {
        memcpy(frame + ((size_t)y * W + a->x1) * 2, px, (size_t)w * 2);
        px += w * 2;
    }
    lv_display_flush_ready(disp);
}

static uint32_t tick_cb(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

static int write_ppm(const char * path)
{
    FILE * f = fopen(path, "wb");
    if(!f) return -1;
    fprintf(f, "P6\n%d %d\n255\n", (int)W, (int)H);
    for(int32_t i = 0; i < W * H; i++) {
        uint16_t c = (uint16_t)(frame[i * 2] | (frame[i * 2 + 1] << 8));
        uint8_t rgb[3] = {(uint8_t)((c >> 11) << 3), (uint8_t)(((c >> 5) & 0x3F) << 2), (uint8_t)((c & 0x1F) << 3)};
        fwrite(rgb, 1, 3, f);
    }
    return fclose(f);
}

int main(int argc, char ** argv)
{
    const char * out = argc > 1 ? argv[1] : "glassvgl.ppm";
    if(argc > 3) {
        W = atoi(argv[2]);
        H = atoi(argv[3]);
    }
    frame = calloc((size_t)W * H, 2);

    lv_init();
    lv_tick_set_cb(tick_cb);
    lv_display_t * disp = lv_display_create(W, H);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);
    lv_display_set_flush_cb(disp, flush_cb);
    size_t buf_bytes = (size_t)W * (H / 10) * 2;
    lv_display_set_buffers(disp, malloc(buf_bytes), NULL, buf_bytes, LV_DISPLAY_RENDER_MODE_PARTIAL);

    lv_glass_config_t cfg;
    lv_glass_config_init(&cfg);
    cfg.scheme = LV_GLASS_SCHEME_DARK;
    cfg.accent = LV_GLASS_ACCENT_AZURE;
    cfg.wallpaper = LV_GLASS_WALLPAPER_GRAPHITE;
    const char * screen = argc > 4 ? argv[4] : NULL;
    /* the app screens use the showcase look: Inter type, soft shadows and the ribbon wallpaper */
    if(screen && !glassvgl_showcase_config(&cfg, disp)) fprintf(stderr, "showcase look unavailable\n");
    lv_glass_theme_init(disp, &cfg);

    if(screen) {
        int i = 0;
        while(i < GLASSVGL_SHOWCASE_COUNT && strcmp(screen, glassvgl_showcase_name((glassvgl_showcase_t)i))) i++;
        if(i == GLASSVGL_SHOWCASE_COUNT) {
            fprintf(stderr, "unknown screen %s\n", screen);
            return 1;
        }
        glassvgl_showcase_show((glassvgl_showcase_t)i);
    }
    else {
        lv_glass_demo_start(0); /* 0: stay on the first screen */
    }
    /* let the theme's short state transitions (a switch turning on) finish before the frame */
    uint32_t start = lv_tick_get();
    while(lv_tick_elaps(start) < 300) lv_timer_handler();
    lv_refr_now(disp);

    lv_glass_stats_t st;
    lv_glass_get_stats(&st);
    printf("GlassVGL %d.%d.%d on LVGL %d.%d.%d: mode %s, bake %u ms, %ux%d written to %s\n",
           LV_GLASS_VERSION_MAJOR, LV_GLASS_VERSION_MINOR, LV_GLASS_VERSION_PATCH, LVGL_VERSION_MAJOR,
           LVGL_VERSION_MINOR, LVGL_VERSION_PATCH, lv_glass_mode_name(st.mode), (unsigned)st.bake_ms,
           (unsigned)W, (int)H, out);
    return write_ppm(out) == 0 ? 0 : 1;
}

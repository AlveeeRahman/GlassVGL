/*
 * lv_conf.h for the GlassVGL Linux runner. The display and input backends are switched on by the
 * build (CMake options or the Makefile's BACKENDS list), which define LV_USE_SDL and friends on the
 * command line; everything left undefined here falls back to off.
 */
#ifndef LV_CONF_H
#define LV_CONF_H

/* 32-bit colour: no banding on desktop monitors, and the native format of most Linux framebuffers.
 * Build with -DGLASSVGL_COLOR_DEPTH=16 for an RGB565 panel. */
#ifndef GLASSVGL_COLOR_DEPTH
#define GLASSVGL_COLOR_DEPTH 32
#endif
#define LV_COLOR_DEPTH GLASSVGL_COLOR_DEPTH /* LVGL 9.3 to 9.5 */
#if GLASSVGL_COLOR_DEPTH == 16
#define LV_COLOR_FORMAT_DEFAULT LV_COLOR_FORMAT_RGB565 /* LVGL 9.6 */
#else
#define LV_COLOR_FORMAT_DEFAULT LV_COLOR_FORMAT_XRGB8888
#endif

#define LV_USE_STDLIB_MALLOC LV_STDLIB_CLIB
#define LV_USE_STDLIB_STRING LV_STDLIB_CLIB
#define LV_USE_STDLIB_SPRINTF LV_STDLIB_CLIB
#define LV_USE_OS LV_OS_NONE
#define LV_DEF_REFR_PERIOD 16 /* about 60 frames a second */

#define LV_USE_LOG 1
#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
#define LV_LOG_PRINTF 1

#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_24 1
#define LV_FONT_MONTSERRAT_28 1
#define LV_FONT_MONTSERRAT_36 1
#define LV_FONT_MONTSERRAT_48 1
#define LV_USE_THEME_DEFAULT 1
#define LV_USE_SNAPSHOT 1

/* --perf shows LVGL's frame-rate and CPU overlay. */
#define LV_USE_SYSMON 1
#define LV_USE_PERF_MONITOR 1
#define LV_USE_PERF_MONITOR_POS LV_ALIGN_BOTTOM_RIGHT

#define LV_BUILD_EXAMPLES 0
#define LV_BUILD_DEMOS 0

/* ---- Linux backends, set by the build ---- */
#ifndef LV_USE_SDL
#define LV_USE_SDL 0
#endif
#if LV_USE_SDL
#define LV_SDL_INCLUDE_PATH "SDL2/SDL.h"
#define LV_SDL_RENDER_MODE LV_DISPLAY_RENDER_MODE_DIRECT
#define LV_SDL_BUF_COUNT 1
#define LV_SDL_ACCELERATED 1
#define LV_SDL_FULLSCREEN 0
#define LV_SDL_DIRECT_EXIT 1
#endif

#ifndef LV_USE_LINUX_DRM
#define LV_USE_LINUX_DRM 0
#endif

#ifndef LV_USE_LINUX_FBDEV
#define LV_USE_LINUX_FBDEV 0
#endif
#if LV_USE_LINUX_FBDEV
#define LV_LINUX_FBDEV_BSD 0
#define LV_LINUX_FBDEV_RENDER_MODE LV_DISPLAY_RENDER_MODE_PARTIAL
#define LV_LINUX_FBDEV_BUFFER_COUNT 0
#define LV_LINUX_FBDEV_BUFFER_SIZE 60
#endif

#ifndef LV_USE_EVDEV
#define LV_USE_EVDEV 0
#endif

#endif /*LV_CONF_H*/

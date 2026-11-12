/* LVGL 9 config used by the GlassVGL examples. Copy it next to your project's sources
 * and change what your board needs. Only the lines that matter to the theme are set here;
 * everything else keeps LVGL's defaults. */
#ifndef LV_CONF_H
#define LV_CONF_H

/* 16-bit colour is what most SPI and parallel panels take. RGB565_SWAPPED (for SPI panels that
 * want the bytes the other way round), RGB888 and XRGB8888 work too. */
/* 16-bit colour. LVGL 9.3 and 9.4 read LV_COLOR_DEPTH; 9.6 and later read LV_COLOR_FORMAT_DEFAULT. */
#define LV_COLOR_DEPTH 16
#define LV_COLOR_FORMAT_DEFAULT LV_COLOR_FORMAT_RGB565

/* Use the C library's malloc. On ESP32 with PSRAM, large blocks then come from PSRAM, and the
 * theme puts its baked glass there on purpose (see the pack's docs/boards.md). */
#define LV_USE_STDLIB_MALLOC LV_STDLIB_CLIB
#define LV_USE_STDLIB_STRING LV_STDLIB_CLIB
#define LV_USE_STDLIB_SPRINTF LV_STDLIB_CLIB

#define LV_USE_LOG 1
#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN

/* fonts the demo screens use; the theme itself only needs the default */
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_28 1
#define LV_FONT_MONTSERRAT_48 1

/* the theme's parent theme */
#define LV_USE_THEME_DEFAULT 1

/* lets the swipe pager slide pictures of its pages (lv_glass_swipe.h); 0 makes it slide them live */
#define LV_USE_SNAPSHOT 1

#endif

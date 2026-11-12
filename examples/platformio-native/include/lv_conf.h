/* LVGL 9 config for the GlassVGL desktop example. */
#ifndef LV_CONF_H
#define LV_CONF_H

#define LV_COLOR_DEPTH 16 /* LVGL 9.3 to 9.5 */
#define LV_COLOR_FORMAT_DEFAULT LV_COLOR_FORMAT_RGB565 /* LVGL 9.6 */
#define LV_USE_STDLIB_MALLOC LV_STDLIB_CLIB
#define LV_USE_STDLIB_STRING LV_STDLIB_CLIB
#define LV_USE_STDLIB_SPRINTF LV_STDLIB_CLIB
#define LV_USE_OS LV_OS_NONE

/* Read the touch panel and redraw every 16 ms instead of LVGL's default 33. A tap then reaches the
 * screen sooner: in GlassVGL Pro's tap test the wait after the finger lifts falls from 16.9 ms to
 * 9.0 ms on average, and from 50 ms to 26 ms at worst. The cost is more redraws while something
 * animates; a board whose frame takes longer than 16 ms just redraws as often as it can. */
#define LV_DEF_REFR_PERIOD 16
#define LV_USE_LOG 1
#define LV_LOG_LEVEL LV_LOG_LEVEL_WARN
#define LV_LOG_PRINTF 1
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_20 1
#define LV_FONT_MONTSERRAT_28 1
#define LV_FONT_MONTSERRAT_48 1
#define LV_USE_THEME_DEFAULT 1
#define LV_USE_SNAPSHOT 1
#define LV_BUILD_EXAMPLES 0
#define LV_BUILD_DEMOS 0

#endif

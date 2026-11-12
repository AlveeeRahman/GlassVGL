/**
 * @file glassvgl_showcase.h
 * Seven app-style sample screens built on the GlassVGL theme: a media player, a smart home room,
 * settings, a music library, weather, a round watch face and an energy dashboard.
 *
 * They use only the public GlassVGL API (lv_glass.h) and stock LVGL widgets, so they show what an
 * app looks like with the theme and nothing else. MIT licence, like the rest of this repository.
 *
 *     lv_glass_theme_init(disp, &cfg);
 *     glassvgl_showcase_show(GLASSVGL_SHOWCASE_SMART_HOME);
 *
 * Every screen is laid out for landscape panels from 800x480 to 1024x600; 480x320 works with fewer
 * items on some screens. The watch face is made
 * for round panels (240x240 up to 466x466) and draws in a centred circle on any other size.
 */
#ifndef GLASSVGL_SHOWCASE_H
#define GLASSVGL_SHOWCASE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lv_glass.h"

typedef enum {
    GLASSVGL_SHOWCASE_PLAYER = 0, /**< now playing: cover, progress, transport and volume */
    GLASSVGL_SHOWCASE_SMART_HOME, /**< a room: light tiles, thermostat dial and scenes */
    GLASSVGL_SHOWCASE_SETTINGS,   /**< sidebar list and a display settings page */
    GLASSVGL_SHOWCASE_MUSIC,      /**< album shelf, recent tracks and a mini player */
    GLASSVGL_SHOWCASE_WEATHER,    /**< current conditions, hourly strip and five-day range bars */
    GLASSVGL_SHOWCASE_WATCH,      /**< round watch face with activity rings */
    GLASSVGL_SHOWCASE_CHARTS,     /**< energy dashboard: line and bar charts, a gauge and stat cards */
    GLASSVGL_SHOWCASE_COUNT,
} glassvgl_showcase_t;

/** Build one sample screen and load it. Call lv_glass_theme_init() first. Returns the screen. */
lv_obj_t * glassvgl_showcase_show(glassvgl_showcase_t which);

/** Short English name of a screen, for logs and file names ("player", "smart-home", ...). */
const char * glassvgl_showcase_name(glassvgl_showcase_t which);

#ifdef __cplusplus
}
#endif

#endif

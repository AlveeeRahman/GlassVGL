/*
 * GlassVGL demo for any Arduino or PlatformIO board with LVGL 9.
 *
 * The one board-specific part is panel_push(): send a block of RGB565 pixels to your display
 * with whatever driver you use (TFT_eSPI, LovyanGFX, Arduino_GFX, esp_lcd, a vendor BSP).
 * Set the size of your panel below. Touch is optional; see the pack's docs/boards.md.
 */
#include <Arduino.h>
#include <lvgl.h>
#include <lv_glass.h>
#include <demo/lv_glass_demo.h>

#ifndef PANEL_W
#define PANEL_W 320
#endif
#ifndef PANEL_H
#define PANEL_H 240
#endif

/* Replace this with your display driver's "draw a block of pixels" call. */
__attribute__((weak)) void panel_push(int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t * px)
{
    (void)x, (void)y, (void)w, (void)h, (void)px;
}

static void flush_cb(lv_display_t * disp, const lv_area_t * a, uint8_t * px)
{
    panel_push(a->x1, a->y1, lv_area_get_width(a), lv_area_get_height(a), (const uint16_t *)px);
    lv_display_flush_ready(disp);
}

static uint32_t tick_cb(void)
{
    return millis();
}

/* a tenth of the screen, in internal RAM: the usual partial-buffer setup */
static uint16_t draw_buf[PANEL_W * (PANEL_H / 10)];

void setup()
{
    Serial.begin(115200);
    lv_init();
    lv_tick_set_cb(tick_cb);

    lv_display_t * disp = lv_display_create(PANEL_W, PANEL_H);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_set_buffers(disp, draw_buf, NULL, sizeof(draw_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);

    lv_glass_config_t cfg;
    lv_glass_config_init(&cfg);
    cfg.scheme = LV_GLASS_SCHEME_DARK;
    cfg.accent = LV_GLASS_ACCENT_VIOLET;
    cfg.wallpaper = LV_GLASS_WALLPAPER_SUNSET;
    cfg.mode = LV_GLASS_MODE_AUTO; /* FULL with enough memory, else LITE, else FLAT */
    lv_glass_theme_init(disp, &cfg);

    lv_glass_stats_t st;
    lv_glass_get_stats(&st);
    char line[96];
    snprintf(line, sizeof(line), "lv_glass: mode %s, bake %u ms, %u bytes", lv_glass_mode_name(st.mode),
             (unsigned)st.bake_ms, (unsigned)(st.buffer_bytes + st.scratch_bytes));
    Serial.println(line);

    lv_glass_demo_start(5000);
}

void loop()
{
    lv_timer_handler();
    delay(5);
}

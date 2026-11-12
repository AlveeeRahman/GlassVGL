/*
 * GlassVGL swipe pager demo for any Arduino or PlatformIO board with LVGL 9 and a touch panel.
 *
 * Four demo pages side by side. Swipe left and right to turn them. With LV_USE_SNAPSHOT 1 in
 * lv_conf.h and enough RAM (2 x W x H x 2 bytes in RGB565), the pager slides pictures of the
 * pages, so each step of a swipe costs two image copies instead of redrawing every widget.
 * Without that RAM it slides the live pages. See the pack's docs/boards.md for which suits your board.
 *
 * Two board-specific parts: panel_push() sends a block of RGB565 pixels to your display, and
 * touch_read() reports where the panel is touched. Replace both with your drivers' calls.
 */
#include <Arduino.h>
#include <lvgl.h>
#include <lv_glass.h>
#include <lv_glass_swipe.h>
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

/* Replace this with your touch driver. Return true while the panel is touched. */
__attribute__((weak)) bool touch_read(int32_t * x, int32_t * y)
{
    (void)x, (void)y;
    return false;
}

static void flush_cb(lv_display_t * disp, const lv_area_t * a, uint8_t * px)
{
    panel_push(a->x1, a->y1, lv_area_get_width(a), lv_area_get_height(a), (const uint16_t *)px);
    lv_display_flush_ready(disp);
}

static void touch_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    (void)indev;
    int32_t x, y;
    if(touch_read(&x, &y)) {
        data->point.x = x;
        data->point.y = y;
        data->state = LV_INDEV_STATE_PRESSED;
    }
    else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

static uint32_t tick_cb(void)
{
    return millis();
}

/* a tenth of the screen, in internal RAM: the usual partial-buffer setup */
static uint16_t draw_buf[PANEL_W * (PANEL_H / 10)];

static lv_obj_t * pager;

void setup()
{
    Serial.begin(115200);
    lv_init();
    lv_tick_set_cb(tick_cb);

    lv_display_t * disp = lv_display_create(PANEL_W, PANEL_H);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_set_buffers(disp, draw_buf, NULL, sizeof(draw_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);

    lv_indev_t * touch = lv_indev_create();
    lv_indev_set_type(touch, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(touch, touch_cb);

    lv_glass_config_t cfg;
    lv_glass_config_init(&cfg);
    cfg.scheme = LV_GLASS_SCHEME_LIGHT;
    lv_glass_theme_init(disp, &cfg);

    lv_glass_swipe_config_t sw;
    lv_glass_swipe_config_init(&sw);   /* AUTO: snapshots in FULL mode, live slides otherwise */
    pager = lv_glass_demo_pager_start(&sw);
}

void loop()
{
    static uint32_t last_swipes;
    lv_timer_handler();
    lv_glass_swipe_stats_t st;
    lv_glass_pager_get_stats(pager, &st);
    if(st.swipes != last_swipes) {
        last_swipes = st.swipes;
        char line[96];
        snprintf(line, sizeof(line), "page %u, %s, snapshots took %u us", (unsigned)lv_glass_pager_get_page(pager),
                 st.mode_used == LV_GLASS_SWIPE_SNAPSHOT ? "snapshot" : "live", (unsigned)st.snapshot_us);
        Serial.println(line);
    }
    delay(5);
}

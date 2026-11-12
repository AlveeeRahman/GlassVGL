/*
 * GlassVGL demo for ESP-IDF 5 with LVGL 9.
 *
 * Fill in panel_push() with your esp_lcd panel (esp_lcd_panel_draw_bitmap) or vendor BSP, and
 * set PANEL_W and PANEL_H. With esp_lcd, call lv_display_flush_ready() from the panel's
 * "colour transfer done" callback instead of straight after the draw call.
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_heap_caps.h"
#include "lvgl.h"
#include "lv_glass.h"
#include "demo/lv_glass_demo.h"

#define PANEL_W 800
#define PANEL_H 480

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
    return (uint32_t)(esp_timer_get_time() / 1000);
}

void app_main(void)
{
    lv_init();
    lv_tick_set_cb(tick_cb);

    lv_display_t * disp = lv_display_create(PANEL_W, PANEL_H);
    lv_display_set_flush_cb(disp, flush_cb);
    /* draw buffer: a tenth of the screen in internal, DMA-capable RAM */
    size_t buf_bytes = PANEL_W * (PANEL_H / 10) * 2;
    void * buf = heap_caps_malloc(buf_bytes, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    lv_display_set_buffers(disp, buf, NULL, buf_bytes, LV_DISPLAY_RENDER_MODE_PARTIAL);

    lv_glass_config_t cfg;
    lv_glass_config_init(&cfg);
    cfg.scheme = LV_GLASS_SCHEME_LIGHT;
    cfg.accent = LV_GLASS_ACCENT_AZURE;
    lv_glass_theme_init(disp, &cfg); /* the glass goes to PSRAM when CONFIG_SPIRAM is on */

    lv_glass_stats_t st;
    lv_glass_get_stats(&st);
    printf("lv_glass: mode %s, bake %u ms, %u bytes\n", lv_glass_mode_name(st.mode), (unsigned)st.bake_ms,
           (unsigned)(st.buffer_bytes + st.scratch_bytes));

    lv_glass_demo_start(5000);
    while(1) {
        uint32_t wait = lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(wait < 5 ? 5 : (wait > 50 ? 50 : wait)));
    }
}

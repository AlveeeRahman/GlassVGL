/*
 * GlassVGL demo for Zephyr. Zephyr's LVGL module creates the display from the devicetree's
 * zephyr,display node; this sets the glass theme on it and cycles the six demo screens.
 */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/display.h>
#include <zephyr/logging/log.h>
#include <lvgl.h>
#include "lv_glass.h"
#include "demo/lv_glass_demo.h"

LOG_MODULE_REGISTER(glassvgl_demo, LOG_LEVEL_INF);

int main(void)
{
    const struct device * display = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
    if(!device_is_ready(display)) {
        LOG_ERR("display not ready");
        return 0;
    }

    lv_glass_config_t cfg;
    lv_glass_config_init(&cfg);
    cfg.scheme = LV_GLASS_SCHEME_DARK;
    cfg.accent = LV_GLASS_ACCENT_AZURE;
    cfg.wallpaper = LV_GLASS_WALLPAPER_GRAPHITE;
    lv_glass_theme_init(lv_display_get_default(), &cfg);

    lv_glass_stats_t st;
    lv_glass_get_stats(&st);
    LOG_INF("GlassVGL: mode %s, bake %u ms", lv_glass_mode_name(st.mode), (unsigned)st.bake_ms);

    lv_glass_demo_start(5000);
    display_blanking_off(display);

    while(1) {
        uint32_t sleep_ms = lv_timer_handler();
        k_msleep(MIN(sleep_ms, 50));
    }
    return 0;
}

/**
 * @file glassvgl_showcase.c
 * App-style sample screens for the GlassVGL theme. See glassvgl_showcase.h.
 *
 * Only the public GlassVGL API and stock LVGL 9 widgets are used. Every panel, button, slider,
 * switch, arc, bar, chart and list picks up its glass look from the theme; the code below only
 * lays things out and sets a few accent colours.
 */
#include "glassvgl_showcase.h"
#include <stdio.h>

/* ------------------------------------------------------------------ fonts */

#if LV_FONT_MONTSERRAT_12
#define F12 (&lv_font_montserrat_12)
#else
#define F12 LV_FONT_DEFAULT
#endif
#if LV_FONT_MONTSERRAT_14
#define F14 (&lv_font_montserrat_14)
#else
#define F14 F12
#endif
#if LV_FONT_MONTSERRAT_16
#define F16 (&lv_font_montserrat_16)
#else
#define F16 F14
#endif
#if LV_FONT_MONTSERRAT_20
#define F20 (&lv_font_montserrat_20)
#else
#define F20 F16
#endif
#if LV_FONT_MONTSERRAT_24
#define F24 (&lv_font_montserrat_24)
#else
#define F24 F20
#endif
#if LV_FONT_MONTSERRAT_28
#define F28 (&lv_font_montserrat_28)
#else
#define F28 F24
#endif
#if LV_FONT_MONTSERRAT_36
#define F36 (&lv_font_montserrat_36)
#else
#define F36 F28
#endif
#if LV_FONT_MONTSERRAT_48
#define F48 (&lv_font_montserrat_48)
#else
#define F48 F36
#endif

/* LVGL 9.6 added one setter per flag and deprecated lv_obj_add_flag()/lv_obj_remove_flag(). */
#if LVGL_VERSION_MAJOR > 9 || (LVGL_VERSION_MAJOR == 9 && LVGL_VERSION_MINOR >= 6)
#define NO_SCROLL(o)   lv_obj_set_scrollable((o), false)
#define NO_CLICK(o)    lv_obj_set_clickable((o), false)
#define FLOATING(o)    lv_obj_set_ignore_layout((o), true)
#else
#define NO_SCROLL(o)   lv_obj_remove_flag((o), LV_OBJ_FLAG_SCROLLABLE)
#define NO_CLICK(o)    lv_obj_remove_flag((o), LV_OBJ_FLAG_CLICKABLE)
#define FLOATING(o)    lv_obj_add_flag((o), LV_OBJ_FLAG_IGNORE_LAYOUT)
#endif

#define DPX(n) lv_dpx(n)

static int32_t s_w, s_h;
static bool s_small; /* 480x320 and smaller: tighter gaps and one font size down */

#define GAP (s_small ? DPX(8) : DPX(12))
#define PAD (s_small ? DPX(10) : DPX(16))

static const uint32_t PALETTE[] = {0x2F8CFF, 0x21C79A, 0xFF6B57, 0x8B6CFF, 0xF5A524, 0xF2508B};
#define C_AZURE 0
#define C_MINT 1
#define C_CORAL 2
#define C_VIOLET 3
#define C_AMBER 4
#define C_ROSE 5
static lv_color_t pal(int i)
{
    return lv_color_hex(PALETTE[i % 6]);
}

/* ---------------------------------------------------------------- helpers */

/* A plain layout container: no glass, no padding, no scrolling. */
static lv_obj_t * box(lv_obj_t * parent, int32_t w, int32_t h, lv_flex_flow_t flow, int32_t gap)
{
    lv_obj_t * b = lv_obj_create(parent);
    lv_glass_remove(b);
    lv_obj_set_size(b, w, h);
    lv_obj_set_style_pad_all(b, 0, 0);
    lv_obj_set_style_pad_gap(b, gap, 0);
    lv_obj_set_flex_flow(b, flow);
    NO_SCROLL(b);
    NO_CLICK(b);
    return b;
}

/* A row whose children sit at both ends (or spread out), centred vertically. */
static lv_obj_t * spread_row(lv_obj_t * parent, int32_t w)
{
    lv_obj_t * r = box(parent, w, LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW, GAP);
    lv_obj_set_flex_align(r, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    return r;
}

static lv_obj_t * centre_row(lv_obj_t * parent, int32_t w, int32_t gap)
{
    lv_obj_t * r = box(parent, w, LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW, gap);
    lv_obj_set_flex_align(r, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    return r;
}

/* A frosted glass panel laid out as a column. */
static lv_obj_t * panel(lv_obj_t * parent, int32_t w, int32_t h)
{
    lv_obj_t * p = lv_obj_create(parent);
    lv_obj_set_size(p, w, h);
    lv_obj_set_flex_flow(p, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_all(p, s_small ? DPX(10) : DPX(14), 0);
    lv_obj_set_style_pad_gap(p, s_small ? DPX(4) : DPX(8), 0);
    NO_SCROLL(p);
    return p;
}

static lv_obj_t * text(lv_obj_t * parent, const char * txt, const lv_font_t * font, bool muted)
{
    lv_obj_t * l = lv_label_create(parent);
    lv_label_set_text(l, txt);
    lv_obj_set_style_text_font(l, font, 0);
    if(muted) lv_obj_set_style_text_color(l, lv_glass_get_muted_color(), 0);
    return l;
}

static lv_obj_t * coloured(lv_obj_t * parent, const char * txt, const lv_font_t * font, lv_color_t c)
{
    lv_obj_t * l = text(parent, txt, font, false);
    lv_obj_set_style_text_color(l, c, 0);
    return l;
}

/* Cover art: a gradient tile with a soft highlight and a symbol, drawn without any image. */
static lv_obj_t * cover(lv_obj_t * parent, int32_t size, lv_color_t top, lv_color_t bottom, const char * sym)
{
    lv_obj_t * a = lv_obj_create(parent);
    lv_glass_set_variant(a, LV_GLASS_VARIANT_SOLID);
    lv_obj_set_size(a, size, size);
    lv_obj_set_style_radius(a, size / 8, 0);
    lv_obj_set_style_border_width(a, 0, 0);
    lv_obj_set_style_bg_opa(a, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(a, top, 0);
    lv_obj_set_style_bg_grad_color(a, bottom, 0);
    lv_obj_set_style_bg_grad_dir(a, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_bg_main_stop(a, 0, 0);
    lv_obj_set_style_bg_grad_stop(a, 255, 0);
    lv_obj_set_style_clip_corner(a, true, 0);
    lv_obj_set_style_pad_all(a, 0, 0);
    NO_SCROLL(a);
    NO_CLICK(a);

    lv_obj_t * glow = lv_obj_create(a);
    lv_glass_remove(glow);
    lv_obj_set_size(glow, size * 9 / 10, size * 9 / 10);
    lv_obj_set_style_radius(glow, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(glow, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(glow, LV_OPA_20, 0);
    lv_obj_set_style_bg_grad_dir(glow, LV_GRAD_DIR_NONE, 0);
    lv_obj_set_style_border_width(glow, 0, 0);
    lv_obj_align(glow, LV_ALIGN_TOP_RIGHT, size * 3 / 10, -size * 3 / 10);
    NO_CLICK(glow);

    if(sym) {
        lv_obj_t * s = lv_label_create(a);
        lv_label_set_text(s, sym);
        lv_obj_set_style_text_color(s, lv_color_white(), 0);
        lv_obj_set_style_text_font(s, size >= DPX(120) ? F48 : (size >= DPX(60) ? F28 : F16), 0);
        lv_obj_center(s);
    }
    return a;
}

/* A round button with a symbol. accent = the theme's accent glass, else plain glass. */
static lv_obj_t * round_button(lv_obj_t * parent, const char * sym, int32_t size, bool accent)
{
    lv_obj_t * b = lv_button_create(parent);
    lv_obj_set_size(b, size, size);
    lv_obj_set_style_radius(b, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_pad_all(b, 0, 0);
    if(!accent) lv_glass_set_variant(b, LV_GLASS_VARIANT_PANEL);
    lv_obj_t * l = lv_label_create(b);
    lv_label_set_text(l, sym);
    lv_obj_set_style_text_font(l, size >= DPX(56) ? F28 : (size >= DPX(40) ? F20 : F14), 0);
    lv_obj_center(l);
    return b;
}

/* A pill button with optional symbol and text. */
static lv_obj_t * pill(lv_obj_t * parent, const char * txt, bool accent)
{
    lv_obj_t * b = lv_button_create(parent);
    lv_obj_set_height(b, s_small ? DPX(30) : DPX(36));
    lv_obj_set_style_radius(b, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_pad_hor(b, DPX(16), 0);
    lv_obj_set_style_pad_ver(b, 0, 0);
    if(!accent) lv_glass_set_variant(b, LV_GLASS_VARIANT_PANEL);
    lv_obj_t * l = lv_label_create(b);
    lv_label_set_text(l, txt);
    lv_obj_set_style_text_font(l, F14, 0);
    lv_obj_center(l);
    return b;
}

/* A filled circle, used for status dots and weather icons. */
static lv_obj_t * dot(lv_obj_t * parent, int32_t size, lv_color_t c, lv_opa_t opa)
{
    lv_obj_t * d = lv_obj_create(parent);
    lv_glass_remove(d);
    lv_obj_set_size(d, size, size);
    lv_obj_set_style_radius(d, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(d, c, 0);
    lv_obj_set_style_bg_opa(d, opa, 0);
    /* the theme's panel gloss is a gradient; a plain dot has none */
    lv_obj_set_style_bg_grad_dir(d, LV_GRAD_DIR_NONE, 0);
    lv_obj_set_style_border_width(d, 0, 0);
    NO_CLICK(d);
    NO_SCROLL(d);
    return d;
}

/* A symbol in a tinted circle, like an app's setting icon. */
static lv_obj_t * badge(lv_obj_t * parent, const char * sym, lv_color_t c, int32_t size)
{
    lv_obj_t * d = dot(parent, size, c, LV_OPA_COVER);
    lv_obj_t * l = lv_label_create(d);
    lv_label_set_text(l, sym);
    lv_obj_set_style_text_color(l, lv_color_white(), 0);
    lv_obj_set_style_text_font(l, size >= DPX(40) ? F20 : F14, 0);
    lv_obj_center(l);
    return d;
}

static lv_obj_t * new_screen(lv_flex_flow_t flow)
{
    lv_obj_t * scr = lv_obj_create(NULL);
    lv_obj_set_style_pad_all(scr, PAD, 0);
    lv_obj_set_style_pad_gap(scr, GAP, 0);
    lv_obj_set_flex_flow(scr, flow);
    NO_SCROLL(scr);
    return scr;
}

/* width of a screen's content area */
#define CONTENT_W (s_w - 2 * PAD)
#define CONTENT_H (s_h - 2 * PAD)

/* --------------------------------------------------------------- player */

static void build_player(lv_obj_t * scr)
{
    lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(scr, s_small ? DPX(14) : DPX(24), 0);

    int32_t art = LV_MIN(CONTENT_H * 82 / 100, CONTENT_W * 40 / 100);
    lv_obj_t * a = cover(scr, art, pal(C_ROSE), pal(C_VIOLET), LV_SYMBOL_AUDIO);
    lv_obj_set_style_shadow_width(a, DPX(40), 0);
    lv_obj_set_style_shadow_color(a, pal(C_VIOLET), 0);
    lv_obj_set_style_shadow_opa(a, LV_OPA_40, 0);
    lv_obj_set_style_shadow_offset_y(a, DPX(12), 0);

    lv_obj_t * p = panel(scr, CONTENT_W - art - lv_obj_get_style_pad_column(scr, 0), LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(p, s_small ? DPX(12) : DPX(20), 0);
    lv_obj_set_style_pad_row(p, s_small ? DPX(6) : DPX(10), 0);

    lv_obj_t * head = spread_row(p, LV_PCT(100));
    text(head, "NOW PLAYING", F12, true);
    text(head, LV_SYMBOL_BLUETOOTH "  Desk speaker", F12, true);

    text(p, "Midnight Static", s_small ? F20 : F28, false);
    text(p, "The Lumen Set  \xE2\x80\xA2  Glasshouse", F16, true);

    lv_obj_t * prog = lv_slider_create(p);
    lv_obj_set_width(prog, LV_PCT(100));
    lv_slider_set_value(prog, 43, LV_ANIM_OFF);
    lv_obj_t * times = spread_row(p, LV_PCT(100));
    text(times, "1:42", F12, true);
    text(times, "-2:16", F12, true);

    lv_obj_t * ctl = centre_row(p, LV_PCT(100), s_small ? DPX(10) : DPX(18));
    round_button(ctl, LV_SYMBOL_SHUFFLE, DPX(36), false);
    round_button(ctl, LV_SYMBOL_PREV, DPX(48), false);
    round_button(ctl, LV_SYMBOL_PAUSE, s_small ? DPX(52) : DPX(64), true);
    round_button(ctl, LV_SYMBOL_NEXT, DPX(48), false);
    round_button(ctl, LV_SYMBOL_LOOP, DPX(36), false);

    lv_obj_t * vol = box(p, LV_PCT(100), LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW, DPX(12));
    lv_obj_set_flex_align(vol, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    text(vol, LV_SYMBOL_VOLUME_MID, F16, true);
    lv_obj_t * vs = lv_slider_create(vol);
    lv_obj_set_flex_grow(vs, 1);
    lv_slider_set_value(vs, 68, LV_ANIM_OFF);
    text(vol, LV_SYMBOL_VOLUME_MAX, F16, true);
}

/* ----------------------------------------------------------- smart home */

typedef struct {
    const char * sym;
    const char * name;
    const char * state;
    int colour;
    bool on;
} device_t;

static void build_home(lv_obj_t * scr)
{
    lv_obj_t * head = spread_row(scr, CONTENT_W);
    lv_obj_t * title = box(head, LV_SIZE_CONTENT, LV_SIZE_CONTENT, LV_FLEX_FLOW_COLUMN, DPX(2));
    text(title, "Living room", s_small ? F20 : F28, false);
    text(title, s_small ? "4 devices on" : "4 of 6 devices on  \xE2\x80\xA2  21.5\xC2\xB0 inside  \xE2\x80\xA2  48% humidity",
         F14, true);
    pill(head, LV_SYMBOL_POWER "  All off", false);

    int32_t scenes_h = s_small ? DPX(30) : DPX(36);
    int32_t head_h = s_small ? DPX(44) : DPX(56);
    int32_t body_h = CONTENT_H - head_h - scenes_h - 2 * GAP;
    lv_obj_t * body = box(scr, CONTENT_W, body_h, LV_FLEX_FLOW_ROW, GAP);

    /* device tiles, three across and two down */
    int32_t grid_w = (CONTENT_W - GAP) * 60 / 100;
    lv_obj_t * grid = box(body, grid_w, body_h, LV_FLEX_FLOW_ROW_WRAP, GAP);
    static const device_t dev[] = {
        {LV_SYMBOL_CHARGE, "Ceiling light", "On  \xE2\x80\xA2  80%", C_AMBER, true},
        {LV_SYMBOL_EYE_OPEN, "Floor lamp", "On  \xE2\x80\xA2  warm", C_CORAL, true},
        {LV_SYMBOL_UP, "Blinds", "60% open", C_AZURE, true},
        {LV_SYMBOL_VIDEO, "TV", "Off", C_VIOLET, false},
        {LV_SYMBOL_AUDIO, "Speaker", "Playing", C_MINT, true},
        {LV_SYMBOL_WIFI, "Air purifier", "Off", C_ROSE, false},
    };
    int cols = s_small ? 2 : 3;
    int32_t tile_w = (grid_w - (cols - 1) * GAP) / cols;
    int32_t tile_h = (body_h - GAP) / 2;
    for(int i = 0; i < 2 * cols; i++) {
        lv_obj_t * t = panel(grid, tile_w, tile_h);
        if(dev[i].on) lv_glass_set_variant(t, LV_GLASS_VARIANT_ACCENT);
        lv_obj_set_flex_align(t, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_t * top = spread_row(t, LV_PCT(100));
        badge(top, dev[i].sym, dev[i].on ? pal(dev[i].colour) : lv_glass_get_muted_color(), s_small ? DPX(30) : DPX(38));
        lv_obj_t * sw = lv_switch_create(top);
        lv_obj_set_size(sw, DPX(40), DPX(22));
        if(dev[i].on) lv_obj_add_state(sw, LV_STATE_CHECKED);
        lv_obj_t * txt = box(t, LV_PCT(100), LV_SIZE_CONTENT, LV_FLEX_FLOW_COLUMN, DPX(2));
        text(txt, dev[i].name, s_small ? F14 : F16, false);
        text(txt, dev[i].state, F12, true);
    }

    /* thermostat */
    lv_obj_t * th = panel(body, CONTENT_W - grid_w - GAP, body_h);
    lv_obj_set_flex_align(th, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_t * th_head = spread_row(th, LV_PCT(100));
    text(th_head, "Thermostat", F16, false);
    coloured(th_head, s_small ? LV_SYMBOL_CHARGE : LV_SYMBOL_CHARGE " Heating", F12, pal(C_CORAL));

    int32_t th_w = CONTENT_W - grid_w - GAP - 2 * (s_small ? DPX(10) : DPX(14));
    int32_t arc_d = LV_MIN(th_w * 80 / 100, body_h - (s_small ? DPX(90) : DPX(120)));
    lv_obj_t * arc = lv_arc_create(th);
    lv_obj_set_size(arc, arc_d, arc_d);
    lv_arc_set_range(arc, 10, 30);
    lv_arc_set_value(arc, 22);
    lv_obj_t * centre = box(arc, LV_SIZE_CONTENT, LV_SIZE_CONTENT, LV_FLEX_FLOW_COLUMN, 0);
    lv_obj_set_flex_align(centre, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_center(centre);
    text(centre, "21.5\xC2\xB0", s_small ? F28 : F36, false);
    text(centre, "Target 22\xC2\xB0", F12, true);

    lv_obj_t * btns = centre_row(th, LV_PCT(100), DPX(10));
    round_button(btns, LV_SYMBOL_MINUS, s_small ? DPX(32) : DPX(40), false);
    pill(btns, "Auto", true);
    round_button(btns, LV_SYMBOL_PLUS, s_small ? DPX(32) : DPX(40), false);

    /* scenes */
    lv_obj_t * scenes = box(scr, CONTENT_W, scenes_h, LV_FLEX_FLOW_ROW, DPX(10));
    lv_obj_set_flex_align(scenes, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    pill(scenes, LV_SYMBOL_EYE_OPEN "  Morning", false);
    pill(scenes, LV_SYMBOL_VIDEO "  Movie night", true);
    pill(scenes, LV_SYMBOL_HOME "  Away", false);
    if(!s_small) pill(scenes, LV_SYMBOL_EYE_CLOSE "  Sleep", false);
}

/* ------------------------------------------------------------- settings */

static void setting_row(lv_obj_t * parent, const char * name, const char * hint, lv_obj_t ** right)
{
    lv_obj_t * r = spread_row(parent, LV_PCT(100));
    lv_obj_set_style_pad_ver(r, s_small ? DPX(2) : DPX(4), 0);
    lv_obj_t * t = box(r, LV_SIZE_CONTENT, LV_SIZE_CONTENT, LV_FLEX_FLOW_COLUMN, DPX(1));
    text(t, name, s_small ? F14 : F16, false);
    if(hint && !s_small) text(t, hint, F12, true);
    *right = r;
}

static void build_settings(lv_obj_t * scr)
{
    lv_obj_t * list = lv_glass_list_create(scr);
    int32_t list_w = CONTENT_W * 32 / 100;
    lv_obj_set_size(list, list_w, CONTENT_H);
    lv_glass_list_add_header(list, "Settings");
    static const char * syms[] = {LV_SYMBOL_WIFI, LV_SYMBOL_BLUETOOTH, LV_SYMBOL_IMAGE, LV_SYMBOL_VOLUME_MAX,
                                  LV_SYMBOL_BELL, LV_SYMBOL_BATTERY_3, LV_SYMBOL_SETTINGS};
    static const char * names[] = {"Wi-Fi", "Bluetooth", "Display", "Sound", "Notifications", "Battery", "About"};
    int rows = s_small ? 5 : 7;
    for(int i = 0; i < rows; i++) {
        lv_obj_t * r = lv_glass_list_add_row(list, syms[i], names[i]);
        if(i == 2) lv_glass_set_variant(r, LV_GLASS_VARIANT_ACCENT);
    }

    lv_obj_t * p = panel(scr, CONTENT_W - list_w - GAP, CONTENT_H);
    lv_obj_set_style_pad_all(p, s_small ? DPX(12) : DPX(20), 0);
    lv_obj_set_style_pad_row(p, s_small ? DPX(6) : DPX(10), 0);
    text(p, "Display", s_small ? F20 : F28, false);

    lv_obj_t * r;
    setting_row(p, "Brightness", "Also changes with the light sensor", &r);
    lv_obj_t * bright = box(r, LV_PCT(48), LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW, DPX(8));
    lv_obj_set_flex_align(bright, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    text(bright, LV_SYMBOL_EYE_CLOSE, F14, true);
    lv_obj_t * s = lv_slider_create(bright);
    lv_obj_set_flex_grow(s, 1);
    lv_slider_set_value(s, 72, LV_ANIM_OFF);
    text(bright, LV_SYMBOL_EYE_OPEN, F14, true);

    setting_row(p, "Auto-brightness", "Follow the room's light", &r);
    lv_obj_t * sw = lv_switch_create(r);
    lv_obj_add_state(sw, LV_STATE_CHECKED);

    bool dark = lv_glass_get_config()->scheme == LV_GLASS_SCHEME_DARK;
    setting_row(p, "Dark scheme", "Glass tint for night use", &r);
    sw = lv_switch_create(r);
    if(dark) lv_obj_add_state(sw, LV_STATE_CHECKED);

    setting_row(p, "Accent colour", NULL, &r);
    lv_obj_t * dots = box(r, LV_SIZE_CONTENT, LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW, DPX(8));
    int cur = (int)lv_glass_get_config()->accent;
    for(int i = 0; i < 6; i++) {
        lv_obj_t * d = dot(dots, DPX(22), pal(i), LV_OPA_COVER);
        if(i == cur) {
            lv_obj_set_style_border_width(d, DPX(3), 0);
            lv_obj_set_style_border_color(d, lv_glass_get_text_color(), 0);
        }
    }

    setting_row(p, "Sleep after", "Screen turns off when idle", &r);
    lv_obj_t * dd = lv_dropdown_create(r);
    lv_dropdown_set_options_static(dd, "15 seconds\n30 seconds\n1 minute\n5 minutes\nNever");
    lv_dropdown_set_selected(dd, 1);
    lv_obj_set_width(dd, s_small ? DPX(130) : DPX(160));
}

/* ---------------------------------------------------------------- music */

static void build_music(lv_obj_t * scr)
{
    lv_obj_t * head = spread_row(scr, CONTENT_W);
    text(head, "Library", s_small ? F20 : F28, false);
    lv_obj_t * tabs = box(head, LV_SIZE_CONTENT, LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW, DPX(8));
    pill(tabs, "Playlists", true);
    pill(tabs, "Albums", false);
    if(!s_small) pill(tabs, "Artists", false);

    /* album shelf */
    int n = s_small ? 4 : 5;
    lv_obj_t * shelf = box(scr, CONTENT_W, LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW, GAP);
    int32_t card_w = (CONTENT_W - (n - 1) * GAP) / n;
    static const struct { const char * t; const char * a; int c1, c2; const char * sym; } al[] = {
        {"Glasshouse", "The Lumen Set", C_ROSE, C_VIOLET, LV_SYMBOL_AUDIO},
        {"Low Tide", "Harbour Lights", C_AZURE, C_MINT, LV_SYMBOL_TINT},
        {"Night Drive", "Kilo Static", C_VIOLET, C_AZURE, LV_SYMBOL_GPS},
        {"Sunroom", "Mira Vale", C_AMBER, C_CORAL, LV_SYMBOL_IMAGE},
        {"Focus", "Daily mix", C_MINT, C_AZURE, LV_SYMBOL_LIST},
    };
    int32_t inner = s_small ? DPX(8) : DPX(10);
    for(int i = 0; i < n; i++) {
        lv_obj_t * c = panel(shelf, card_w, LV_SIZE_CONTENT);
        lv_obj_set_style_pad_all(c, inner, 0);
        lv_obj_set_style_pad_row(c, DPX(3), 0);
        cover(c, card_w - 2 * inner, pal(al[i].c1), pal(al[i].c2), al[i].sym);
        text(c, al[i].t, s_small ? F12 : F14, false);
        text(c, al[i].a, F12, true);
    }

    /* up next, two columns */
    lv_obj_t * next = panel(scr, CONTENT_W, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(next, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_style_pad_column(next, DPX(24), 0);
    static const char * tracks[][3] = {
        {"2", "Paper Moons", "3:41"}, {"3", "Cold Light", "4:05"}, {"4", "Satellites", "3:12"}, {"5", "Afterglow", "5:20"}};
    for(int i = 0; i < (s_small ? 2 : 4); i++) {
        lv_obj_t * r = box(next, LV_PCT(47), LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW, DPX(12));
        lv_obj_set_flex_align(r, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        text(r, tracks[i][0], F14, true);
        lv_obj_t * t = text(r, tracks[i][1], F14, false);
        lv_obj_set_flex_grow(t, 1);
        text(r, tracks[i][2], F12, true);
    }

    /* mini player */
    lv_obj_t * mini = panel(scr, CONTENT_W, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(mini, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(mini, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(mini, DPX(8), 0);
    lv_obj_set_style_pad_column(mini, DPX(12), 0);
    lv_obj_set_style_radius(mini, DPX(24), 0);
    cover(mini, DPX(40), pal(C_ROSE), pal(C_VIOLET), NULL);
    lv_obj_t * nm = box(mini, LV_SIZE_CONTENT, LV_SIZE_CONTENT, LV_FLEX_FLOW_COLUMN, 0);
    text(nm, "Midnight Static", F14, false);
    text(nm, "The Lumen Set", F12, true);
    lv_obj_t * bar = lv_bar_create(mini);
    lv_obj_set_flex_grow(bar, 1);
    lv_obj_set_height(bar, DPX(6));
    lv_bar_set_value(bar, 43, LV_ANIM_OFF);
    round_button(mini, LV_SYMBOL_PREV, DPX(36), false);
    round_button(mini, LV_SYMBOL_PAUSE, DPX(40), true);
    round_button(mini, LV_SYMBOL_NEXT, DPX(36), false);
}

/* -------------------------------------------------------------- weather */

typedef enum { WX_SUN, WX_PART, WX_CLOUD, WX_RAIN } wx_t;

/* A weather icon from circles: sun, sun behind a cloud, cloud, or cloud with rain drops. */
static lv_obj_t * wx_icon(lv_obj_t * parent, wx_t kind, int32_t size)
{
    lv_obj_t * holder = box(parent, size, size, LV_FLEX_FLOW_ROW, 0);
    lv_obj_set_layout(holder, LV_LAYOUT_NONE);
    if(kind == WX_SUN || kind == WX_PART) {
        int32_t d = kind == WX_SUN ? size * 3 / 4 : size * 3 / 5;
        lv_obj_t * sun = dot(holder, d, pal(C_AMBER), LV_OPA_COVER);
        lv_obj_set_style_bg_grad_color(sun, pal(C_CORAL), 0);
        lv_obj_set_style_bg_grad_dir(sun, LV_GRAD_DIR_VER, 0);
        lv_obj_set_style_bg_main_stop(sun, 0, 0);
        lv_obj_set_style_bg_grad_stop(sun, 255, 0);
        lv_obj_set_style_shadow_width(sun, size / 3, 0);
        lv_obj_set_style_shadow_color(sun, pal(C_AMBER), 0);
        lv_obj_set_style_shadow_opa(sun, LV_OPA_60, 0);
        lv_obj_align(sun, kind == WX_SUN ? LV_ALIGN_CENTER : LV_ALIGN_TOP_RIGHT, 0, 0);
    }
    if(kind != WX_SUN) {
        lv_color_t c = kind == WX_RAIN ? lv_color_hex(0xB8C4D6) : lv_color_hex(0xF4F6FA);
        lv_obj_t * puff = dot(holder, size / 2, c, LV_OPA_COVER);
        lv_obj_align(puff, LV_ALIGN_LEFT_MID, size / 6, size / 16);
        lv_obj_t * base = dot(holder, size * 7 / 8, c, LV_OPA_COVER);
        lv_obj_set_height(base, size * 3 / 8);
        lv_obj_align(base, LV_ALIGN_BOTTOM_MID, 0, kind == WX_RAIN ? -size / 5 : -size / 10);
        lv_obj_t * puff2 = dot(holder, size * 2 / 5, c, LV_OPA_COVER);
        lv_obj_align(puff2, LV_ALIGN_CENTER, size / 8, size / 10);
        if(kind == WX_RAIN) {
            for(int i = 0; i < 3; i++) {
                lv_obj_t * drop = dot(holder, LV_MAX(2, size / 10), pal(C_AZURE), LV_OPA_COVER);
                lv_obj_set_height(drop, LV_MAX(4, size / 6));
                lv_obj_align(drop, LV_ALIGN_BOTTOM_MID, (i - 1) * size / 4, 0);
            }
        }
    }
    return holder;
}

static void build_weather(lv_obj_t * scr)
{
    /* now */
    int32_t now_w = CONTENT_W * 36 / 100;
    lv_obj_t * now = panel(scr, now_w, CONTENT_H);
    lv_obj_set_style_pad_all(now, s_small ? DPX(12) : DPX(18), 0);
    lv_obj_set_flex_align(now, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_t * city = box(now, LV_PCT(100), LV_SIZE_CONTENT, LV_FLEX_FLOW_COLUMN, DPX(2));
    text(city, LV_SYMBOL_GPS "  Lisbon", s_small ? F16 : F20, false);
    text(city, "Tuesday, 13:20", F12, true);

    lv_obj_t * mid = box(now, LV_PCT(100), LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW, DPX(10));
    lv_obj_set_flex_align(mid, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_t * tcol = box(mid, LV_SIZE_CONTENT, LV_SIZE_CONTENT, LV_FLEX_FLOW_COLUMN, 0);
    text(tcol, "24\xC2\xB0", F48, false);
    text(tcol, "Mostly sunny", F16, false);
    text(tcol, "H 27\xC2\xB0   L 18\xC2\xB0", F14, true);
    wx_icon(mid, WX_PART, s_small ? DPX(56) : DPX(84));

    lv_obj_t * stats = box(now, LV_PCT(100), LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW, 0);
    lv_obj_set_flex_align(stats, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    static const char * st[][2] = {{"12 km/h", "Wind"}, {"58%", "Humidity"}, {"6", "UV index"}};
    for(int i = 0; i < 3; i++) {
        lv_obj_t * c = box(stats, LV_SIZE_CONTENT, LV_SIZE_CONTENT, LV_FLEX_FLOW_COLUMN, 0);
        text(c, st[i][0], s_small ? F14 : F16, false);
        text(c, st[i][1], F12, true);
    }

    /* hourly and daily */
    int32_t right_w = CONTENT_W - now_w - GAP;
    lv_obj_t * right = box(scr, right_w, CONTENT_H, LV_FLEX_FLOW_COLUMN, GAP);

    lv_obj_t * hourly = panel(right, right_w, LV_SIZE_CONTENT);
    text(hourly, "Next hours", F12, true);
    lv_obj_t * hrow = spread_row(hourly, LV_PCT(100));
    static const char * hours[] = {"Now", "14:00", "15:00", "16:00", "17:00", "18:00", "19:00"};
    static const char * htemp[] = {"24\xC2\xB0", "25\xC2\xB0", "26\xC2\xB0", "25\xC2\xB0", "23\xC2\xB0", "21\xC2\xB0", "19\xC2\xB0"};
    static const wx_t hwx[] = {WX_PART, WX_SUN, WX_SUN, WX_PART, WX_PART, WX_CLOUD, WX_CLOUD};
    for(int i = 0; i < (s_small ? 5 : 7); i++) {
        lv_obj_t * c = box(hrow, LV_SIZE_CONTENT, LV_SIZE_CONTENT, LV_FLEX_FLOW_COLUMN, DPX(4));
        lv_obj_set_flex_align(c, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        text(c, hours[i], F12, true);
        wx_icon(c, hwx[i], s_small ? DPX(22) : DPX(30));
        text(c, htemp[i], F14, false);
    }

    lv_obj_t * daily = panel(right, right_w, LV_SIZE_CONTENT);
    lv_obj_set_flex_grow(daily, 1);
    lv_obj_set_flex_align(daily, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    static const struct { const char * day; wx_t wx; int lo, hi; } days[] = {
        {"Wed", WX_SUN, 17, 28}, {"Thu", WX_PART, 16, 25}, {"Fri", WX_RAIN, 14, 19}, {"Sat", WX_CLOUD, 15, 22},
        {"Sun", WX_SUN, 18, 29}};
    for(int i = 0; i < (s_small ? 3 : 5); i++) {
        lv_obj_t * r = box(daily, LV_PCT(100), LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW, DPX(12));
        lv_obj_set_flex_align(r, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_t * d = text(r, days[i].day, F14, false);
        lv_obj_set_width(d, DPX(40));
        wx_icon(r, days[i].wx, DPX(24));
        char lo[8], hi[8];
        snprintf(lo, sizeof lo, "%d\xC2\xB0", days[i].lo);
        snprintf(hi, sizeof hi, "%d\xC2\xB0", days[i].hi);
        lv_obj_t * l = text(r, lo, F14, true);
        lv_obj_set_width(l, DPX(32));
        lv_obj_set_style_text_align(l, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_t * bar = lv_bar_create(r);
        lv_obj_set_flex_grow(bar, 1);
        lv_obj_set_height(bar, DPX(6));
        lv_bar_set_range(bar, 12, 30);
        lv_bar_set_mode(bar, LV_BAR_MODE_RANGE);
        lv_bar_set_start_value(bar, days[i].lo, LV_ANIM_OFF);
        lv_bar_set_value(bar, days[i].hi, LV_ANIM_OFF);
        lv_obj_set_style_bg_color(bar, pal(C_AZURE), LV_PART_INDICATOR);
        lv_obj_set_style_bg_grad_color(bar, pal(C_AMBER), LV_PART_INDICATOR);
        lv_obj_set_style_bg_grad_dir(bar, LV_GRAD_DIR_HOR, LV_PART_INDICATOR);
        text(r, hi, F14, false);
    }
}

/* ---------------------------------------------------------------- watch */

static lv_obj_t * ring(lv_obj_t * parent, int32_t d, int32_t width, lv_color_t c, int value)
{
    lv_obj_t * a = lv_arc_create(parent);
    lv_obj_set_size(a, d, d);
    lv_arc_set_rotation(a, 270);
    lv_arc_set_bg_angles(a, 0, 360);
    lv_arc_set_range(a, 0, 100);
    lv_arc_set_value(a, value);
    lv_obj_remove_style(a, NULL, LV_PART_KNOB);
    NO_CLICK(a);
    lv_obj_set_style_pad_all(a, 0, 0);
    lv_obj_set_style_arc_width(a, width, LV_PART_MAIN);
    lv_obj_set_style_arc_width(a, width, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(a, c, LV_PART_MAIN);
    lv_obj_set_style_arc_opa(a, LV_OPA_20, LV_PART_MAIN);
    lv_obj_set_style_arc_color(a, c, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(a, true, LV_PART_INDICATOR);
    lv_obj_center(a);
    return a;
}

static void build_watch(lv_obj_t * scr)
{
    lv_obj_set_layout(scr, LV_LAYOUT_NONE);
    lv_obj_set_style_pad_all(scr, 0, 0);
    int32_t d = LV_MIN(s_w, s_h);
    bool tiny = d < 300;
    int32_t rw = tiny ? DPX(9) : DPX(13);
    int32_t step = rw + DPX(4);
    int32_t d0 = d - 2 * DPX(tiny ? 8 : 12);

    /* glass dial behind the rings */
    lv_obj_t * face = lv_obj_create(scr);
    lv_obj_set_size(face, d0 - 6 * step, d0 - 6 * step);
    lv_obj_set_style_radius(face, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_pad_all(face, 0, 0);
    NO_SCROLL(face);
    lv_obj_center(face);

    ring(scr, d0, rw, pal(C_CORAL), 78);
    ring(scr, d0 - 2 * step, rw, pal(C_MINT), 56);
    ring(scr, d0 - 4 * step, rw, pal(C_AZURE), 90);

    lv_obj_t * c = box(face, LV_SIZE_CONTENT, LV_SIZE_CONTENT, LV_FLEX_FLOW_COLUMN, tiny ? 0 : DPX(2));
    lv_obj_set_flex_align(c, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_center(c);
    text(c, "TUE 11 NOV", tiny ? F12 : F14, true);
    text(c, "09:41", tiny ? F28 : F48, false);
    lv_obj_t * steps = text(c, LV_SYMBOL_SHUFFLE " 8,412 steps", tiny ? F12 : F14, false);
    lv_obj_set_style_text_color(steps, pal(C_MINT), 0);
    if(!tiny) {
        lv_obj_t * comp = box(c, LV_SIZE_CONTENT, LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW, DPX(8));
        lv_obj_set_style_pad_top(comp, DPX(8), 0);
        lv_obj_t * hr = pill(comp, LV_SYMBOL_CHARGE " 72", false);
        lv_obj_set_height(hr, DPX(28));
        lv_obj_set_style_pad_hor(hr, DPX(10), 0);
        lv_obj_t * bt = pill(comp, LV_SYMBOL_BATTERY_3 " 86%", false);
        lv_obj_set_height(bt, DPX(28));
        lv_obj_set_style_pad_hor(bt, DPX(10), 0);
    }
}

/* --------------------------------------------------------------- charts */

static void build_charts(lv_obj_t * scr)
{
    lv_obj_t * head = spread_row(scr, CONTENT_W);
    lv_obj_t * title = box(head, LV_SIZE_CONTENT, LV_SIZE_CONTENT, LV_FLEX_FLOW_COLUMN, DPX(2));
    text(title, "Energy", s_small ? F20 : F28, false);
    text(title, "This week, in kWh", F14, true);
    lv_obj_t * seg = lv_buttonmatrix_create(head);
    static const char * map[] = {"Day", "Week", "Month", ""};
    lv_buttonmatrix_set_map(seg, map);
    lv_buttonmatrix_set_button_ctrl_all(seg, LV_BUTTONMATRIX_CTRL_CHECKABLE);
    lv_buttonmatrix_set_one_checked(seg, true);
    lv_buttonmatrix_set_button_ctrl(seg, 1, LV_BUTTONMATRIX_CTRL_CHECKED);
    lv_obj_set_size(seg, s_small ? DPX(200) : DPX(260), s_small ? DPX(34) : DPX(40));

    /* stat cards */
    lv_obj_t * cards = box(scr, CONTENT_W, LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW, GAP);
    static const struct { const char * sym; const char * name; const char * val; const char * delta; int c; } sc[] = {
        {LV_SYMBOL_CHARGE, "Solar", "18.4", "+12% on last week", C_AMBER},
        {LV_SYMBOL_HOME, "Home use", "24.5", "-8% on last week", C_AZURE},
        {LV_SYMBOL_BATTERY_FULL, "Battery", "86%", "Full by 15:10", C_MINT},
    };
    int32_t cw = (CONTENT_W - 2 * GAP) / 3;
    for(int i = 0; i < 3; i++) {
        lv_obj_t * c = panel(cards, cw, LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(c, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(c, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_column(c, DPX(12), 0);
        badge(c, sc[i].sym, pal(sc[i].c), s_small ? DPX(30) : DPX(40));
        lv_obj_t * t = box(c, LV_SIZE_CONTENT, LV_SIZE_CONTENT, LV_FLEX_FLOW_COLUMN, 0);
        text(t, sc[i].name, F12, true);
        text(t, sc[i].val, s_small ? F20 : F24, false);
        if(!s_small) coloured(t, sc[i].delta, F12, i == 0 ? pal(C_MINT) : lv_glass_get_muted_color());
    }

    /* line chart and gauge */
    int32_t head_h = s_small ? DPX(44) : DPX(56);
    int32_t cards_h = s_small ? DPX(54) : DPX(78);
    int32_t body_h = CONTENT_H - head_h - cards_h - 2 * GAP;
    lv_obj_t * body = box(scr, CONTENT_W, body_h, LV_FLEX_FLOW_ROW, GAP);
    int32_t chart_w = (CONTENT_W - GAP) * 66 / 100;

    lv_obj_t * cp = panel(body, chart_w, body_h);
    lv_obj_t * legend = box(cp, LV_PCT(100), LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW, DPX(16));
    lv_obj_set_flex_align(legend, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    coloured(legend, LV_SYMBOL_MINUS " Solar", F12, pal(C_AMBER));
    coloured(legend, LV_SYMBOL_MINUS " Home use", F12, pal(C_AZURE));

    lv_obj_t * chart = lv_chart_create(cp);
    lv_obj_set_width(chart, LV_PCT(100));
    lv_obj_set_flex_grow(chart, 1);
    lv_chart_set_type(chart, LV_CHART_TYPE_LINE);
    lv_chart_set_point_count(chart, 7);
    lv_chart_set_div_line_count(chart, 4, 7);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 50);
    lv_obj_set_style_line_width(chart, DPX(3), LV_PART_ITEMS);
    lv_obj_set_style_size(chart, DPX(7), DPX(7), LV_PART_INDICATOR);
    lv_chart_series_t * solar = lv_chart_add_series(chart, pal(C_AMBER), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t * use = lv_chart_add_series(chart, pal(C_AZURE), LV_CHART_AXIS_PRIMARY_Y);
    static const int32_t vs[] = {14, 22, 19, 31, 27, 38, 34};
    static const int32_t vu[] = {26, 24, 29, 23, 30, 25, 21};
    for(int i = 0; i < 7; i++) {
        lv_chart_set_next_value(chart, solar, vs[i]);
        lv_chart_set_next_value(chart, use, vu[i]);
    }
    lv_obj_t * days = spread_row(cp, LV_PCT(100));
    static const char * dn[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
    for(int i = 0; i < 7; i++) text(days, dn[i], F12, true);

    lv_obj_t * gp = panel(body, CONTENT_W - chart_w - GAP, body_h);
    lv_obj_set_flex_align(gp, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    text(gp, "Self-powered", F14, false);
    int32_t gw = CONTENT_W - chart_w - GAP - 2 * (s_small ? DPX(10) : DPX(14));
    int32_t gd = LV_MIN(gw * 88 / 100, body_h - (s_small ? DPX(60) : DPX(84)));
    lv_obj_t * holder = box(gp, gd, gd, LV_FLEX_FLOW_ROW, 0);
    lv_obj_set_layout(holder, LV_LAYOUT_NONE);

    lv_obj_t * scale = lv_scale_create(holder);
    lv_obj_set_size(scale, gd, gd);
    lv_scale_set_mode(scale, LV_SCALE_MODE_ROUND_INNER);
    lv_scale_set_range(scale, 0, 100);
    lv_scale_set_angle_range(scale, 270);
    lv_scale_set_rotation(scale, 135);
    lv_scale_set_total_tick_count(scale, 21);
    lv_scale_set_major_tick_every(scale, 5);
    lv_scale_set_label_show(scale, false);
    lv_obj_set_style_length(scale, DPX(4), LV_PART_ITEMS);
    lv_obj_set_style_length(scale, DPX(8), LV_PART_INDICATOR);
    lv_obj_set_style_line_color(scale, lv_glass_get_muted_color(), LV_PART_ITEMS);
    lv_obj_set_style_line_color(scale, lv_glass_get_text_color(), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(scale, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(scale, DPX(16), 0);
    lv_obj_center(scale);

    lv_obj_t * arc = lv_arc_create(holder);
    lv_obj_set_size(arc, gd, gd);
    lv_arc_set_range(arc, 0, 100);
    lv_arc_set_value(arc, 72);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
    NO_CLICK(arc);
    lv_obj_set_style_arc_width(arc, DPX(10), LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc, DPX(10), LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, pal(C_MINT), LV_PART_INDICATOR);
    lv_obj_center(arc);

    lv_obj_t * v = box(holder, LV_SIZE_CONTENT, LV_SIZE_CONTENT, LV_FLEX_FLOW_COLUMN, 0);
    lv_obj_set_flex_align(v, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_center(v);
    text(v, "72%", s_small ? F20 : F28, false);
    text(v, "of the week", F12, true);
    text(gp, "Grid import 6.1 kWh", F12, true);
}

/* ----------------------------------------------------------------- API */

static const char * const NAMES[] = {"player", "smart-home", "settings", "music", "weather", "watch", "charts"};

const char * glassvgl_showcase_name(glassvgl_showcase_t which)
{
    return which < GLASSVGL_SHOWCASE_COUNT ? NAMES[which] : "?";
}

lv_obj_t * glassvgl_showcase_show(glassvgl_showcase_t which)
{
    lv_display_t * disp = lv_display_get_default();
    s_w = lv_display_get_horizontal_resolution(disp);
    s_h = lv_display_get_vertical_resolution(disp);
    s_small = s_h < 400;

    lv_flex_flow_t flow = which == GLASSVGL_SHOWCASE_PLAYER || which == GLASSVGL_SHOWCASE_SETTINGS
                                  || which == GLASSVGL_SHOWCASE_WEATHER
                              ? LV_FLEX_FLOW_ROW
                              : LV_FLEX_FLOW_COLUMN;
    lv_obj_t * scr = new_screen(flow);
    switch(which) {
        case GLASSVGL_SHOWCASE_PLAYER: build_player(scr); break;
        case GLASSVGL_SHOWCASE_SMART_HOME: build_home(scr); break;
        case GLASSVGL_SHOWCASE_SETTINGS: build_settings(scr); break;
        case GLASSVGL_SHOWCASE_MUSIC: build_music(scr); break;
        case GLASSVGL_SHOWCASE_WEATHER: build_weather(scr); break;
        case GLASSVGL_SHOWCASE_WATCH: build_watch(scr); break;
        case GLASSVGL_SHOWCASE_CHARTS: build_charts(scr); break;
        default: break;
    }
    lv_screen_load(scr);
    return scr;
}

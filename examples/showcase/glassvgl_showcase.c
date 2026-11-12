/**
 * @file glassvgl_showcase.c
 * App-style sample screens for the GlassVGL theme. See glassvgl_showcase.h.
 *
 * Only the public GlassVGL API and stock LVGL 9 widgets are used. Every panel, button, slider,
 * switch, arc, bar, chart and list picks up its glass look from the theme; the code below lays
 * things out on an 8 px grid, sets type from one scale and picks a few colours.
 */
#include "glassvgl_showcase.h"
#include <math.h>
#include "fonts/gv_fonts.h"
#include "fonts/gv_icons.h"
#include <stdio.h>

/* ------------------------------------------------------------ type scale */

#define T_CAPTION (&gv_font_12)  /* Inter Medium 12 */
#define T_FOOT    (&gv_font_14)  /* Inter Medium 14 */
#define T_BODY    (&gv_font_16)  /* Inter Medium 16 */
#define T_HEAD    (&gv_font_16b) /* Inter SemiBold 16 */
#define T_TITLE3  (&gv_font_20)  /* Inter SemiBold 20 */
#define T_TITLE2  (&gv_font_24)  /* Inter Display SemiBold 24 */
#define T_TITLE1  (&gv_font_32)  /* Inter Display SemiBold 32 */
#define T_DISPLAY (&gv_font_48)  /* Inter Display Light 48, digits and icons only */
#define T_HUGE    (&gv_font_72)  /* Inter Display Thin 72, digits only */

/* LVGL 9.6 added one setter per flag and deprecated lv_obj_add_flag()/lv_obj_remove_flag(). */
#if LVGL_VERSION_MAJOR > 9 || (LVGL_VERSION_MAJOR == 9 && LVGL_VERSION_MINOR >= 6)
#define NO_SCROLL(o) lv_obj_set_scrollable((o), false)
#define NO_CLICK(o)  lv_obj_set_clickable((o), false)
#define OVERFLOW(o)  lv_obj_set_overflow_visible((o), true)
#else
#define NO_SCROLL(o) lv_obj_remove_flag((o), LV_OBJ_FLAG_SCROLLABLE)
#define NO_CLICK(o)  lv_obj_remove_flag((o), LV_OBJ_FLAG_CLICKABLE)
#define OVERFLOW(o)  lv_obj_add_flag((o), LV_OBJ_FLAG_OVERFLOW_VISIBLE)
#endif

/* A 4/8 px grid in plain pixels, like design points at 1x. The fonts are fixed pixel sizes, so
 * the spacing is too; scaling it by the panel's DPI would pull it away from the text. */
#define U(n) ((int32_t)(n) * 4)
#define SCREEN_PAD U(6) /* 24 */
#define GAP        U(4) /* 16 */
#define PANEL_PAD  U(5) /* 20 */
#define RADIUS     U(6) /* 24 */

static int32_t s_w, s_h;
#define CONTENT_W (s_w - 2 * SCREEN_PAD)
#define CONTENT_H (s_h - 2 * SCREEN_PAD)

/* System-style colours; the dark scheme uses brighter night versions */
enum { C_BLUE, C_GREEN, C_RED, C_ORANGE, C_YELLOW, C_PINK, C_PURPLE, C_TEAL, C_GRAY };
static const uint32_t DAY[] = {0x007AFF, 0x34C759, 0xFF3B30, 0xFF9500, 0xFFCC00, 0xFF2D55, 0xAF52DE, 0x30B0C7, 0x8E8E93};
static const uint32_t NIGHT[] = {0x0A84FF, 0x30D158, 0xFF453A, 0xFF9F0A, 0xFFD60A, 0xFF375F, 0xBF5AF2, 0x40C8E0, 0x98989D};

static bool s_dark;
static lv_color_t sys(int c)
{
    return lv_color_hex((s_dark ? NIGHT : DAY)[c]);
}

/* chart series colours, checked for colour-blind separation against each scheme's surface */
static lv_color_t series(int i)
{
    static const uint32_t day[] = {0xC98A0E, 0x2F7CF6};
    static const uint32_t night[] = {0xC8851A, 0x4D8FF5};
    return lv_color_hex((s_dark ? night : day)[i]);
}

/* ---------------------------------------------------------------- helpers */

/* A plain layout container: no glass, no padding, no scrolling. */
/* A layout box with overflow visible still clips its children to its own extended area, so it
 * reserves room for the soft shadow of the glass panels inside it. */
static void shadow_room_cb(lv_event_t * e)
{
    lv_event_set_ext_draw_size(e, U(6));
}

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
    OVERFLOW(b); /* let the panels' shadows spill past the layout box */
    lv_obj_add_event_cb(b, shadow_room_cb, LV_EVENT_REFR_EXT_DRAW_SIZE, NULL);
    lv_obj_refresh_ext_draw_size(b);
    return b;
}

static lv_obj_t * row(lv_obj_t * parent, int32_t w, int32_t gap, lv_flex_align_t main)
{
    lv_obj_t * r = box(parent, w, LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW, gap);
    /* A row as wide as its content has nothing to align, and on LVGL 9.3 an end or centre alignment
     * there pushes the children left of the row before its width is known, so it comes out too narrow. */
    if(w == LV_SIZE_CONTENT) main = LV_FLEX_ALIGN_START;
    lv_obj_set_flex_align(r, main, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    return r;
}

static lv_obj_t * column(lv_obj_t * parent, int32_t w, int32_t gap)
{
    return box(parent, w, LV_SIZE_CONTENT, LV_FLEX_FLOW_COLUMN, gap);
}

/* A frosted glass panel laid out as a column. */
static lv_obj_t * panel(lv_obj_t * parent, int32_t w, int32_t h)
{
    lv_obj_t * p = lv_obj_create(parent);
    lv_obj_set_size(p, w, h);
    lv_obj_set_flex_flow(p, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_radius(p, RADIUS, 0);
    lv_obj_set_style_pad_all(p, PANEL_PAD, 0);
    lv_obj_set_style_pad_gap(p, U(3), 0);
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

/* Text in a set colour, for text that sits on its own coloured background. */
static lv_obj_t * coloured(lv_obj_t * parent, const char * txt, const lv_font_t * font, lv_color_t c)
{
    lv_obj_t * l = text(parent, txt, font, false);
    lv_obj_set_style_text_color(l, c, 0);
    return l;
}

static float luminance(lv_color_t c)
{
    const uint8_t ch[3] = {c.red, c.green, c.blue};
    float l[3];
    for(int i = 0; i < 3; i++) {
        float v = ch[i] / 255.0f;
        l[i] = v <= 0.04045f ? v / 12.92f : powf((v + 0.055f) / 1.055f, 2.4f);
    }
    return 0.2126f * l[0] + 0.7152f * l[1] + 0.0722f * l[2];
}

static float contrast(lv_color_t a, lv_color_t b)
{
    float la = luminance(a), lb = luminance(b);
    return la > lb ? (la + 0.05f) / (lb + 0.05f) : (lb + 0.05f) / (la + 0.05f);
}

/* Coloured text on glass, kept at 4.5:1 or more (WCAG AA): a bright system colour is deepened on
 * light glass (orange #FF9500 becomes about #A06000) and lifted on dark glass, keeping its hue. */
static lv_color_t on_glass(lv_color_t c, float target)
{
    const lv_color_t glass = lv_color_hex(s_dark ? 0x22263A : 0xF4F5F9); /* typical glass behind text */
    lv_color_t t = c;
    for(int k = 13; k <= 153 && contrast(t, glass) < target; k += 13)
        t = s_dark ? lv_color_lighten(c, (lv_opa_t)k) : lv_color_darken(c, (lv_opa_t)k);
    return t;
}

static lv_obj_t * tinted(lv_obj_t * parent, const char * txt, const lv_font_t * font, lv_color_t c)
{
    return coloured(parent, txt, font, on_glass(c, 4.5f));
}

/* A coloured icon on glass: graphics need 3:1 (WCAG 1.4.11), so it keeps more of its colour. */
static lv_obj_t * tinted_icon(lv_obj_t * parent, const char * icon, const lv_font_t * font, lv_color_t c)
{
    return coloured(parent, icon, font, on_glass(c, 3.0f));
}

/* A thin rule between rows of a grouped panel. */
static void hairline(lv_obj_t * parent)
{
    lv_obj_t * l = box(parent, LV_PCT(100), 1, LV_FLEX_FLOW_ROW, 0);
    lv_obj_set_style_bg_color(l, lv_glass_get_text_color(), 0);
    lv_obj_set_style_bg_opa(l, s_dark ? 40 : 28, 0);
    lv_obj_set_style_bg_grad_dir(l, LV_GRAD_DIR_NONE, 0);
}

/* A filled circle or rounded square. */
static lv_obj_t * shape(lv_obj_t * parent, int32_t size, int32_t radius, lv_color_t c, lv_opa_t opa)
{
    lv_obj_t * d = lv_obj_create(parent);
    lv_glass_remove(d);
    lv_obj_set_size(d, size, size);
    lv_obj_set_style_radius(d, radius, 0);
    lv_obj_set_style_bg_color(d, c, 0);
    lv_obj_set_style_bg_opa(d, opa, 0);
    /* lv_glass_remove() leaves the panel's gloss gradient settings behind; a plain shape has none */
    lv_obj_set_style_bg_grad_dir(d, LV_GRAD_DIR_NONE, 0);
    lv_obj_set_style_border_width(d, 0, 0);
    lv_obj_set_style_pad_all(d, 0, 0);
    NO_CLICK(d);
    NO_SCROLL(d);
    return d;
}

/* An icon on a filled shape, like an app or a setting icon. */
static lv_obj_t * icon_on(lv_obj_t * parent, const char * icon, const lv_font_t * font, int32_t size, int32_t radius,
                          lv_color_t bg, lv_opa_t opa, lv_color_t fg)
{
    lv_obj_t * d = shape(parent, size, radius, bg, opa);
    lv_obj_t * l = coloured(d, icon, font, fg);
    lv_obj_center(l);
    return d;
}

/* A round glass button with an icon. accent = the theme's accent glass, else plain glass. */
static lv_obj_t * round_button(lv_obj_t * parent, const char * icon, int32_t size, bool accent)
{
    lv_obj_t * b = lv_button_create(parent);
    lv_obj_set_size(b, size, size);
    lv_obj_set_style_radius(b, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_pad_all(b, 0, 0);
    if(!accent) lv_glass_set_variant(b, LV_GLASS_VARIANT_PANEL);
    lv_obj_t * l = text(b, icon, size >= U(14) ? T_TITLE2 : (size >= U(10) ? T_TITLE3 : T_BODY), false);
    lv_obj_center(l);
    return b;
}

/* An icon with no button around it, for controls that sit quietly beside the main ones. */
static lv_obj_t * glyph(lv_obj_t * parent, const char * icon, const lv_font_t * font, bool muted)
{
    return text(parent, icon, font, muted);
}

/* A pill button: optional icon, then text. */
static lv_obj_t * pill(lv_obj_t * parent, const char * icon, const char * txt, bool accent)
{
    lv_obj_t * b = lv_button_create(parent);
    lv_obj_set_height(b, U(10));
    lv_obj_set_width(b, LV_SIZE_CONTENT);
    lv_obj_set_style_radius(b, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_pad_hor(b, U(4), 0);
    lv_obj_set_style_pad_ver(b, 0, 0);
    lv_obj_set_style_pad_column(b, U(2), 0);
    lv_obj_set_flex_flow(b, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(b, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    if(!accent) lv_glass_set_variant(b, LV_GLASS_VARIANT_PANEL);
    if(icon) text(b, icon, T_BODY, false);
    if(txt) text(b, txt, T_FOOT, false);
    return b;
}

/* Segmented control on a buttonmatrix, one item selected. */
static lv_obj_t * segmented(lv_obj_t * parent, const char * const * map, uint32_t selected, int32_t w)
{
    lv_obj_t * seg = lv_buttonmatrix_create(parent);
    lv_buttonmatrix_set_map(seg, map);
    lv_buttonmatrix_set_button_ctrl_all(seg, LV_BUTTONMATRIX_CTRL_CHECKABLE);
    lv_buttonmatrix_set_one_checked(seg, true);
    lv_buttonmatrix_set_button_ctrl(seg, selected, LV_BUTTONMATRIX_CTRL_CHECKED);
    lv_obj_set_size(seg, w, U(10));
    lv_obj_set_style_radius(seg, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_radius(seg, LV_RADIUS_CIRCLE, LV_PART_ITEMS);
    lv_obj_set_style_pad_all(seg, U(1), 0);
    lv_obj_set_style_pad_gap(seg, U(1), 0);
    lv_obj_set_style_text_font(seg, T_FOOT, LV_PART_ITEMS);
    return seg;
}

/* A slider with an icon at each end. */
static lv_obj_t * icon_slider(lv_obj_t * parent, const char * lo, const char * hi, int value)
{
    lv_obj_t * r = row(parent, LV_PCT(100), U(3), LV_FLEX_ALIGN_START);
    glyph(r, lo, T_BODY, true);
    lv_obj_t * s = lv_slider_create(r);
    lv_obj_set_flex_grow(s, 1);
    lv_slider_set_value(s, value, LV_ANIM_OFF);
    glyph(r, hi, T_TITLE3, true);
    return s;
}

static lv_obj_t * new_screen(lv_flex_flow_t flow)
{
    lv_obj_t * scr = lv_obj_create(NULL);
    lv_obj_set_style_pad_all(scr, SCREEN_PAD, 0);
    lv_obj_set_style_pad_gap(scr, GAP, 0);
    lv_obj_set_flex_flow(scr, flow);
    NO_SCROLL(scr);
    return scr;
}

/* A title with a line under it, top left of a screen. */
static lv_obj_t * screen_title(lv_obj_t * parent, const char * title, const char * sub)
{
    lv_obj_t * c = column(parent, LV_SIZE_CONTENT, U(1));
    text(c, title, T_TITLE1, false);
    if(sub) text(c, sub, T_FOOT, true);
    return c;
}

/* Album art made of a gradient and two soft light discs, no image needed. */
static lv_obj_t * cover(lv_obj_t * parent, int32_t size, uint32_t top, uint32_t bottom, const char * caption)
{
    lv_obj_t * a = lv_obj_create(parent);
    lv_glass_set_variant(a, LV_GLASS_VARIANT_SOLID);
    lv_obj_set_size(a, size, size);
    lv_obj_set_style_radius(a, LV_MAX(U(2), size / 10), 0);
    lv_obj_set_style_border_width(a, 0, 0);
    lv_obj_set_style_bg_opa(a, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(a, lv_color_hex(top), 0);
    lv_obj_set_style_bg_grad_color(a, lv_color_hex(bottom), 0);
    lv_obj_set_style_bg_grad_dir(a, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_bg_main_stop(a, 0, 0);
    lv_obj_set_style_bg_grad_stop(a, 255, 0);
    lv_obj_set_style_clip_corner(a, true, 0);
    lv_obj_set_style_pad_all(a, 0, 0);
    NO_SCROLL(a);
    NO_CLICK(a);

    lv_obj_t * d1 = shape(a, size * 9 / 10, LV_RADIUS_CIRCLE, lv_color_white(), 46);
    lv_obj_align(d1, LV_ALIGN_TOP_RIGHT, size * 3 / 10, -size * 3 / 10);
    lv_obj_t * d2 = shape(a, size * 6 / 10, LV_RADIUS_CIRCLE, lv_color_white(), 30);
    lv_obj_align(d2, LV_ALIGN_BOTTOM_LEFT, -size / 6, size / 6);

    if(caption) {
        lv_obj_t * l = coloured(a, caption, size >= U(60) ? T_TITLE2 : (size >= U(25) ? T_HEAD : T_CAPTION), lv_color_white());
        lv_obj_set_style_text_letter_space(l, 1, 0);
        lv_obj_align(l, LV_ALIGN_BOTTOM_LEFT, size / 12, -size / 14);
    }
    return a;
}

/* --------------------------------------------------------------- player */

static void build_player(lv_obj_t * scr)
{
    lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(scr, U(8), 0);

    int32_t art = LV_MIN(CONTENT_H * 86 / 100, CONTENT_W * 46 / 100);
    lv_obj_t * a = cover(scr, art, 0x6FA6F7, 0x1F4FAE, "GLASSHOUSE");
    lv_obj_set_style_shadow_width(a, U(12), 0);
    lv_obj_set_style_shadow_color(a, lv_color_hex(0x1E3A8A), 0);
    lv_obj_set_style_shadow_opa(a, s_dark ? LV_OPA_60 : LV_OPA_40, 0);
    lv_obj_set_style_shadow_offset_y(a, U(4), 0);

    int32_t pw = CONTENT_W - art - U(8);
    lv_obj_t * p = panel(scr, pw, LV_SIZE_CONTENT);
    lv_obj_set_style_pad_all(p, U(6), 0);
    lv_obj_set_style_pad_row(p, U(2), 0);

    lv_obj_t * head = row(p, LV_PCT(100), U(2), LV_FLEX_ALIGN_SPACE_BETWEEN);
    text(head, "NOW PLAYING", T_CAPTION, true);
    glyph(head, GV_IC_AIRPLAY, T_TITLE3, true);

    lv_obj_t * t = text(p, "Midnight Static", T_TITLE1, false);
    lv_obj_set_style_pad_top(t, U(2), 0);
    text(p, "The Lumen Set \xE2\x80\xA2 Glasshouse", T_BODY, true);

    lv_obj_t * prog = lv_slider_create(p);
    lv_obj_set_width(prog, LV_PCT(100));
    lv_obj_set_style_margin_top(prog, U(4), 0);
    lv_slider_set_value(prog, 43, LV_ANIM_OFF);
    lv_obj_t * times = row(p, LV_PCT(100), 0, LV_FLEX_ALIGN_SPACE_BETWEEN);
    text(times, "1:42", T_CAPTION, true);
    text(times, "\xE2\x88\x92" "2:16", T_CAPTION, true);

    lv_obj_t * ctl = row(p, LV_PCT(100), 0, LV_FLEX_ALIGN_SPACE_BETWEEN);
    lv_obj_set_style_pad_ver(ctl, U(2), 0);
    glyph(ctl, GV_IC_SHUFFLE, T_TITLE3, true);
    round_button(ctl, GV_IC_SKIP_BACK, U(12), false);
    round_button(ctl, GV_IC_PAUSE, U(17), true);
    round_button(ctl, GV_IC_SKIP_FORWARD, U(12), false);
    glyph(ctl, GV_IC_REPEAT, T_TITLE3, true);

    icon_slider(p, GV_IC_VOLUME_1, GV_IC_VOLUME_2, 68);
}

/* ----------------------------------------------------------- smart home */

typedef struct {
    const char * icon;
    const char * name;
    const char * state;
    int colour;
    bool on;
} device_t;

/* A smart home accessory tile: bright and opaque when on, clear glass when off. */
static void device_tile(lv_obj_t * parent, const device_t * d, int32_t w, int32_t h)
{
    lv_obj_t * t = panel(parent, w, h);
    lv_obj_set_style_pad_all(t, U(4), 0);
    lv_obj_set_flex_align(t, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_color_t ink = lv_glass_get_text_color(), sub = lv_glass_get_muted_color();
    if(d->on) {
        lv_obj_set_style_bg_color(t, lv_color_white(), 0);
        lv_obj_set_style_bg_opa(t, 238, 0);
        lv_obj_set_style_bg_grad_dir(t, LV_GRAD_DIR_NONE, 0);
        ink = lv_color_hex(0x1C1C1E);
        sub = lv_color_hex(0x5E5E66);
    }
    icon_on(t, d->icon, T_TITLE3, U(10), LV_RADIUS_CIRCLE, d->on ? sys(d->colour) : ink, d->on ? LV_OPA_COVER : 36,
            d->on ? lv_color_white() : ink);
    lv_obj_t * c = column(t, LV_PCT(100), 0);
    coloured(c, d->name, T_HEAD, ink);
    coloured(c, d->state, T_FOOT, sub);
}

static void build_home(lv_obj_t * scr)
{
    lv_obj_t * head = row(scr, CONTENT_W, GAP, LV_FLEX_ALIGN_SPACE_BETWEEN);
    screen_title(head, "Living Room", "21.5\xC2\xB0 inside \xE2\x80\xA2 48% humidity \xE2\x80\xA2 4 accessories on");
    lv_obj_t * acts = row(head, LV_SIZE_CONTENT, U(2), LV_FLEX_ALIGN_END);
    round_button(acts, GV_IC_PLUS, U(10), false);
    pill(acts, GV_IC_POWER, "All Off", false);

    int32_t scenes_h = U(10);
    int32_t head_h = U(14);
    int32_t body_h = CONTENT_H - head_h - scenes_h - 2 * GAP;
    lv_obj_t * body = box(scr, CONTENT_W, body_h, LV_FLEX_FLOW_ROW, GAP);

    int32_t grid_w = (CONTENT_W - GAP) * 62 / 100;
    lv_obj_t * grid = box(body, grid_w, body_h, LV_FLEX_FLOW_ROW_WRAP, GAP);
    static const device_t dev[] = {
        {GV_IC_LAMP_CEILING, "Ceiling Light", "80%", C_YELLOW, true},
        {GV_IC_LAMP_FLOOR, "Floor Lamp", "Warm \xE2\x80\xA2 40%", C_ORANGE, true},
        {GV_IC_BLINDS, "Blinds", "60% open", C_TEAL, true},
        {GV_IC_TV, "Television", "Off", C_BLUE, false},
        {GV_IC_SPEAKER, "Speaker", "Playing", C_PINK, true},
        {GV_IC_FAN, "Air Purifier", "Off", C_GREEN, false},
    };
    int32_t tile_w = (grid_w - 2 * GAP) / 3;
    int32_t tile_h = (body_h - GAP) / 2;
    for(int i = 0; i < 6; i++) device_tile(grid, &dev[i], tile_w, tile_h);

    /* thermostat */
    lv_obj_t * th = panel(body, CONTENT_W - grid_w - GAP, body_h);
    lv_obj_set_flex_align(th, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_t * th_head = row(th, LV_PCT(100), 0, LV_FLEX_ALIGN_SPACE_BETWEEN);
    text(th_head, "Thermostat", T_HEAD, false);
    tinted(th_head, GV_IC_FLAME " Heating", T_FOOT, sys(C_ORANGE));

    int32_t arc_d = LV_MIN(CONTENT_W - grid_w - GAP - 2 * PANEL_PAD, body_h - 2 * PANEL_PAD - U(5) - U(10) - 2 * U(3));
    lv_obj_t * arc = lv_arc_create(th);
    lv_obj_set_size(arc, arc_d, arc_d);
    lv_arc_set_range(arc, 10, 30);
    lv_arc_set_value(arc, 22);
    lv_obj_set_style_arc_width(arc, U(3), LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc, U(3), LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, sys(C_ORANGE), LV_PART_INDICATOR);
    lv_obj_t * centre = column(arc, LV_SIZE_CONTENT, 0);
    lv_obj_set_flex_align(centre, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_center(centre);
    text(centre, "21.5\xC2\xB0", T_DISPLAY, false);
    text(centre, "Heat to 22\xC2\xB0", T_FOOT, true);

    lv_obj_t * btns = row(th, LV_PCT(100), U(3), LV_FLEX_ALIGN_CENTER);
    round_button(btns, GV_IC_MINUS, U(10), false);
    pill(btns, NULL, "Auto", true);
    round_button(btns, GV_IC_PLUS, U(10), false);

    lv_obj_t * scenes = row(scr, CONTENT_W, U(3), LV_FLEX_ALIGN_START);
    lv_obj_set_height(scenes, scenes_h);
    pill(scenes, GV_IC_SUNRISE, "Good Morning", false);
    pill(scenes, GV_IC_CLAPPERBOARD, "Movie Night", true);
    pill(scenes, GV_IC_HOUSE, "Arrive Home", false);
    pill(scenes, GV_IC_MOON, "Good Night", false);
}

/* ------------------------------------------------------------- settings */

typedef struct {
    const char * icon;
    const char * name;
    int colour;
} setting_t;

/* A grouped settings section: a caption over a glass panel of rows. */
/* The caption sits inside the glass, not on the wallpaper, so it stays readable over any ribbon. */
static lv_obj_t * group(lv_obj_t * parent, const char * caption)
{
    lv_obj_t * g = panel(parent, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_radius(g, U(4), 0);
    lv_obj_set_style_pad_ver(g, U(1), 0);
    lv_obj_set_style_pad_hor(g, U(4), 0);
    lv_obj_set_style_pad_row(g, 0, 0);
    if(caption) {
        lv_obj_t * c = text(g, caption, T_CAPTION, true);
        lv_obj_set_style_pad_top(c, U(2), 0);
    }
    return g;
}

/* One row of a group: a name (and hint) on the left, a control on the right. */
static lv_obj_t * group_row(lv_obj_t * g, const char * name, const char * hint)
{
    lv_obj_t * r = row(g, LV_PCT(100), U(3), LV_FLEX_ALIGN_SPACE_BETWEEN);
    lv_obj_set_style_min_height(r, U(12), 0);
    lv_obj_t * t = column(r, LV_SIZE_CONTENT, 0);
    text(t, name, T_BODY, false);
    if(hint) text(t, hint, T_CAPTION, true);
    return r;
}

static void build_settings(lv_obj_t * scr)
{
    int32_t list_w = U(66);
    lv_obj_t * list = lv_glass_list_create(scr);
    lv_obj_set_size(list, list_w, CONTENT_H);
    lv_obj_set_style_radius(list, RADIUS, 0);
    lv_obj_set_style_pad_all(list, U(2), 0);
    lv_obj_t * h = lv_glass_list_add_header(list, "Settings");
    lv_obj_set_style_text_font(h, T_TITLE2, 0);
    lv_obj_set_style_text_color(h, lv_glass_get_text_color(), 0);
    lv_obj_set_style_pad_bottom(h, U(3), 0);
    static const setting_t items[] = {
        {GV_IC_WIFI, "Wi-Fi", C_BLUE},        {GV_IC_BLUETOOTH, "Bluetooth", C_BLUE},
        {GV_IC_SUN_MEDIUM, "Display", C_BLUE}, {GV_IC_VOLUME_2, "Sounds", C_PINK},
        {GV_IC_BELL, "Notifications", C_RED}, {GV_IC_BATTERY_FULL, "Battery", C_GREEN},
        {GV_IC_SETTINGS, "General", C_GRAY},
    };
    for(int i = 0; i < 7; i++) {
        lv_obj_t * r = lv_glass_list_add_row(list, items[i].icon, items[i].name);
        lv_obj_set_style_border_width(r, 0, 0);
        lv_obj_set_style_pad_ver(r, U(2), 0);
        lv_obj_t * ic = lv_obj_get_child(r, 0);
        /* the row's icon label becomes a rounded coloured square */
        lv_obj_set_size(ic, U(7), U(7));
        lv_obj_set_style_radius(ic, U(2), 0);
        lv_obj_set_style_bg_opa(ic, LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(ic, sys(items[i].colour), 0);
        lv_obj_set_style_text_color(ic, lv_color_white(), 0);
        lv_obj_set_style_text_font(ic, T_BODY, 0);
        lv_obj_set_style_text_align(ic, LV_TEXT_ALIGN_CENTER, 0);
        lv_obj_set_style_pad_top(ic, (U(7) - lv_font_get_line_height(T_BODY)) / 2, 0);
        lv_obj_set_style_text_font(lv_obj_get_child(r, 1), T_BODY, 0);
        if(i == 2) { /* the selected row: the theme's accent fill, with the icon turned white */
            lv_glass_set_variant(r, LV_GLASS_VARIANT_ACCENT);
            lv_obj_set_style_bg_color(ic, lv_color_white(), 0);
            lv_obj_set_style_text_color(ic, lv_glass_get_accent(), 0);
        }
    }

    lv_obj_t * page = box(scr, CONTENT_W - list_w - GAP, CONTENT_H, LV_FLEX_FLOW_COLUMN, U(2));
    lv_obj_set_style_pad_hor(page, U(1), 0);
    text(page, "Display", T_TITLE1, false);

    lv_obj_t * g = group(page, "BRIGHTNESS");
    lv_obj_t * s = icon_slider(g, GV_IC_SUN_DIM, GV_IC_SUN, 72);
    lv_obj_set_height(lv_obj_get_parent(s), U(12));
    hairline(g);
    lv_obj_t * r = group_row(g, "Auto Colour", "Match the room's light");
    lv_obj_t * sw = lv_switch_create(r);
    lv_obj_add_state(sw, LV_STATE_CHECKED);

    g = group(page, "APPEARANCE");
    r = group_row(g, "Dark Mode", NULL);
    sw = lv_switch_create(r);
    if(s_dark) lv_obj_add_state(sw, LV_STATE_CHECKED);
    hairline(g);
    r = group_row(g, "Accent Colour", NULL);
    lv_obj_t * dots = row(r, LV_SIZE_CONTENT, U(2), LV_FLEX_ALIGN_END);
    static const int acc[] = {C_BLUE, C_GREEN, C_RED, C_PURPLE, C_ORANGE, C_PINK}; /* lv_glass_accent_t order */
    int chosen = (int)lv_glass_get_config()->accent;
    for(int i = 0; i < 6; i++) {
        lv_obj_t * d = shape(dots, U(6), LV_RADIUS_CIRCLE, sys(acc[i]), LV_OPA_COVER);
        if(i == chosen) {
            lv_obj_set_style_outline_width(d, 2, 0);
            lv_obj_set_style_outline_pad(d, 2, 0);
            lv_obj_set_style_outline_color(d, lv_glass_get_text_color(), 0);
        }
    }

    g = group(page, NULL);
    lv_obj_set_style_margin_top(g, U(2), 0);
    r = group_row(g, "Auto-Lock", NULL);
    lv_obj_t * dd = lv_dropdown_create(r);
    lv_dropdown_set_options_static(dd, "30 seconds\n1 minute\n5 minutes\nNever");
    lv_dropdown_set_symbol(dd, GV_IC_CHEVRON_DOWN);
    lv_obj_set_style_text_font(dd, T_FOOT, 0);
    lv_obj_set_width(dd, U(36));
}

/* ---------------------------------------------------------------- music */

static void build_music(lv_obj_t * scr)
{
    lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_t * head = row(scr, CONTENT_W, GAP, LV_FLEX_ALIGN_SPACE_BETWEEN);
    text(head, "Library", T_TITLE1, false);
    static const char * tabs[] = {"Playlists", "Albums", "Artists", ""};
    segmented(head, tabs, 1, U(72));

    lv_obj_t * shelf = panel(scr, CONTENT_W, LV_SIZE_CONTENT);
    lv_obj_t * sh = row(shelf, LV_PCT(100), 0, LV_FLEX_ALIGN_SPACE_BETWEEN);
    text(sh, "Recently Played", T_TITLE3, false);
    tinted(sh, "See All", T_FOOT, lv_glass_get_accent());
    lv_obj_t * cards = row(shelf, LV_PCT(100), GAP, LV_FLEX_ALIGN_START);
    static const struct { const char * t; const char * a; uint32_t c1, c2; const char * cap; } al[] = {
        {"Glasshouse", "The Lumen Set", 0x6FA6F7, 0x1F4FAE, "GLASS"},
        {"Low Tide", "Harbour Lights", 0x5EC4D3, 0x1D6A86, "TIDE"},
        {"Night Drive", "Kilo Static", 0x47587A, 0x141A2A, "NIGHT"},
        {"Sunroom", "Mira Vale", 0xE6B877, 0xA8683A, "SUN"},
        {"Focus Flow", "Daily Mix", 0x5CC6A6, 0x1C7768, "FOCUS"},
    };
    int32_t card_w = (CONTENT_W - 2 * PANEL_PAD - 4 * GAP) / 5;
    for(int i = 0; i < 5; i++) {
        lv_obj_t * c = column(cards, card_w, U(1));
        lv_obj_t * a = cover(c, card_w, al[i].c1, al[i].c2, al[i].cap);
        lv_obj_set_style_margin_bottom(a, U(1), 0);
        text(c, al[i].t, T_HEAD, false);
        text(c, al[i].a, T_FOOT, true);
    }

    /* a floating mini player */
    lv_obj_t * mini = panel(scr, CONTENT_W, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(mini, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(mini, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_radius(mini, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_pad_all(mini, U(2), 0);
    lv_obj_set_style_pad_right(mini, U(3), 0);
    lv_obj_set_style_pad_column(mini, U(3), 0);
    lv_obj_t * mc = cover(mini, U(12), 0x6FA6F7, 0x1F4FAE, NULL);
    lv_obj_set_style_radius(mc, LV_RADIUS_CIRCLE, 0);
    lv_obj_t * nm = column(mini, U(40), 0);
    text(nm, "Midnight Static", T_HEAD, false);
    text(nm, "The Lumen Set", T_FOOT, true);
    lv_obj_t * bar = lv_bar_create(mini);
    lv_obj_set_flex_grow(bar, 1);
    lv_obj_set_height(bar, U(1));
    lv_bar_set_value(bar, 43, LV_ANIM_OFF);
    round_button(mini, GV_IC_SKIP_BACK, U(10), false);
    round_button(mini, GV_IC_PAUSE, U(11), true);
    round_button(mini, GV_IC_SKIP_FORWARD, U(10), false);
}

/* -------------------------------------------------------------- weather */

typedef enum { WX_SUN, WX_PART, WX_CLOUD, WX_RAIN } wx_t;

static void wx(lv_obj_t * parent, wx_t k, const lv_font_t * font)
{
    static const char * icons[] = {GV_IC_SUN, GV_IC_CLOUD_SUN, GV_IC_CLOUD, GV_IC_CLOUD_RAIN};
    lv_color_t c = k == WX_SUN ? sys(C_YELLOW) : (k == WX_RAIN ? sys(C_TEAL) : lv_glass_get_text_color());
    if(k == WX_SUN && !s_dark) c = lv_color_hex(0xF2A100); /* system yellow is too pale on light glass */
    tinted_icon(parent, icons[k], font, c);
}

static void build_weather(lv_obj_t * scr)
{
    int32_t now_w = U(70);
    lv_obj_t * now = panel(scr, now_w, CONTENT_H);
    lv_obj_set_style_pad_all(now, U(6), 0);
    lv_obj_set_flex_align(now, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_t * city = column(now, LV_PCT(100), 0);
    lv_obj_t * cr = row(city, LV_SIZE_CONTENT, U(1), LV_FLEX_ALIGN_START);
    text(cr, "Lisbon", T_TITLE2, false);
    glyph(cr, GV_IC_NAVIGATION, T_BODY, false);
    text(city, "Tuesday 13:20", T_FOOT, true);

    lv_obj_t * mid = column(now, LV_PCT(100), 0);
    lv_obj_t * temp = text(mid, "24\xC2\xB0", T_HUGE, false);
    lv_obj_set_style_text_letter_space(temp, -2, 0);
    text(mid, "Mostly Sunny", T_TITLE3, false);
    text(mid, "H:27\xC2\xB0  L:18\xC2\xB0", T_BODY, true);

    lv_obj_t * stats = row(now, LV_PCT(100), 0, LV_FLEX_ALIGN_SPACE_BETWEEN);
    static const char * st[][3] = {{GV_IC_WIND, "12 km/h", "Wind"}, {GV_IC_DROPLETS, "58%", "Humidity"},
                                   {GV_IC_SUN, "6", "UV Index"}};
    for(int i = 0; i < 3; i++) {
        lv_obj_t * c = column(stats, LV_SIZE_CONTENT, 0);
        glyph(c, st[i][0], T_BODY, true);
        text(c, st[i][1], T_HEAD, false);
        text(c, st[i][2], T_CAPTION, true);
    }

    int32_t right_w = CONTENT_W - now_w - GAP;
    lv_obj_t * right = box(scr, right_w, CONTENT_H, LV_FLEX_FLOW_COLUMN, GAP);

    lv_obj_t * hourly = panel(right, right_w, LV_SIZE_CONTENT);
    text(hourly, "Sunny all afternoon. Clouds roll in after 18:00.", T_FOOT, true);
    hairline(hourly);
    lv_obj_t * hrow = row(hourly, LV_PCT(100), 0, LV_FLEX_ALIGN_SPACE_BETWEEN);
    static const char * hours[] = {"Now", "14", "15", "16", "17", "18", "19"};
    static const char * htemp[] = {"24\xC2\xB0", "25\xC2\xB0", "26\xC2\xB0", "25\xC2\xB0", "23\xC2\xB0", "21\xC2\xB0", "19\xC2\xB0"};
    static const wx_t hwx[] = {WX_PART, WX_SUN, WX_SUN, WX_SUN, WX_PART, WX_CLOUD, WX_CLOUD};
    for(int i = 0; i < 7; i++) {
        lv_obj_t * c = column(hrow, LV_SIZE_CONTENT, U(2));
        lv_obj_set_flex_align(c, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        text(c, hours[i], T_FOOT, i != 0);
        wx(c, hwx[i], T_TITLE2);
        text(c, htemp[i], T_HEAD, false);
    }

    lv_obj_t * daily = panel(right, right_w, LV_SIZE_CONTENT);
    lv_obj_set_flex_grow(daily, 1);
    lv_obj_set_flex_align(daily, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_t * dh = row(daily, LV_PCT(100), U(2), LV_FLEX_ALIGN_START);
    glyph(dh, GV_IC_CALENDAR, T_FOOT, true);
    text(dh, "5-DAY FORECAST", T_CAPTION, true);
    static const struct { const char * day; wx_t wx; int lo, hi; } days[] = {
        {"Today", WX_PART, 18, 27}, {"Wed", WX_SUN, 17, 28}, {"Thu", WX_CLOUD, 16, 23}, {"Fri", WX_RAIN, 14, 19},
        {"Sat", WX_SUN, 18, 29}};
    for(int i = 0; i < 5; i++) {
        hairline(daily);
        lv_obj_t * r = row(daily, LV_PCT(100), U(3), LV_FLEX_ALIGN_START);
        lv_obj_t * d = text(r, days[i].day, T_HEAD, false);
        lv_obj_set_width(d, U(14));
        lv_obj_t * ic = column(r, U(8), 0);
        wx(ic, days[i].wx, T_TITLE3);
        char lo[8], hi[8];
        snprintf(lo, sizeof lo, "%d\xC2\xB0", days[i].lo);
        snprintf(hi, sizeof hi, "%d\xC2\xB0", days[i].hi);
        lv_obj_t * l = text(r, lo, T_HEAD, true);
        lv_obj_set_width(l, U(9));
        lv_obj_set_style_text_align(l, LV_TEXT_ALIGN_RIGHT, 0);
        lv_obj_t * bar = lv_bar_create(r);
        lv_obj_set_flex_grow(bar, 1);
        lv_obj_set_height(bar, U(1) + 1);
        lv_bar_set_range(bar, 12, 30);
        lv_bar_set_mode(bar, LV_BAR_MODE_RANGE);
        lv_bar_set_value(bar, days[i].hi, LV_ANIM_OFF); /* first: the start can't pass the value */
        lv_bar_set_start_value(bar, days[i].lo, LV_ANIM_OFF);
        lv_obj_set_style_bg_color(bar, sys(C_TEAL), LV_PART_INDICATOR);
        lv_obj_set_style_bg_grad_color(bar, sys(C_ORANGE), LV_PART_INDICATOR);
        lv_obj_set_style_bg_grad_dir(bar, LV_GRAD_DIR_HOR, LV_PART_INDICATOR);
        lv_obj_set_style_bg_main_stop(bar, 0, LV_PART_INDICATOR);
        lv_obj_set_style_bg_grad_stop(bar, 255, LV_PART_INDICATOR);
        lv_obj_t * hl = text(r, hi, T_HEAD, false);
        lv_obj_set_width(hl, U(9));
        lv_obj_set_style_text_align(hl, LV_TEXT_ALIGN_RIGHT, 0);
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
    lv_obj_set_style_arc_opa(a, 56, LV_PART_MAIN);
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
    int32_t rw = d * 3 / 100 + 1;
    int32_t step = rw + LV_MAX(2, d / 120);
    int32_t d0 = d - 2 * (d * 7 / 100); /* clear of a round bezel */

    lv_obj_t * face = lv_obj_create(scr);
    lv_obj_set_size(face, d0 - 6 * step - rw, d0 - 6 * step - rw);
    lv_obj_set_style_radius(face, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_pad_all(face, 0, 0);
    NO_SCROLL(face);
    lv_obj_center(face);

    ring(scr, d0, rw, sys(C_RED), 78);
    ring(scr, d0 - 2 * step, rw, sys(C_GREEN), 56);
    ring(scr, d0 - 4 * step, rw, sys(C_TEAL), 90);

    lv_obj_t * c = column(face, LV_SIZE_CONTENT, 0);
    lv_obj_set_flex_align(c, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_center(c);
    tinted(c, "TUE 11", tiny ? T_CAPTION : T_HEAD, sys(C_RED));
    lv_obj_t * t = text(c, "09:41", tiny ? T_DISPLAY : T_HUGE, false);
    lv_obj_set_style_text_letter_space(t, tiny ? -1 : -2, 0);
    lv_obj_t * steps = row(c, LV_SIZE_CONTENT, U(1), LV_FLEX_ALIGN_CENTER);
    tinted_icon(steps, GV_IC_FOOTPRINTS, tiny ? T_CAPTION : T_BODY, sys(C_GREEN));
    text(steps, "8,412", tiny ? T_CAPTION : T_HEAD, false);
    if(!tiny) {
        lv_obj_t * comp = row(c, LV_SIZE_CONTENT, U(2), LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_top(comp, U(3), 0);
        lv_obj_t * hr = pill(comp, NULL, GV_IC_HEART_PULSE " 72", false);
        lv_obj_set_height(hr, U(8));
        lv_obj_set_style_pad_hor(hr, U(3), 0);
        lv_obj_t * bt = pill(comp, NULL, GV_IC_BATTERY_FULL " 86%", false);
        lv_obj_set_height(bt, U(8));
        lv_obj_set_style_pad_hor(bt, U(3), 0);
    }
}

/* --------------------------------------------------------------- charts */

static void build_charts(lv_obj_t * scr)
{
    lv_obj_t * head = row(scr, CONTENT_W, GAP, LV_FLEX_ALIGN_SPACE_BETWEEN);
    screen_title(head, "Energy", "This week, in kWh");
    static const char * map[] = {"Day", "Week", "Month", ""};
    segmented(head, map, 1, U(60));

    lv_obj_t * cards = box(scr, CONTENT_W, LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW, GAP);
    static const struct { const char * icon; const char * name; const char * val; const char * unit;
                          const char * delta; int c; bool good; } sc[] = {
        {GV_IC_SUN, "Solar", "18.4", "kWh", GV_IC_ARROW_UP_RIGHT " 12% on last week", C_ORANGE, true},
        {GV_IC_HOUSE, "Home Use", "24.5", "kWh", GV_IC_ARROW_DOWN_RIGHT " 8% on last week", C_BLUE, true},
        {GV_IC_BATTERY_CHARGING, "Battery", "86", "%", "Full by 15:10", C_GREEN, false},
    };
    int32_t cw = (CONTENT_W - 2 * GAP) / 3;
    for(int i = 0; i < 3; i++) {
        lv_obj_t * c = panel(cards, cw, LV_SIZE_CONTENT);
        lv_obj_set_flex_flow(c, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(c, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
        lv_obj_set_style_pad_all(c, U(4), 0);
        icon_on(c, sc[i].icon, T_TITLE3, U(10), LV_RADIUS_CIRCLE, sys(sc[i].c), LV_OPA_COVER, lv_color_white());
        lv_obj_t * t = column(c, LV_SIZE_CONTENT, 0);
        text(t, sc[i].name, T_FOOT, true);
        lv_obj_t * v = row(t, LV_SIZE_CONTENT, U(1), LV_FLEX_ALIGN_START);
        lv_obj_set_flex_align(v, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_END);
        text(v, sc[i].val, T_TITLE2, false);
        lv_obj_t * u = text(v, sc[i].unit, T_FOOT, true);
        lv_obj_set_style_pad_bottom(u, 3, 0);
        if(sc[i].good) tinted(t, sc[i].delta, T_CAPTION, sys(C_GREEN));
        else text(t, sc[i].delta, T_CAPTION, true);
    }

    int32_t body_h = CONTENT_H - U(14) - U(25) - 2 * GAP;
    lv_obj_t * body = box(scr, CONTENT_W, body_h, LV_FLEX_FLOW_ROW, GAP);
    lv_obj_set_flex_grow(body, 1);
    int32_t chart_w = (CONTENT_W - GAP) * 64 / 100;

    lv_obj_t * cp = panel(body, chart_w, LV_PCT(100));
    lv_obj_set_style_pad_row(cp, U(2), 0);
    lv_obj_t * legend = row(cp, LV_PCT(100), U(4), LV_FLEX_ALIGN_START);
    text(legend, "Daily kWh", T_HEAD, false);
    for(int i = 0; i < 2; i++) {
        lv_obj_t * k = row(legend, LV_SIZE_CONTENT, U(1), LV_FLEX_ALIGN_START);
        shape(k, U(2) + 2, 2, series(i), LV_OPA_COVER);
        text(k, i == 0 ? "Solar" : "Home use", T_FOOT, true);
    }

    /* the plot: a kWh scale on the left, the bars on the right, the days under the bars */
    int32_t scale_w = U(5);
    lv_obj_t * plot = box(cp, LV_PCT(100), LV_SIZE_CONTENT, LV_FLEX_FLOW_ROW, U(2));
    lv_obj_set_flex_grow(plot, 1);
    lv_obj_t * scale = column(plot, scale_w, 0);
    lv_obj_set_height(scale, LV_PCT(100));
    lv_obj_set_flex_align(scale, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_END);
    int32_t lh = lv_font_get_line_height(T_CAPTION);
    lv_obj_set_style_translate_y(scale, -lh / 2, 0); /* centre each number on its grid line */
    lv_obj_set_style_pad_bottom(scale, -lh, 0);
    static const char * ticks[] = {"40", "30", "20", "10", "0"};
    for(int i = 0; i < 5; i++) text(scale, ticks[i], T_CAPTION, true);

    lv_obj_t * chart = lv_chart_create(plot);
    lv_glass_remove(chart); /* no glass frame inside the panel: just the grid and the bars */
    lv_obj_set_height(chart, LV_PCT(100));
    lv_obj_set_flex_grow(chart, 1);
    lv_chart_set_type(chart, LV_CHART_TYPE_BAR);
    lv_chart_set_point_count(chart, 7);
    lv_chart_set_div_line_count(chart, 5, 0);
    lv_chart_set_range(chart, LV_CHART_AXIS_PRIMARY_Y, 0, 40);
    lv_obj_set_style_radius(chart, 0, 0);
    lv_obj_set_style_pad_all(chart, 0, 0);
    lv_obj_set_style_line_color(chart, lv_glass_get_muted_color(), 0);
    lv_obj_set_style_line_opa(chart, LV_OPA_20, 0);
    lv_obj_set_style_line_width(chart, 1, 0);
    lv_obj_set_style_pad_column(chart, 2, LV_PART_ITEMS);   /* 2 px between a day's two bars */
    lv_obj_set_style_pad_column(chart, U(4), LV_PART_MAIN); /* space between days */
    lv_obj_set_style_radius(chart, U(1), LV_PART_ITEMS);
    lv_chart_series_t * solar = lv_chart_add_series(chart, series(0), LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t * use = lv_chart_add_series(chart, series(1), LV_CHART_AXIS_PRIMARY_Y);
    static const int32_t vs[] = {14, 22, 19, 31, 27, 36, 33};
    static const int32_t vu[] = {26, 24, 29, 23, 30, 25, 21};
    for(int i = 0; i < 7; i++) {
        lv_chart_set_next_value(chart, solar, vs[i]);
        lv_chart_set_next_value(chart, use, vu[i]);
    }
    /* same gap as the chart's days and equal widths, so each name sits under its pair of bars */
    lv_obj_t * days = row(cp, LV_PCT(100), U(4), LV_FLEX_ALIGN_START);
    lv_obj_set_style_pad_left(days, scale_w + U(2), 0);
    static const char * dn[] = {"Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"};
    for(int i = 0; i < 7; i++) {
        lv_obj_t * l = text(days, dn[i], T_CAPTION, true);
        lv_obj_set_flex_grow(l, 1);
        lv_obj_set_style_text_align(l, LV_TEXT_ALIGN_CENTER, 0);
    }

    lv_obj_t * gp = panel(body, CONTENT_W - chart_w - GAP, LV_PCT(100));
    lv_obj_set_flex_align(gp, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_t * gh = row(gp, LV_PCT(100), 0, LV_FLEX_ALIGN_SPACE_BETWEEN);
    text(gh, "Self-Powered", T_HEAD, false);
    glyph(gh, GV_IC_LEAF, T_BODY, true);
    lv_obj_t * arc = lv_arc_create(gp);
    lv_obj_set_width(arc, LV_PCT(100));
    lv_obj_set_flex_grow(arc, 1); /* takes the height left between the title and the footer */
    lv_arc_set_range(arc, 0, 100);
    lv_arc_set_value(arc, 72);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);
    NO_CLICK(arc);
    lv_obj_set_style_arc_width(arc, U(3), LV_PART_MAIN);
    lv_obj_set_style_arc_width(arc, U(3), LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(arc, sys(C_GREEN), LV_PART_INDICATOR);
    lv_obj_t * v = column(arc, LV_SIZE_CONTENT, 0);
    lv_obj_set_flex_align(v, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_center(v);
    text(v, "72%", T_TITLE1, false);
    text(v, "of this week", T_CAPTION, true);
    text(gp, "Grid import 6.1 kWh", T_FOOT, true);

    /* then make it round: the smaller of the space's width and height, both ways */
    lv_obj_update_layout(scr);
    int32_t gd = LV_MIN(lv_obj_get_width(arc), lv_obj_get_height(arc));
    lv_obj_set_flex_grow(arc, 0);
    lv_obj_set_size(arc, gd, gd);
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
    s_dark = lv_glass_get_config()->scheme == LV_GLASS_SCHEME_DARK;

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

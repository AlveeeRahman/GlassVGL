/**
 * @file glassvgl_showcase_look.c
 * The look the README screenshots use: Inter type with Lucide icons, soft panel shadows and a
 * generated wallpaper of glossy ribbons over a pastel (light) or night (dark) mesh. See
 * glassvgl_showcase_config() in glassvgl_showcase.h.
 *
 * The wallpaper is drawn once, into one screen-sized buffer in the display's own colour format,
 * which the theme then uses in place without a second copy. The ribbons have crisp edges on
 * purpose: behind a glass panel the theme blurs them, which is what makes the glass read as glass.
 */
#include "glassvgl_showcase.h"
#include "fonts/gv_fonts.h"
#include <math.h>

typedef struct {
    float cx, cy, r;   /* centre and radius as fractions of the height */
    uint32_t c;
    float a;           /* strength 0..1 */
} blob_t;

typedef struct {
    float cy, amp, thick, waves, phase; /* centre line, swing, thickness, waves across, phase */
    uint32_t c1, c2;                    /* left and right colours */
} ribbon_t;

typedef struct {
    uint32_t top, bottom;
    blob_t blobs[4];
    ribbon_t ribbons[3];
    float shadow;
} look_t;

static const look_t LIGHT = {
    0xF3F5FC, 0xFBF3F1,
    {{0.05f, 0.05f, 0.60f, 0xB9D3FF, 0.90f}, {0.95f, 0.10f, 0.50f, 0xFFC9DA, 0.85f},
     {0.70f, 1.00f, 0.60f, 0xFFE0B8, 0.85f}, {0.15f, 1.00f, 0.50f, 0xC4F1E4, 0.80f}},
    {{0.38f, 0.20f, 0.11f, 0.8f, 0.3f, 0x9B8CFF, 0xFF9EC4},
     {0.62f, 0.16f, 0.10f, 0.95f, 2.2f, 0x7FD8FF, 0x8AA8FF},
     {0.86f, 0.12f, 0.12f, 0.65f, 4.0f, 0xFFB38A, 0xFF8FB8}},
    0.10f,
};

static const look_t DARK = {
    0x0A0E24, 0x04050C,
    {{0.10f, 0.10f, 0.60f, 0x2B2A8C, 0.80f}, {0.90f, 0.05f, 0.50f, 0x6A1E63, 0.70f},
     {0.80f, 1.00f, 0.60f, 0x0B4A63, 0.80f}, {0.20f, 1.00f, 0.40f, 0x1B2B6B, 0.50f}},
    {{0.38f, 0.20f, 0.11f, 0.8f, 0.3f, 0x7A5CFF, 0xFF5FA8},
     {0.62f, 0.16f, 0.10f, 0.95f, 2.2f, 0x2DD4BF, 0x3B82F6},
     {0.86f, 0.12f, 0.12f, 0.65f, 4.0f, 0xF472B6, 0x8B5CF6}},
    0.28f,
};

static float ch(uint32_t c, int shift)
{
    return (float)((c >> shift) & 0xFF) / 255.0f;
}

static float lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

static float clamp01(float v)
{
    return v < 0.0f ? 0.0f : (v > 1.0f ? 1.0f : v);
}

/* 4x4 ordered dither in -0.5..0.5 */
static float bayer(int32_t x, int32_t y)
{
    static const uint8_t m[16] = {0, 8, 2, 10, 12, 4, 14, 6, 3, 11, 1, 9, 15, 7, 13, 5};
    return ((float)m[(y & 3) * 4 + (x & 3)] + 0.5f) / 16.0f - 0.5f;
}

static void put(uint8_t * p, lv_color_format_t cf, const float rgb[3], int32_t x, int32_t y)
{
    float d = bayer(x, y);
    if(cf == LV_COLOR_FORMAT_RGB565) {
        int32_t r = (int32_t)(clamp01(rgb[0]) * 31.0f + 0.5f + d);
        int32_t g = (int32_t)(clamp01(rgb[1]) * 63.0f + 0.5f + d);
        int32_t b = (int32_t)(clamp01(rgb[2]) * 31.0f + 0.5f + d);
        r = LV_CLAMP(0, r, 31), g = LV_CLAMP(0, g, 63), b = LV_CLAMP(0, b, 31);
        uint16_t v = (uint16_t)((r << 11) | (g << 5) | b);
        p[0] = (uint8_t)(v & 0xFF);
        p[1] = (uint8_t)(v >> 8);
        return;
    }
    uint8_t c[3];
    for(int i = 0; i < 3; i++) {
        int32_t v = (int32_t)(clamp01(rgb[i]) * 255.0f + 0.5f + d);
        c[i] = (uint8_t)LV_CLAMP(0, v, 255);
    }
    p[0] = c[2], p[1] = c[1], p[2] = c[0]; /* LVGL stores B, G, R */
    if(cf != LV_COLOR_FORMAT_RGB888) p[3] = 0xFF;
}

/* Base gradient and blobs on a coarse grid; they are smooth, so this is sampled bilinearly. */
#define GRID 8

static void mesh_at(const look_t * L, float u, float v, float asp, float out[3])
{
    for(int i = 0; i < 3; i++) out[i] = lerp(ch(L->top, 16 - 8 * i), ch(L->bottom, 16 - 8 * i), v);
    for(int b = 0; b < 4; b++) {
        const blob_t * o = &L->blobs[b];
        float dx = (u - o->cx) * asp, dy = v - o->cy;
        float w = o->a * expf(-(dx * dx + dy * dy) / (o->r * o->r));
        for(int i = 0; i < 3; i++) out[i] = lerp(out[i], ch(o->c, 16 - 8 * i), w);
    }
}

static bool draw_wallpaper(lv_image_dsc_t * dsc, int32_t w, int32_t h, lv_color_format_t cf, bool dark)
{
    const look_t * L = dark ? &DARK : &LIGHT;
    uint32_t bpp = lv_color_format_get_size(cf);
    uint32_t stride = (uint32_t)w * bpp;
    int32_t gw = w / GRID + 2, gh = h / GRID + 2;
    uint8_t * px = lv_malloc((size_t)stride * h);
    float * grid = lv_malloc(sizeof(float) * 3 * gw * gh);
    float * edge = lv_malloc(sizeof(float) * 2 * 3 * w);
    if(!px || !grid || !edge) {
        lv_free(px), lv_free(grid), lv_free(edge);
        return false;
    }
    float asp = (float)w / (float)h;

    for(int32_t gy = 0; gy < gh; gy++)
        for(int32_t gx = 0; gx < gw; gx++)
            mesh_at(L, (float)(gx * GRID) / w, (float)(gy * GRID) / h, asp, grid + 3 * (gy * gw + gx));

    /* each ribbon's top and bottom edge per column, in pixels */
    for(int r = 0; r < 3; r++) {
        const ribbon_t * o = &L->ribbons[r];
        for(int32_t x = 0; x < w; x++) {
            float u = (float)x / w;
            float top = o->cy + o->amp * sinf(6.2831853f * u * o->waves + o->phase) - o->thick / 2;
            float th = o->thick * (0.7f + 0.3f * (1.0f + sinf(6.2831853f * u * 0.8f + o->phase * 1.7f)));
            edge[(r * w + x) * 2] = top * h;
            edge[(r * w + x) * 2 + 1] = (top + th) * h;
        }
    }

    for(int32_t y = 0; y < h; y++) {
        int32_t gy = y / GRID;
        float fy = (float)(y % GRID) / GRID;
        for(int32_t x = 0; x < w; x++) {
            int32_t gx = x / GRID;
            float fx = (float)(x % GRID) / GRID;
            const float * g00 = grid + 3 * (gy * gw + gx);
            const float * g10 = g00 + 3;
            const float * g01 = g00 + 3 * gw;
            const float * g11 = g01 + 3;
            float c[3];
            for(int i = 0; i < 3; i++) c[i] = lerp(lerp(g00[i], g10[i], fx), lerp(g01[i], g11[i], fx), fy);

            float yy = (float)y + 0.5f;
            for(int r = 0; r < 3; r++) {
                const ribbon_t * o = &L->ribbons[r];
                float top = edge[(r * w + x) * 2], bot = edge[(r * w + x) * 2 + 1];
                if(yy > bot) { /* soft shadow under the ribbon */
                    float s = 1.0f - (yy - bot) / (0.06f * h);
                    if(s > 0.0f) for(int i = 0; i < 3; i++) c[i] *= 1.0f - L->shadow * s;
                    continue;
                }
                float in = clamp01(yy - top) * clamp01(bot - yy); /* one pixel of anti-aliasing */
                if(in <= 0.0f) continue;
                float f = clamp01((yy - top) / (bot - top));
                float t = clamp01((float)x / w * 0.9f + f * 0.3f);
                float hl = 1.0f - f / 0.30f; /* lit top edge */
                hl = hl > 0.0f ? 0.45f * hl * hl : 0.0f;
                float shade = 1.0f - 0.25f * f * f; /* deeper toward the lower edge */
                for(int i = 0; i < 3; i++) {
                    float rc = lerp(ch(o->c1, 16 - 8 * i), ch(o->c2, 16 - 8 * i), t);
                    rc = lerp(rc, 1.0f, hl) * shade;
                    c[i] = lerp(c[i], rc, in);
                }
            }
            put(px + (size_t)y * stride + (size_t)x * bpp, cf, c, x, y);
        }
    }
    lv_free(grid);
    lv_free(edge);

    lv_memzero(dsc, sizeof(*dsc));
    dsc->header.magic = LV_IMAGE_HEADER_MAGIC;
    dsc->header.cf = cf;
    dsc->header.w = (uint32_t)w;
    dsc->header.h = (uint32_t)h;
    dsc->header.stride = stride;
    dsc->data_size = stride * (uint32_t)h;
    dsc->data = px;
    return true;
}

static lv_image_dsc_t s_wall;

bool glassvgl_showcase_config(lv_glass_config_t * cfg, lv_display_t * disp)
{
    cfg->font_small = &gv_font_12;
    cfg->font_normal = &gv_font_16;
    cfg->font_large = &gv_font_20;
    cfg->shadows = true;

    if(s_wall.data) {
        lv_free((void *)s_wall.data);
        lv_memzero(&s_wall, sizeof(s_wall));
    }
    lv_color_format_t cf = lv_display_get_color_format(disp);
    if(cf != LV_COLOR_FORMAT_RGB565 && cf != LV_COLOR_FORMAT_RGB888 && cf != LV_COLOR_FORMAT_XRGB8888 &&
       cf != LV_COLOR_FORMAT_ARGB8888)
        return false;
    if(!draw_wallpaper(&s_wall, lv_display_get_horizontal_resolution(disp), lv_display_get_vertical_resolution(disp),
                       cf, cfg->scheme == LV_GLASS_SCHEME_DARK))
        return false;
    cfg->wallpaper = LV_GLASS_WALLPAPER_IMAGE;
    cfg->wallpaper_image = &s_wall;
    return true;
}

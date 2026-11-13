# Changelog

## Unreleased

- GlassVGL Pro is now called GlassVGL Edge GUI. Only the name changed; the sample folder layout still
  expects the pack as `GlassVGL-Pro`.
- New prices: GlassVGL Icons $8.99 on Gumroad and $7.99 on Ko-fi, GlassVGL Desktop $19.99 on
  Gumroad and $18.99 on Ko-fi. The bundle price will be announced with GlassVGL Desktop.
- What's for sale now says every GlassVGL product was checked and works in light and dark mode.
- The README now lists the rest of what GlassVGL Pro does: the accessibility settings, the edge light
  colour, the low-power look, the LiteEDA starter design and the design tokens, with the glass memory
  per display size, the code size and the microcontroller builds checked under QEMU.
- GlassVGL Desktop: GlassVGL Flow's five depths, with two screenshot sheets, and that every pack is a
  one-time payment for a lifetime licence.
- The demo's inactive page dots now keep the same soft fill at every GlassVGL Flow depth, as in
  GlassVGL Pro, so they stay visible at Tinted. The screenshots that show page dots are redrawn.
- GlassVGL Flow in the README: the glass depth setting of GlassVGL Pro, from clear (48) to solid
  (255), with a sheet of the controls screen at the four presets in light and dark.
- "Why it stays snappy" now says how GlassVGL Pro makes taps land sooner: the demo builds the next
  page while the finger is down (20% less work after the lift at 800x480 on LVGL 9.6, 17% on 9.3),
  and a 16 ms refresh period (wait after the lift 16.9 ms to 9.0 ms on average).
- The desktop, PlatformIO and PlatformIO native samples set `LV_DEF_REFR_PERIOD` to 16 in their
  `lv_conf.h`, as the Linux sample already did.
- Every screenshot redrawn from GlassVGL Pro as it is now, light and dark, with the watermark.
  The bevel close-up is replaced by an edge light close-up, because GlassVGL Pro now draws a thin
  highlight along the top of each panel and a faint shade along the bottom instead of a bevel.

- Seven app sample screens in `examples/showcase`: media player, smart home, settings, music
  library, weather, round watch face and energy dashboard. The desktop sample can render any of them.
- New screenshots in `assets/screens`: every demo and app screen in light and dark at 800x480, the
  watch face at 466x466 and 240x240, six accents, six wallpapers and the three render modes. Each
  file is an exact 2x copy of the panel's pixels and the README shows it at the panel's real size.
- The wordmark in both logos is now frosted glass in mint, blue and violet, readable on light and
  dark pages; the README uses one logo for both themes.
- New social preview with the smart home and watch samples.
- Redrawn screenshots: the app sample screens now use Inter type with Lucide icons, soft panel
  shadows and a ribbon wallpaper drawn in code (`glassvgl_showcase_config()`), and the accent sheet
  shows the settings screen at 800x480, at 1x. All 35 files keep their names.
- Showcase fonts in `examples/showcase/fonts`, built from Inter (SIL OFL 1.1) and Lucide (ISC),
  with their licence files and a script to rebuild them.
- The settings sample's accent dots now mark the accent the theme was started with.
- A "What's for sale" section in the README with the planned prices of GlassVGL Pro and GlassVGL
  Icons, and a 25-icon GlassVGL Icons sample in `icons/` under CC BY 4.0.
- GlassVGL Desktop and a GlassVGL Desktop + Icons bundle in "What's for sale", both coming soon,
  with the Canonical trademark note.
- Every screenshot redrawn in the GlassVGL brand colours: azure accents, slate and teal ribbons
  over a porcelain or graphite base, with calmer album covers. The pack's own demo screens now use
  the same look. Every picture of a paid pack carries a "GlassVGL preview" watermark.
- Design renders of GlassVGL Desktop and a bundle picture in "What's for sale".
- The README's speed section now covers the change-only live setters and instant page switches.

## 1.0.0 (2026-11-12)

First public release of the samples.

- Arduino sketches `GlassVGLDemo` and `GlassVGLPager`.
- PlatformIO samples for boards and for the native platform.
- ESP-IDF, Zephyr and desktop (CMake and make) samples.
- Screenshots of every demo screen, accent, wallpaper and render mode, and the logo.
- A 1280x640 social preview image in `assets/social-preview.png`.

# Changelog

## Unreleased

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

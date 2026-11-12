<p align="center">
  <img src="assets/logo/glassvgl-logo.svg" alt="GlassVGL logo: a frosted-glass tile with a square-wave mark and a frosted mint-to-violet wordmark" width="360">
</p>

<h1 align="center">GlassVGL: a frosted-glass theme for LVGL 9 on microcontrollers</h1>

<p align="center">
  <a href="LICENSE"><img src="https://img.shields.io/github/license/AlveeeRahman/GlassVGL?label=licence" alt="Samples licence: MIT"></a>
  <a href="CHANGELOG.md"><img src="https://img.shields.io/badge/version-1.0.0-2ea44f" alt="Version 1.0.0"></a>
  <a href="https://lvgl.io"><img src="https://img.shields.io/badge/LVGL-9.3%20to%209.6-343839" alt="Works with LVGL 9.3 to 9.6"></a>
  <a href="https://github.com/AlveeeRahman/GlassVGL/stargazers"><img src="https://img.shields.io/github/stars/AlveeeRahman/GlassVGL?style=flat" alt="GitHub stars"></a>
  <a href="https://github.com/AlveeeRahman/GlassVGL/commits/main"><img src="https://img.shields.io/github/last-commit/AlveeeRahman/GlassVGL" alt="Last commit"></a>
</p>
<p align="center">
  <a href="examples/GlassVGLDemo"><img src="https://img.shields.io/badge/Arduino-sketches-00878F?logo=arduino&logoColor=white" alt="Arduino sketches"></a>
  <a href="examples/platformio"><img src="https://img.shields.io/badge/PlatformIO-project-F5822A?logo=platformio&logoColor=white" alt="PlatformIO project"></a>
  <a href="examples/esp-idf"><img src="https://img.shields.io/badge/ESP--IDF-component-E7352C?logo=espressif&logoColor=white" alt="ESP-IDF component"></a>
  <a href="examples/zephyr"><img src="https://img.shields.io/badge/Zephyr-module-7929D2" alt="Zephyr module"></a>
  <a href="examples/desktop"><img src="https://img.shields.io/badge/CMake%20%7C%20make-desktop-064F8C?logo=cmake&logoColor=white" alt="CMake and make desktop sample"></a>
</p>

GlassVGL is a frosted-glass (liquid glass) UI theme for [LVGL](https://lvgl.io) 9, built to run on
microcontrollers such as the ESP32, ESP32-S3, Raspberry Pi Pico (RP2040) and STM32, as well as on a
PC. It gives LVGL widgets translucent glass panels, a soft 3D bevel, accent colours and built-in
wallpapers, with light and dark schemes.

This repository holds the free samples: an example project for every common LVGL toolchain, seven
app sample screens, the screenshots, the logo and a free sample of the icon pack. The theme itself
is the paid GlassVGL Pro pack, which is not on sale yet; see [What's for sale](#whats-for-sale).
The samples build once the pack sits next to this repository.

<img src="assets/screens/smart-home-dark.png" alt="GlassVGL smart home sample at 800x480 in the dark scheme: frosted glass device tiles, a thermostat dial and scene buttons over a ribbon wallpaper" width="800">

Seven app-style sample screens come with the samples: a media player, a smart home room, settings,
a music library, weather, a round watch face and an energy dashboard. They are in
[`examples/showcase`](examples/showcase) and use only the theme's public API and stock LVGL widgets.

## Why it stays snappy on an MCU

**The blur runs once.** Blur is the costly part of a glass look, so GlassVGL does it once at
start-up and keeps the result. After that a glass panel costs one image copy, one translucent fill
and two thin bevel lines each time it is drawn. On a PC benchmark (LVGL 9.6, RGB565, one core of a
2.1 GHz Xeon), a full-screen redraw of the controls screen at 800x480 took 1.2 ms with the glass on
and 0.7 ms in FLAT mode with the glass off.

**Unchanged values are not redrawn.** A live screen usually refreshes its labels from a timer, and
LVGL 9 redraws a label each time its text is set, even when the text is the same. GlassVGL Pro's
change-only setters compare first and leave unchanged widgets alone. In the pack's host test, a
screen of 12 glass tiles refreshed ten times a second with one value changing went from 7,570 to
994 redrawn pixels per refresh. The same technique, used in an earlier ESP32-S3 firmware of ours
without GlassVGL (172x320 SPI panel), cut LVGL's share of a CPU core on a static screen from 50.2%
to 9.5%.

**Page switches skip the fade.** `lv_glass_live_show_screen()` loads a page with no fade, which
would draw both pages for the whole fade, and runs its refresh timer on the next pass of LVGL's
timer handler, so the new page shows fresh values instead of waiting a whole timer period.

A microcontroller is much slower than a PC, so the demo's STATS screen measures redraw time on your
board. The soft panel shadows in the app sample screenshots are an option that is off by default:
shadows are drawn on every redraw, so leave them off on slow boards.

## Screenshots of the LVGL glass theme

Every image was rendered by the theme's own code on a PC and is shown at the panel's real size,
in both the light and the dark scheme. Each file holds an exact 2x copy of the panel's pixels, so it
stays sharp on high-density screens and is never resampled; the one change is a light "GlassVGL
preview" watermark drawn over it. The exception is the accent sheets, noted under them. They were
rendered in 24-bit colour; on an RGB565 panel, smooth gradients show light banding. Click a heading
to open it.

<details open>
<summary>App sample screens at 800x480, light and dark</summary>

<img src="assets/screens/player-light.png" alt="Media player sample, light scheme: gradient cover art, track title, progress slider, transport buttons and volume" width="800">

<img src="assets/screens/player-dark.png" alt="Media player sample, dark scheme over the ribbon wallpaper" width="800">

<img src="assets/screens/smart-home-light.png" alt="Smart home sample, light scheme: six device tiles with switches, a thermostat dial and scene buttons" width="800">

<img src="assets/screens/settings-light.png" alt="Settings sample, light scheme: a glass sidebar list and a display page with a brightness slider, switches, accent dots and a dropdown" width="800">

<img src="assets/screens/settings-dark.png" alt="Settings sample, dark scheme" width="800">

<img src="assets/screens/music-light.png" alt="Music library sample, light scheme: five recently played album covers and a mini player" width="800">

<img src="assets/screens/music-dark.png" alt="Music library sample, dark scheme" width="800">

<img src="assets/screens/weather-light.png" alt="Weather sample, light scheme: current temperature, wind, humidity and UV, an hourly strip and five-day range bars" width="800">

<img src="assets/screens/weather-dark.png" alt="Weather sample, dark scheme" width="800">

<img src="assets/screens/charts-light.png" alt="Energy dashboard sample, light scheme: stat cards, a bar chart of daily solar and home use, and a self-powered gauge" width="800">

<img src="assets/screens/charts-dark.png" alt="Energy dashboard sample, dark scheme" width="800">

</details>

<details>
<summary>Round watch face: 466x466 and 240x240</summary>

<img src="assets/screens/watch-466-dark.png" alt="Round watch face at 466x466, dark scheme: three activity rings around a glass dial with the time, date, steps, heart rate and battery" width="466">
<img src="assets/screens/watch-466-light.png" alt="Round watch face at 466x466, light scheme" width="466">

<img src="assets/screens/watch-240-dark.png" alt="Round watch face at 240x240, dark scheme" width="240">
<img src="assets/screens/watch-240-light.png" alt="Round watch face at 240x240, light scheme" width="240">

</details>

<details>
<summary>Six accent colours, light and dark</summary>

<img src="assets/screens/accents-light.png" alt="The settings sample at 800x480 in the six accents, light scheme: azure, mint, coral, violet, amber and rose" width="816">

<img src="assets/screens/accents-dark.png" alt="The settings sample at 800x480 in the six accents, dark scheme" width="816">

Left to right, top to bottom: azure, mint, coral, violet, amber, rose. Each sheet holds the screens
at 1x and is shown at half size; open an image to see it full size.

</details>

<details>
<summary>The pack's own demo screens at 800x480</summary>

These are the demo screens exactly as GlassVGL Pro draws them, with the Inter demo fonts that come
with the pack and its built-in wallpapers (ocean in light, graphite in dark). The app samples above
add the showcase look from this repository: the ribbon wallpaper image, Lucide icons and soft
shadows.

<img src="assets/screens/demo-home-light.png" alt="Demo home screen, light scheme: clock card, app icon grid and a dock" width="800">

<img src="assets/screens/demo-home-dark.png" alt="Demo home screen, dark scheme" width="800">

<img src="assets/screens/demo-controls-light.png" alt="Demo controls screen, light scheme: sliders, switches, an arc dial and buttons" width="800">

<img src="assets/screens/demo-controls-dark.png" alt="Demo controls screen, dark scheme" width="800">

<img src="assets/screens/demo-lists-light.png" alt="Demo lists screen, light scheme: tabs with lists of devices and scenes" width="800">

<img src="assets/screens/demo-lists-dark.png" alt="Demo lists screen, dark scheme" width="800">

<img src="assets/screens/demo-keyboard-light.png" alt="Demo keyboard screen, light scheme: a text field and the on-screen keyboard" width="800">

<img src="assets/screens/demo-keyboard-dark.png" alt="Demo keyboard screen, dark scheme" width="800">

<img src="assets/screens/demo-dashboard-light.png" alt="Demo dashboard screen, light scheme: line and bar charts with stat cards" width="800">

<img src="assets/screens/demo-dashboard-dark.png" alt="Demo dashboard screen, dark scheme" width="800">

<img src="assets/screens/demo-stats-light.png" alt="Demo stats screen, light scheme: render mode, bake time, glass memory and redraw time measured on the board" width="800">

<img src="assets/screens/demo-stats-dark.png" alt="Demo stats screen, dark scheme" width="800">

</details>

<details>
<summary>Small screens: 240x240 round and 320x240</summary>

<img src="assets/screens/small-240-round-light.png" alt="The six demo screens on a 240x240 round panel, light scheme" width="752">

<img src="assets/screens/small-240-round-dark.png" alt="The six demo screens on a 240x240 round panel, dark scheme" width="752">

<img src="assets/screens/small-320x240-light.png" alt="The six demo screens at 320x240, light scheme" width="656">

<img src="assets/screens/small-320x240-dark.png" alt="The six demo screens at 320x240, dark scheme" width="656">

</details>

<details>
<summary>The six built-in wallpapers, light and dark</summary>

<img src="assets/screens/wallpapers-light.png" alt="The demo home screen at 400x260 on the light version of each wallpaper: aurora, sunset, ocean, blush, meadow and graphite" width="816">

<img src="assets/screens/wallpapers-dark.png" alt="The demo home screen at 400x260 on the dark version of each wallpaper" width="816">

Left to right, top to bottom: aurora, sunset, ocean, blush, meadow, graphite. Each wallpaper has a
light and a dark version, and the theme draws the one that matches the scheme.

</details>

<details>
<summary>FULL, LITE and FLAT render modes, light and dark</summary>

<img src="assets/screens/mode-full-light.png" alt="Controls screen in FULL mode, light scheme: full-resolution baked glass" width="800">

<img src="assets/screens/mode-full-dark.png" alt="Controls screen in FULL mode, dark scheme" width="800">

<img src="assets/screens/mode-lite-light.png" alt="Controls screen in LITE mode, light scheme: quarter-resolution glass smoothed back up" width="800">

<img src="assets/screens/mode-lite-dark.png" alt="Controls screen in LITE mode, dark scheme" width="800">

<img src="assets/screens/mode-flat-light.png" alt="Controls screen in FLAT mode, light scheme: gradients and translucent fills, no image buffers" width="800">

<img src="assets/screens/mode-flat-dark.png" alt="Controls screen in FLAT mode, dark scheme" width="800">

</details>

<details>
<summary>The 3D bevel up close</summary>

<img src="assets/previews/bevel-close-up.png" alt="The thermostat card at twice its size in the light and dark schemes: a bright edge on the top and left of each glass panel and button, and a darker edge on the bottom and right" width="830">

</details>

<details>
<summary>Swipe pager mid-swipe: live, snapshot and direct</summary>

<img src="assets/previews/swipe-light.png" alt="The pager halfway through a swipe in live, snapshot and direct mode, light scheme" width="400">
<img src="assets/previews/swipe-dark.png" alt="The pager halfway through a swipe in live, snapshot and direct mode, dark scheme" width="400">

Top to bottom: live, snapshot, direct. In live mode the wallpaper stays still while the pages move.
In snapshot and direct mode each page carries its own copy of the wallpaper.

</details>

## Features

- **Schemes:** light and dark.
- **Accents:** azure, mint, coral, violet, amber and rose, plus any colour you pick.
- **Wallpapers:** aurora, sunset, ocean, blush, meadow and graphite, each in a light and a dark version, drawn in code so they cost no image flash, or your own image.
- **Render modes:** FULL for boards with PSRAM, LITE for boards without it, FLAT when RAM is very short. AUTO picks one from the memory it finds.
- **Widget variants:** panel, accent or clear glass, or a plain solid fill, set per widget.
- **Swipe pager:** slides pictures of the pages instead of redrawing every widget, when the RAM allows.
- **Change-only live setters:** labels and colours set from a refresh timer are redrawn only when they change.
- **Demo:** six screens (home, controls, lists, keyboard, dashboard, stats) that fit any size from 240x240 round to 1024x600.
- **App samples (free, MIT):** media player, smart home, settings, music library, weather, round watch face and energy dashboard, in `examples/showcase`.

It works with LVGL 9.3 to 9.6, in RGB565, RGB565_SWAPPED, RGB888 and XRGB8888.

## What's for sale

None of the paid packs is on sale yet. The store links below will go live when they are. Every
picture of a paid pack on this page carries a "GlassVGL preview" watermark.

| | What it is | Price | Licence | Store |
|---|---|---|---|---|
| **Free (this repository)** | Samples, app screens and assets | Free | MIT (samples), CC BY 4.0 (icon sample) | You are here |
| **GlassVGL Pro** | The LVGL glass theme itself | $12.99 on Gumroad, $11.99 on Ko-fi | Build into your own products, no royalties | Coming soon |
| **GlassVGL Icons** | A glass icon theme for Linux desktops | $12.99 on Gumroad, $11.99 on Ko-fi | Personal use on your own devices | Coming soon |
| **GlassVGL Desktop** | A glass GTK and GNOME Shell theme for Ubuntu 24.04. Design preview: still being built, not available yet | $18.99 on Gumroad, $17.99 on Ko-fi | Not announced yet | Coming soon |
| **GlassVGL Desktop + Icons bundle** | GlassVGL Desktop and GlassVGL Icons together | $27.99 on Gumroad, $24.99 on Ko-fi | As for each pack | Coming soon |

### Free, in this repository

- Sample projects for Arduino, PlatformIO, ESP-IDF, Zephyr, CMake and make.
- Seven app sample screens in `examples/showcase`, with their Inter and Lucide fonts.
- Every screenshot on this page, the logo and the social preview.
- A 25-icon sample of GlassVGL Icons in [`icons/`](icons), in light and dark, under CC BY 4.0.

The samples need GlassVGL Pro to build. No theme source is in this repository.

### GlassVGL Pro

The theme that drew every screenshot on this page.

- **One call styles every widget:** `lv_glass_theme_init()` themes all the LVGL widgets you create after it.
- **Glass that is cheap to draw:** the blur is baked once at start-up, with FULL, LITE and FLAT render modes picked by the memory your board has.
- **Change-only live setters:** labels and colours that have not changed are not redrawn, as in [Why it stays snappy on an MCU](#why-it-stays-snappy-on-an-mcu).
- **Swipe pager:** slides pictures of the pages instead of redrawing every widget, when the RAM allows.
- **Light and dark schemes,** six accents plus your own colour, and six wallpapers drawn in code, each in a light and a dark version.
- **Six demo screens,** including a STATS screen that measures your board, laid out from 240x240 round to 1024x600.
- **Ready for your build:** Arduino library, PlatformIO, ESP-IDF component, Zephyr module, CMake and make.

It works with LVGL 9.3 to 9.6. It has been built and run on a PC, and has not been run on real hardware yet.

### GlassVGL Icons

A frosted-glass icon theme for Linux desktops, in light and dark.

<img src="assets/icons/icons-light.png" alt="GlassVGL Icons, light theme: app, folder and file icons on frosted glass tiles" width="800">

- **397 colour icons** in light and dark, each drawn for 16, 22, 24, 32, 48 and 64 px plus a scalable version.
- **159 symbolic icons** for sidebars and status areas that follow your theme's colours.
- **Apps, folders, 116 file types, devices and emblems** for the GNOME apps and for Debian-based LXDE and labwc desktops.
- **One-command install** that switches your icons and keeps a note of your old theme, and a one-command uninstall that puts it back.

It is built for Ubuntu 24.04 to 26.04 (GNOME 46 to 50) and for Debian-based LXDE and labwc desktops,
and was tested on Ubuntu 24.04 in GNOME Files and in PCManFM. Snap apps, such as Firefox on Ubuntu,
always show their own icons. No brand logos are drawn.

<details>
<summary>More GlassVGL Icons previews</summary>

<img src="assets/icons/icons-dark.png" alt="GlassVGL Icons, dark theme" width="800">

<img src="assets/icons/files-light.png" alt="GNOME Files on Ubuntu 24.04 with GlassVGL Icons, light theme: glass folder icons and the sidebar's symbolic icons" width="800">

<img src="assets/icons/files-dark.png" alt="GNOME Files on Ubuntu 24.04 with GlassVGL Icons, dark theme" width="800">

</details>

### GlassVGL Desktop (design preview)

**Not available yet.** GlassVGL Desktop is not on sale yet, and the pictures below are design
renders of it, not screenshots of a running theme.

A frosted-glass theme for the whole desktop, in light and dark, made to go with GlassVGL Icons.
It will style GTK 3 and GTK 4 apps and the GNOME Shell top bar, dock, menus and notifications, and
come with seven wallpapers drawn to match, each in a light and a dark version that switch with the
desktop's style. It is being made for Ubuntu 24.04 (GNOME 46).

<img src="assets/previews/desktop-light.png" alt="GlassVGL Desktop design render, light scheme: Files and Settings windows with a glass top bar, dock and notification over the prism wallpaper" width="800">

<details>
<summary>More GlassVGL Desktop design renders</summary>

<img src="assets/previews/desktop-dark.png" alt="GlassVGL Desktop design render, dark scheme over the dark prism wallpaper" width="800">

<img src="assets/previews/desktop-light-fullglass.png" alt="GlassVGL Desktop design render, light scheme with the optional full-glass sidebars and headerbars" width="800">

<img src="assets/previews/desktop-dark-fullglass.png" alt="GlassVGL Desktop design render, dark scheme with the optional full-glass sidebars and headerbars" width="800">

<img src="assets/previews/desktop-wallpapers-light.png" alt="The seven GlassVGL Desktop wallpapers, light versions: prism, aurora, sunset, ocean, graphite, blush and meadow" width="800">

<img src="assets/previews/desktop-wallpapers-dark.png" alt="The seven GlassVGL Desktop wallpapers, dark versions" width="800">

Wallpapers, left to right and top to bottom: prism, aurora, sunset, ocean, graphite, blush, meadow.

</details>

### GlassVGL Desktop + Icons bundle

Both packs together, once GlassVGL Desktop is ready: $3.99 less than buying them one at a time on
Gumroad, and $4.99 less on Ko-fi.

<img src="assets/previews/bundle.png" alt="The bundle: a GlassVGL Desktop design render next to the GlassVGL Icons preview sheet" width="800">

Ubuntu is a trademark of Canonical Ltd. GlassVGL is not affiliated with or endorsed by Canonical.

## Sample projects for every LVGL toolchain

| Folder | Toolchain | What it shows |
|---|---|---|
| `examples/GlassVGLDemo` | Arduino IDE, arduino-cli | The six demo screens on any board |
| `examples/GlassVGLPager` | Arduino IDE, arduino-cli | Four demo screens in the swipe pager, with touch |
| `examples/platformio` | PlatformIO | The demo for ESP32-S3, ESP32, ESP32-C3, Raspberry Pi Pico and STM32F429 Discovery |
| `examples/platformio-native` | PlatformIO | The demo on your PC, no board needed |
| `examples/esp-idf` | ESP-IDF 5 | The demo with PSRAM switched on |
| `examples/zephyr` | Zephyr | The demo with GlassVGL as a Zephyr module |
| `examples/desktop` | CMake or make | The demo or any app sample on your PC, writing a picture of the screen |
| `examples/linux` | CMake or make | The demo or any app sample on Linux, in a window (SDL), on the screen through DRM/KMS or on `/dev/fb0`, with evdev touch and keys |
| `examples/showcase` | Any (plain C) | Seven app sample screens to drop into your own project |

The Arduino, PlatformIO and ESP-IDF board samples have one function to fill in, `panel_push()`,
which sends a block of pixels to your display with whatever driver you already use. The pager
sketch also has `touch_read()`. The Zephyr sample uses the display in your board's devicetree.

## Quick start: Arduino, PlatformIO, ESP-IDF, Zephyr, CMake

Put your copy of GlassVGL Pro next to this repository, so the two folders sit side by side as
`GlassVGL` and `GlassVGL-Pro`. The samples look for it there. You also need LVGL 9.3 or newer.

**Arduino IDE.** Add GlassVGL Pro with Sketch > Include Library > Add .ZIP Library, and install
LVGL 9 from the Library Manager. Put an `lv_conf.h` beside the lvgl library folder, as LVGL's
Arduino guide says (`examples/platformio/include/lv_conf.h` works). Then open
`examples/GlassVGLDemo/GlassVGLDemo.ino`.

**PlatformIO.**

Run either line from the top of this repository:

```sh
cd examples/platformio-native && pio run -t exec    # on your PC
cd examples/platformio && pio run -e esp32s3         # or esp32, esp32c3, pico, stm32f429
```

**ESP-IDF.**

```sh
cd examples/esp-idf && idf.py set-target esp32s3 && idf.py build
```

**Zephyr.** From your west workspace, with the Zephyr SDK installed (or, for `native_sim`, set
`ZEPHYR_TOOLCHAIN_VARIANT=host` to use your PC's own compiler):

```sh
west build -b native_sim/native/64 path/to/GlassVGL/examples/zephyr
```

**CMake or make.**

```sh
cd examples/desktop
cmake -S . -B build -DLVGL_DIR=/path/to/lvgl && cmake --build build && ./build/glassvgl_desktop
make LVGL_PATH=/path/to/lvgl && ./glassvgl_desktop
./glassvgl_desktop smart-home.ppm 800 480 smart-home   # an app sample instead of the demo
```

To use an app sample in your own project, add `examples/showcase/glassvgl_showcase.c`,
`glassvgl_showcase_look.c` and the `gv_font_*.c` files in `examples/showcase/fonts` to the build,
and call `glassvgl_showcase_show(GLASSVGL_SHOWCASE_WEATHER);` after starting the theme. For the
look of the screenshots (Inter type, Lucide icons, soft shadows and the ribbon wallpaper), call
`glassvgl_showcase_config(&cfg, disp);` before `lv_glass_theme_init()`. Montserrat 14 must be on
in `lv_conf.h`, because LVGL's own widget symbols fall back to it.

If GlassVGL Pro lives somewhere else, point the sample at it: `-DGLASSVGL_DIR=...` for CMake and
Zephyr, `GLASSVGL_DIR=...` for make, the `symlink://` line in `platformio.ini`, or the `path:` line
in `examples/esp-idf/main/idf_component.yml`.

The theme starts with one call after you create the display:

```c
lv_glass_config_t cfg;
lv_glass_config_init(&cfg);
cfg.scheme = LV_GLASS_SCHEME_DARK;
cfg.accent = LV_GLASS_ACCENT_AZURE;
lv_glass_theme_init(disp, &cfg);
```

## What has been tested

With GlassVGL Pro beside them, these samples were built on a Linux PC by each toolchain's own tool,
with no warnings from GlassVGL's files:

- **Desktop:** the desktop sample, with all seven app samples, with CMake and with make against
  LVGL 9.3.0 and 9.6.0. Every build draws the same frames.
- **PlatformIO on your PC:** `platformio-native` with PlatformIO 6.2 against LVGL 9.3.0 and 9.6.0.
  It runs and writes the same frame on both.
- **Raspberry Pi Pico:** both Arduino sketches with arduino-cli 1.3.1 and the arduino-pico core, and
  the `pico` environment in `examples/platformio`.
- **Zephyr:** `zephyr` on the `native_sim/native/64` board. It builds and runs the demo.
- **Linux:** `linux` with CMake and with make on Ubuntu 24.04 against LVGL 9.3.0 and 9.6.0, with
  all four backends built in. It draws the demo, the pager and all seven app samples in an SDL window, run under Xvfb.

The PlatformIO and Pico builds were done offline, so LVGL came from a local copy, and the `pico`
environment used PlatformIO's standard Raspberry Pi platform in place of the one `platformio.ini`
names. Not verified yet: the ESP-IDF sample, the ESP32, ESP32-S3, ESP32-C3 and STM32F429 builds,
and anything on real hardware, including the Linux runner's DRM, fbdev and evdev backends.

## Licence

The sample code and configuration files are under the MIT licence in [LICENSE](LICENSE). The
GlassVGL name and logo are not covered by it. The fonts in `examples/showcase/fonts` are built from
Inter (SIL Open Font License 1.1) and Lucide icons (ISC licence), and keep those licences. The icon
sample in `icons/` is under CC BY 4.0. The GlassVGL Pro pack the samples build against has its own
paid licence and is not in this repository. Third-party credits are in
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).

The look was inspired by Kyant0's [AndroidLiquidGlass](https://github.com/Kyant0/AndroidLiquidGlass)
(Apache-2.0). No code from it is used.

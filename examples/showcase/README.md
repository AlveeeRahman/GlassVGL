# App sample screens

Seven screens that show what an app looks like with the GlassVGL theme: a media player, a smart
home room, settings, a music library, weather, a round watch face and an energy dashboard. They use
only the theme's public API (`lv_glass.h`) and stock LVGL 9 widgets. The code is under the MIT
licence like the rest of this repository; the fonts in `fonts/` keep their own licences (see below).

```c
lv_glass_config_init(&cfg);
cfg.scheme = LV_GLASS_SCHEME_DARK;
glassvgl_showcase_config(&cfg, disp);   // optional: the look of the screenshots
lv_glass_theme_init(disp, &cfg);
glassvgl_showcase_show(GLASSVGL_SHOWCASE_SMART_HOME);
```

Add `glassvgl_showcase.c`, `glassvgl_showcase_look.c` and the `fonts/gv_font_*.c` files to your
build next to the GlassVGL Edge GUI sources. The landscape screens are
laid out for 800x480 to 1024x600 panels, and 480x320 works with fewer items on some screens. The
watch face is made for round panels from 240x240 to 466x466.

`glassvgl_showcase_config()` sets the Inter fonts, turns on soft panel shadows and draws a ribbon
wallpaper once, into one screen-sized buffer (768 KB at 800x480 in RGB565). Leave it out on boards
without that much RAM, or where redraw time matters: shadows are drawn on every redraw. Montserrat
14 must stay on in `lv_conf.h`, because LVGL's widget symbols fall back to it.

## Fonts

`fonts/` holds Inter with Lucide icons merged in, as LVGL bitmap fonts at 4 bits per pixel, made
by `fonts/generate.py`. Inter is under the SIL Open Font License 1.1 (`fonts/LICENSE-Inter.txt`)
and Lucide under the ISC licence, with some icons from Feather under MIT
(`fonts/LICENSE-Lucide.txt`).

To see one on your PC without a board, use the desktop sample:

```sh
cd ../desktop
cmake -S . -B build -DLVGL_DIR=/path/to/lvgl && cmake --build build
./build/glassvgl_desktop weather.ppm 800 480 weather
```

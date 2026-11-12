# App sample screens

Seven screens that show what an app looks like with the GlassVGL theme: a media player, a smart
home room, settings, a music library, weather, a round watch face and an energy dashboard. They use
only the theme's public API (`lv_glass.h`) and stock LVGL 9 widgets, and are under the MIT licence
like the rest of this repository.

```c
lv_glass_theme_init(disp, &cfg);
glassvgl_showcase_show(GLASSVGL_SHOWCASE_SMART_HOME);
```

Add `glassvgl_showcase.c` to your build next to the GlassVGL Pro sources. The landscape screens are
laid out for 800x480 to 1024x600 panels, and 480x320 works with fewer items on some screens. The
watch face is made for round panels from 240x240 to 466x466.

Turn on Montserrat 12, 14, 16, 20, 24, 28, 36 and 48 in `lv_conf.h` for the sizes in the
screenshots. Any size that is off falls back to the next smaller one.

To see one on your PC without a board, use the desktop sample:

```sh
cd ../desktop
cmake -S . -B build -DLVGL_DIR=/path/to/lvgl && cmake --build build
./build/glassvgl_desktop weather.ppm 800 480 weather
```

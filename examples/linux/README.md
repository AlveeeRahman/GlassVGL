# GlassVGL on Linux

`glassvgl_linux` runs the GlassVGL demo and the app samples on Linux: in a window on a desktop
(SDL), straight on the screen through DRM/KMS, or on the `/dev/fb0` framebuffer, with touch, mouse
and keyboard read through evdev. It builds on Ubuntu 18.04 and newer, on x86-64 and on arm64
(Jetson), and it needs the GlassVGL pack (GlassVGL-Pro) next to an LVGL 9.3 or newer checkout.

## Which backend to use

| Machine | What it runs | Backend |
|---|---|---|
| Ubuntu desktop (lab PC, 22.04, 24.04) | GNOME on X11 or Wayland | `sdl`, a normal window |
| Ubuntu 26.04 desktop | GNOME on Wayland only | `sdl`, through XWayland |
| Jetson Nano, JetPack 4.6.x (Ubuntu 18.04) | console or desktop | `fbdev` on the console, `sdl` on the desktop |
| Jetson Orin, JetPack 5 (Ubuntu 20.04) | no framebuffer console | `drm` |
| Jetson Orin Nano, JetPack 6 (Ubuntu 22.04) | console or desktop | `drm` or `fbdev` on the console, `sdl` on the desktop |

With no `--backend`, the program picks a window when a desktop is running, then DRM, then fbdev.
`--list` prints which backends a build has.

JetPack 4.6.6 (L4T 32.7.6) is the last release for the original Jetson Nano, so it stays on
Ubuntu 18.04. That is why this example also has a plain Makefile: 18.04's CMake 3.10 is older than
LVGL needs.

## Packages

Ubuntu 20.04 and newer:

```sh
sudo apt install build-essential cmake pkg-config libsdl2-dev libdrm-dev libevdev-dev
```

Ubuntu 18.04 (Jetson Nano): the same list. Build with `make`, not CMake.

Only want a console build? Leave out `libsdl2-dev` and build with `BACKENDS="drm fbdev evdev"`
(make) or `-DGLASSVGL_SDL=OFF` (CMake).

## Build

With CMake (Ubuntu 20.04 and newer):

```sh
cmake -S . -B build -DLVGL_DIR=$HOME/lvgl -DGLASSVGL_DIR=$HOME/GlassVGL-Pro
cmake --build build -j
./build/glassvgl_linux --help
```

With make (any Ubuntu, including 18.04):

```sh
make LVGL_PATH=$HOME/lvgl GLASSVGL_DIR=$HOME/GlassVGL-Pro -j4
./glassvgl_linux --help
```

## Cross-compile for a Jetson from a PC

Copy the Jetson's root filesystem (or an Ubuntu Base image of the same release with the `-dev`
packages above installed) to the PC, then point `SYSROOT` at it:

```sh
sudo apt install gcc-aarch64-linux-gnu
make LVGL_PATH=$HOME/lvgl GLASSVGL_DIR=$HOME/GlassVGL-Pro \
     CC=aarch64-linux-gnu-gcc SYSROOT=$HOME/jetson-rootfs -j8
```

The Makefile links against the sysroot's own C library and links libgcc statically, so a binary
built this way on a newer PC still runs on Ubuntu 18.04. Copy `glassvgl_linux` to the Jetson and run it.

## Run

```sh
./glassvgl_linux                                   # the demo, in a window
./glassvgl_linux --screen smart-home --fullscreen  # one sample, full screen
./glassvgl_linux --backend drm                     # on the screen, from a console
./glassvgl_linux --backend fbdev                   # on /dev/fb0, from a console
./glassvgl_linux --screen weather --shot weather.ppm   # save a screenshot and exit
```

DRM and fbdev need the screen to themselves, so run them from a text console (Ctrl+Alt+F3), or
stop the desktop first with `sudo systemctl stop gdm`. Your user needs to be in the `video` group
for the screen and the `input` group for touch and keys:

```sh
sudo usermod -aG video,input $USER    # then log out and back in
```

When a device can't be opened, the program says why: the device isn't there, your user has no
permission for it, or another program (usually the desktop) already holds it.

## Tested

| Build | Result |
|---|---|
| Ubuntu 24.04 x86-64, CMake, LVGL 9.6 and 9.3 | builds with no warnings, renders the samples |
| Ubuntu 18.04 arm64, gcc 7.5, make, SDL 2.0.8 | builds with no warnings, renders smart-home |
| Cross from Ubuntu 24.04 to 18.04 arm64 | needs only GLIBC 2.17, renders weather on 18.04 |

These were run under Xvfb and QEMU. DRM, fbdev and evdev on real Jetson hardware are still to be
tried on the board.

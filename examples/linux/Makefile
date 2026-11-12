# GlassVGL Linux runner with plain make, for machines whose CMake is too old for LVGL (Ubuntu 18.04
# on a Jetson Nano ships CMake 3.10; LVGL wants 3.12.4 or newer).
#
#   make LVGL_PATH=/path/to/lvgl GLASSVGL_DIR=/path/to/GlassVGL-Pro -j4
#   make ... BACKENDS="fbdev evdev"     (no desktop libraries needed)
#   make ... CC=aarch64-linux-gnu-gcc SYSROOT=/path/to/jetson-rootfs   (cross-compile)
#
# BACKENDS is any of: sdl drm fbdev evdev. COLOR_DEPTH is 32 (default) or 16.
ifndef LVGL_PATH
$(error set LVGL_PATH to an LVGL 9.3 or newer checkout)
endif
GLASSVGL_DIR ?= ../../../GlassVGL-Pro
BACKENDS ?= sdl drm fbdev evdev
COLOR_DEPTH ?= 32

CC ?= cc
PKG_CONFIG ?= pkg-config
ifdef SYSROOT
TRIPLE ?= aarch64-linux-gnu
CFLAGS += --sysroot=$(SYSROOT)
# search the target's own glibc and start files before the cross compiler's newer ones
LDFLAGS += --sysroot=$(SYSROOT) -B$(SYSROOT)/usr/lib/$(TRIPLE) \
           -L$(SYSROOT)/lib/$(TRIPLE) -L$(SYSROOT)/usr/lib/$(TRIPLE) \
           -Wl,-rpath-link,$(SYSROOT)/lib/$(TRIPLE):$(SYSROOT)/usr/lib/$(TRIPLE) -static-libgcc
export PKG_CONFIG_SYSROOT_DIR := $(SYSROOT)
export PKG_CONFIG_LIBDIR := $(SYSROOT)/usr/lib/$(TRIPLE)/pkgconfig:$(SYSROOT)/usr/lib/pkgconfig:$(SYSROOT)/usr/share/pkgconfig
endif

CFLAGS ?= -O2
CFLAGS += -Wall -I. -I../showcase -DLV_CONF_INCLUDE_SIMPLE -DGLASSVGL_COLOR_DEPTH=$(COLOR_DEPTH)
LIBS := -lm

has = $(filter $(1),$(BACKENDS))
ifneq ($(call has,sdl),)
CFLAGS += -DLV_USE_SDL=1 $(shell $(PKG_CONFIG) --cflags sdl2)
LIBS += $(shell $(PKG_CONFIG) --libs sdl2)
# SDL 2.0.8 (Ubuntu 18.04, so JetPack 4) leaves the pixel-format enum unnamed; LVGL uses the name
SDL_PIXELS_H := $(SYSROOT)$(shell $(PKG_CONFIG) --variable=includedir sdl2)/SDL2/SDL_pixels.h
ifeq ($(shell grep -c '} SDL_PixelFormatEnum;' $(SDL_PIXELS_H) 2>/dev/null),0)
CFLAGS += -DSDL_PixelFormatEnum=Uint32
endif
endif
ifneq ($(call has,drm),)
CFLAGS += -DLV_USE_LINUX_DRM=1 $(shell $(PKG_CONFIG) --cflags libdrm)
LIBS += $(shell $(PKG_CONFIG) --libs libdrm)
endif
ifneq ($(call has,fbdev),)
CFLAGS += -DLV_USE_LINUX_FBDEV=1
endif
ifneq ($(call has,evdev),)
CFLAGS += -DLV_USE_EVDEV=1 $(shell $(PKG_CONFIG) --cflags libevdev)
LIBS += $(shell $(PKG_CONFIG) --libs libevdev)
endif

CSRCS :=
include $(LVGL_PATH)/lvgl.mk
include $(GLASSVGL_DIR)/glassvgl.mk
CSRCS += main.c ../showcase/glassvgl_showcase.c ../showcase/glassvgl_showcase_look.c \
         $(wildcard ../showcase/fonts/gv_font_*.c)

OBJDIR ?= obj
OBJS := $(patsubst %.c,$(OBJDIR)/%.o,$(abspath $(CSRCS)))

TARGET ?= glassvgl_linux
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET)

.PHONY: clean

# GlassVGL desktop example with plain make, using lvgl.mk and glassvgl.mk.
#   make LVGL_PATH=/path/to/lvgl [GLASSVGL_DIR=/path/to/GlassVGL-Pro]
#   ./glassvgl_desktop
ifndef LVGL_PATH
$(error set LVGL_PATH to an LVGL 9.3 or newer checkout)
endif
GLASSVGL_DIR ?= ../../../GlassVGL-Pro

CC ?= cc
CFLAGS ?= -O2 -Wall -Wextra
CFLAGS += -I. -I../showcase -DLV_CONF_INCLUDE_SIMPLE

CSRCS :=
include $(LVGL_PATH)/lvgl.mk
include $(GLASSVGL_DIR)/glassvgl.mk

OBJDIR := obj
SHOWCASE := ../showcase/glassvgl_showcase.c ../showcase/glassvgl_showcase_look.c $(wildcard ../showcase/fonts/gv_font_*.c)
OBJS := $(patsubst %.c,$(OBJDIR)/%.o,$(abspath $(CSRCS))) $(OBJDIR)/main.o \
        $(patsubst ../showcase/%.c,$(OBJDIR)/showcase/%.o,$(SHOWCASE))

glassvgl_desktop: $(OBJS)
	$(CC) -o $@ $^ -lm

$(OBJDIR)/main.o: main.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/showcase/%.o: ../showcase/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) glassvgl_desktop

.PHONY: clean

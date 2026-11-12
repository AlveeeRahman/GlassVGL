# GlassVGL demo for Zephyr. Build it from your west workspace:
#   west build -b native_sim/native/64 path/to/GlassVGL/examples/zephyr
# This file adds the GlassVGL pack as a module by itself, so the workspace only needs Zephyr and
# LVGL. It looks for a GlassVGL-Pro checkout next to this repository; pass -DGLASSVGL_DIR=... to
# use another place.
cmake_minimum_required(VERSION 3.20.0)
set(GLASSVGL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../GlassVGL-Pro" CACHE PATH "the GlassVGL pack")
list(APPEND EXTRA_ZEPHYR_MODULES ${GLASSVGL_DIR})
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(glassvgl_zephyr_demo)
target_sources(app PRIVATE src/main.c)

# GlassVGL Linux runner: the GlassVGL demo and app samples on a Linux desktop, a DRM/KMS screen or
# the /dev/fb0 framebuffer, with evdev input. Works on Ubuntu 18.04 (Jetson Nano) and newer.
#
#   cmake -S . -B build -DLVGL_DIR=/path/to/lvgl -DGLASSVGL_DIR=/path/to/GlassVGL-Pro
#   cmake --build build -j
#   ./build/glassvgl_linux --help
#
# Backends: GLASSVGL_SDL, GLASSVGL_DRM, GLASSVGL_FBDEV and GLASSVGL_EVDEV, each AUTO (built when its
# library is installed), ON (required) or OFF. GLASSVGL_COLOR_DEPTH is 32 (default) or 16.
cmake_minimum_required(VERSION 3.13)
project(glassvgl_linux C)

set(LVGL_DIR "" CACHE PATH "LVGL 9.3 or newer source tree")
if(NOT LVGL_DIR)
  message(FATAL_ERROR "set -DLVGL_DIR to an LVGL 9 checkout")
endif()
set(GLASSVGL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../GlassVGL-Pro" CACHE PATH "the GlassVGL pack")
if(NOT EXISTS "${GLASSVGL_DIR}/CMakeLists.txt")
  message(FATAL_ERROR "GlassVGL pack not found at ${GLASSVGL_DIR}; set -DGLASSVGL_DIR")
endif()

set(GLASSVGL_SDL AUTO CACHE STRING "SDL2 desktop window: AUTO, ON or OFF")
set(GLASSVGL_DRM AUTO CACHE STRING "DRM/KMS full screen: AUTO, ON or OFF")
set(GLASSVGL_FBDEV ON CACHE STRING "Linux framebuffer /dev/fb0: ON or OFF")
set(GLASSVGL_EVDEV AUTO CACHE STRING "evdev touch, mouse and keyboard: AUTO, ON or OFF")
set(GLASSVGL_COLOR_DEPTH 32 CACHE STRING "32 (XRGB8888) or 16 (RGB565)")

find_package(PkgConfig REQUIRED)

# Resolves one backend: sets <var>_ON and, for a library backend, the pkg-config target.
function(glassvgl_backend var pkg)
  set(want "${${var}}")
  set(on OFF)
  if(NOT want STREQUAL "OFF")
    if(pkg)
      pkg_check_modules(${var}_PKG IMPORTED_TARGET GLOBAL ${pkg})
      if(${var}_PKG_FOUND)
        set(on ON)
      elseif(want STREQUAL "ON")
        message(FATAL_ERROR "${var}=ON but ${pkg} is not installed")
      endif()
    else()
      set(on ON)
    endif()
  endif()
  set(${var}_ON ${on} PARENT_SCOPE)
  message(STATUS "GlassVGL Linux: ${var} ${on}")
endfunction()

glassvgl_backend(GLASSVGL_SDL sdl2)
glassvgl_backend(GLASSVGL_DRM libdrm)
glassvgl_backend(GLASSVGL_FBDEV "")
glassvgl_backend(GLASSVGL_EVDEV libevdev)

set(backend_defs
    GLASSVGL_COLOR_DEPTH=${GLASSVGL_COLOR_DEPTH}
    $<IF:$<BOOL:${GLASSVGL_SDL_ON}>,LV_USE_SDL=1,LV_USE_SDL=0>
    $<IF:$<BOOL:${GLASSVGL_DRM_ON}>,LV_USE_LINUX_DRM=1,LV_USE_LINUX_DRM=0>
    $<IF:$<BOOL:${GLASSVGL_FBDEV_ON}>,LV_USE_LINUX_FBDEV=1,LV_USE_LINUX_FBDEV=0>
    $<IF:$<BOOL:${GLASSVGL_EVDEV_ON}>,LV_USE_EVDEV=1,LV_USE_EVDEV=0>)

# LVGL reads lv_conf.h from this folder; the backend switches reach it as compile definitions.
set(LV_BUILD_CONF_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lv_conf.h CACHE PATH "" FORCE)
set(CONFIG_LV_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_USE_THORVG_INTERNAL OFF CACHE BOOL "" FORCE)
set(LV_BUILD_INSTALL OFF CACHE BOOL "" FORCE)
set(LV_FETCH_DEPENDENCIES OFF CACHE BOOL "" FORCE)
add_subdirectory(${LVGL_DIR} lvgl EXCLUDE_FROM_ALL)
target_compile_definitions(lvgl PUBLIC ${backend_defs})
if(GLASSVGL_SDL_ON)
  target_link_libraries(lvgl PUBLIC PkgConfig::GLASSVGL_SDL_PKG)
  # SDL 2.0.8 (Ubuntu 18.04, so JetPack 4) leaves the pixel-format enum unnamed; LVGL uses the name
  find_file(GLASSVGL_SDL_PIXELS_H SDL_pixels.h HINTS ${GLASSVGL_SDL_PKG_INCLUDE_DIRS} PATH_SUFFIXES SDL2)
  if(GLASSVGL_SDL_PIXELS_H)
    file(STRINGS "${GLASSVGL_SDL_PIXELS_H}" _sdl_enum REGEX "} SDL_PixelFormatEnum;")
    if(NOT _sdl_enum)
      target_compile_definitions(lvgl PUBLIC SDL_PixelFormatEnum=Uint32)
    endif()
  endif()
endif()
if(GLASSVGL_DRM_ON)
  target_link_libraries(lvgl PUBLIC PkgConfig::GLASSVGL_DRM_PKG)
endif()
if(GLASSVGL_EVDEV_ON)
  target_link_libraries(lvgl PUBLIC PkgConfig::GLASSVGL_EVDEV_PKG)
endif()

add_subdirectory(${GLASSVGL_DIR} glassvgl)

file(GLOB SHOWCASE_FONTS ${CMAKE_CURRENT_SOURCE_DIR}/../showcase/fonts/gv_font_*.c)
add_executable(glassvgl_linux main.c ../showcase/glassvgl_showcase.c ../showcase/glassvgl_showcase_look.c
               ${SHOWCASE_FONTS})
target_include_directories(glassvgl_linux PRIVATE ../showcase)
target_link_libraries(glassvgl_linux PRIVATE glassvgl lvgl m)
install(TARGETS glassvgl_linux RUNTIME DESTINATION bin)

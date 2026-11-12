# GlassVGL desktop example: LVGL and GlassVGL pulled in with add_subdirectory().
#   cmake -S . -B build -DLVGL_DIR=/path/to/lvgl [-DGLASSVGL_DIR=/path/to/GlassVGL-Pro]
#   cmake --build build && ./build/glassvgl_desktop [out.ppm] [width] [height] [screen]
# GLASSVGL_DIR defaults to a GlassVGL-Pro checkout next to this repository.
cmake_minimum_required(VERSION 3.16)
project(glassvgl_desktop C)

set(LVGL_DIR "" CACHE PATH "LVGL 9.3 or newer source tree")
if(NOT LVGL_DIR)
  message(FATAL_ERROR "set -DLVGL_DIR to an LVGL 9 checkout")
endif()

# LVGL reads lv_conf.h from this folder.
set(LV_BUILD_CONF_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lv_conf.h CACHE PATH "" FORCE)
set(CONFIG_LV_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CONFIG_LV_USE_THORVG_INTERNAL OFF CACHE BOOL "" FORCE)
set(LV_BUILD_INSTALL OFF CACHE BOOL "" FORCE)
set(LV_FETCH_DEPENDENCIES OFF CACHE BOOL "" FORCE)
add_subdirectory(${LVGL_DIR} lvgl)

set(GLASSVGL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../GlassVGL-Pro" CACHE PATH "the GlassVGL pack")
if(NOT EXISTS "${GLASSVGL_DIR}/CMakeLists.txt")
  message(FATAL_ERROR "GlassVGL pack not found at ${GLASSVGL_DIR}; set -DGLASSVGL_DIR")
endif()
add_subdirectory(${GLASSVGL_DIR} glassvgl)

add_executable(glassvgl_desktop main.c ../showcase/glassvgl_showcase.c)
target_include_directories(glassvgl_desktop PRIVATE ../showcase)
target_link_libraries(glassvgl_desktop PRIVATE glassvgl lvgl m)

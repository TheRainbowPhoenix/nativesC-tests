# Configure with [fxsdk build-fx] or [fxsdk build-cg], which provide the
# toolchain file and module path of the fxSDK

cmake_minimum_required(VERSION 3.15)
project(CED)

include(GenerateG1A)
include(GenerateG3A)
include(GenerateHH3)
include(Fxconv)
find_package(Gint 2.9 REQUIRED)
find_package(JustUI 1.0 REQUIRED)

set(SOURCES
  src/main.cpp
  src/ncinput.cpp
  src/ced.cpp
  src/filebrowser.cpp
  src/goto.cpp
  src/search.cpp
  src/outline.cpp
  src/problems.cpp
  src/stubs.cpp
)
# Shared assets, fx-9860G-only assets and fx-CG-50-only assets
set(ASSETS
  # ...
)
set(ASSETS_fx
  # ...
)
set(ASSETS_cg
  # ...
)

fxconv_declare_assets(${ASSETS} ${ASSETS_fx} ${ASSETS_cg} WITH_METADATA)

add_executable(ced ${SOURCES} ${ASSETS} ${ASSETS_${FXSDK_PLATFORM}})
target_compile_options(ced PRIVATE -Wall -Wextra -Os -g -flto=auto -fno-exceptions -fno-rtti -fno-threadsafe-statics)
target_compile_features(ced PRIVATE cxx_std_17)
target_link_libraries(ced Gint::Gint JustUI::JustUI stdc++ m c)

if("${FXSDK_PLATFORM_LONG}" STREQUAL fx9860G)
  generate_g1a(TARGET ced OUTPUT "CED.g1a"
    NAME "CED" ICON assets-fx/icon.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fxCG50)
  generate_g3a(TARGET ced OUTPUT "CED.g3a"
    NAME "CED" ICONS assets-cg/icon-uns.png assets-cg/icon-sel.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fx9860G_G3A)
  generate_g3a(TARGET ced OUTPUT "CED-fx.g3a"
    NAME "CED-fx" ICONS assets-cg/icon-uns.png assets-cg/icon-sel.png)
elseif("${FXSDK_PLATFORM_LONG}" STREQUAL fxCP)
  generate_hh3(TARGET ced OUTPUT "CED.hh3")
endif()

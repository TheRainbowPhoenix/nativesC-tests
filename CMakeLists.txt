# Configure with [fxsdk build-fx] or [fxsdk build-cg], which provide the
# toolchain file and module path of the fxSDK

cmake_minimum_required(VERSION 3.15)
project(CED)

include(GenerateG1A)
include(GenerateG3A)
include(GenerateHH3)
include(Fxconv)
find_package(Gint 2.9 REQUIRED)

set(SOURCES
  src/main.cpp
  src/ced.cpp
  src/ncinput.cpp
  src/navigation.cpp
  src/search.cpp
  src/analysis.cpp
)

add_executable(ced ${SOURCES})
target_compile_options(ced PRIVATE -Wall -Wextra -Os -g -flto=auto -fno-exceptions -fno-rtti)
target_link_libraries(ced Gint::Gint)

if("${FXSDK_PLATFORM_LONG}" STREQUAL fxCP)
  generate_hh3(TARGET ced OUTPUT "CED.hh3")
endif()

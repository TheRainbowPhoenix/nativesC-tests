cmake_minimum_required(VERSION 3.15)
project(CED)

set(SDK_DIR "/sdk")
include_directories("${SDK_DIR}/include")
include_directories("${SDK_DIR}/include/sdk")
include_directories("src")

set(SOURCES
  src/main.cpp
  src/ced.cpp
  src/ncinput.cpp
  src/navigation.cpp
  src/search.cpp
  src/analysis.cpp
  src/nrender.cpp
  src/nui.cpp
)

add_executable(ced ${SOURCES})
target_compile_options(ced PRIVATE -Wall -Wextra -Os -g -flto=auto -fno-exceptions -fno-rtti)
target_link_libraries(ced PRIVATE "-L${SDK_DIR}" lsdk)

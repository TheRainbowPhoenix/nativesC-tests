cmake_minimum_required(VERSION 3.15)
project(CED)

# Hollyhock-3 pure build
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
)

add_executable(ced ${SOURCES})
target_compile_options(ced PRIVATE -Wall -Wextra -Os -g -flto=auto -fno-exceptions -fno-rtti)
target_link_libraries(ced PRIVATE "-L${SDK_DIR}" lsdk)

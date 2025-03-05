cmake_minimum_required(VERSION 3.25)

project(KBSwitch VERSION 1.0.0 LANGUAGES CXX)

if(NOT WIN32)
  message(FATAL_ERROR "This project is only for Windows")
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(WIL_BUILD_PACKAGING OFF)
set(WIL_BUILD_TESTS OFF)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(WIL)
include(argparse)
include(spdlog)

# Configure version header
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/include/kbswitch/version.h.in"
  "${CMAKE_CURRENT_BINARY_DIR}/include/kbswitch/version.h"
  @ONLY
)

add_executable(kbswitch "src/main.cc")
target_compile_features(kbswitch PRIVATE cxx_std_23)
target_compile_options(kbswitch PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:
    -Wall -Wextra -Wpedantic
    -Wno-language-extension-token
    -Wno-cast-function-type-mismatch
    -Wno-gnu-zero-variadic-macro-arguments>
)
target_link_libraries(
  kbswitch
  PRIVATE argparse
  PRIVATE spdlog
  PRIVATE WIL
  PRIVATE Shlwapi
)
target_include_directories(kbswitch 
  PRIVATE "include/"
  PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/include/"
)
target_sources(kbswitch
  PRIVATE "src/keyboard_layout_registry.cc"
  PRIVATE "src/keyboard_layout_manager.cc"
  PRIVATE "src/command_executor.cc"
  PRIVATE "src/encoding.cc")

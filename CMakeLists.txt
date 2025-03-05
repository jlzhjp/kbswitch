cmake_minimum_required(VERSION 3.25)

project(KBSwitch VERSION 0.1.0 LANGUAGES CXX)

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

add_executable(kbswitch src/main.cxx)
target_compile_features(kbswitch PRIVATE cxx_std_23)
target_link_libraries(
  kbswitch
  PRIVATE argparse
  PRIVATE spdlog
  PRIVATE WIL
  PRIVATE Shlwapi
)
target_include_directories(kbswitch PRIVATE "include/")
target_sources(kbswitch
  PRIVATE "src/keyboard_layout_registry.cc"
  PRIVATE "src/keyboard_layout_manager.cc"
  PRIVATE "src/command_executor.cc"
  PRIVATE "src/encoding.cc")

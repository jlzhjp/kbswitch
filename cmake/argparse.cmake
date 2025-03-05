include_guard()

include(FetchContent)

FetchContent_Declare(
  argparse
  GIT_TAG v3.2
  GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)

FetchContent_MakeAvailable(argparse)
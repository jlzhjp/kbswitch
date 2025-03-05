include_guard()

include(FetchContent)

FetchContent_Declare(
  WIL
  GIT_TAG v1.0.240803.1
  GIT_REPOSITORY https://github.com/microsoft/wil.git
)

FetchContent_MakeAvailable(WIL)

include_guard()

include(FetchContent)

FetchContent_Declare(
  spdlog
  GIT_TAG v1.15.1
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
)

FetchContent_MakeAvailable(spdlog)
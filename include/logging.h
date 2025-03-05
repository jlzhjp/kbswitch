#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace kbswitch {

struct LoggerMixin {
protected:
  template <typename T>
  spdlog::logger& logger(this T& self);
};

} // namespace kbswitch

template <typename T>
spdlog::logger& kbswitch::LoggerMixin::logger(this T&) {
  static std::shared_ptr<spdlog::logger> logger = spdlog::stderr_color_mt(typeid(T).name());
  return *logger;
}
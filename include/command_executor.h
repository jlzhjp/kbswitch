#pragma once

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>
#include <string>

#include "logging.h"

namespace kbswitch {

class CommandExecutor : LoggerMixin {
public:
  CommandExecutor();

  void query_keyboard_layouts() const;
  void query_current_active_layout() const;
  void activate_keyboard_layout(const std::string& klid) const;
  void cache_keyboard_layout() const;
  void restore_keyboard_layout() const;

private:
  void* _std_output;
};

} // namespace kbswitch

#pragma once

#include <string>

#include "logging.h"

namespace kbswitch {

class KeyboardLayoutManager : public LoggerMixin {
public:
  static KeyboardLayoutManager& instance();
  KeyboardLayoutManager();
  std::wstring get_current_layout_name() const;
  void activate_for_foreground(const std::wstring& klid) const;
};

} // namespace kbswitch
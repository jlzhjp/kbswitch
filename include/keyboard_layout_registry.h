#pragma once

#include <optional>
#include <string>
#include <vector>

#include "logging.h"

namespace kbswitch {

struct KeyboardLayoutEntry {
  std::wstring klid;
  std::wstring layout_text;
  std::wstring layout_file;
  std::wstring layout_display_name;
};

struct KeyboardLayoutPreloadEntry : public LoggerMixin {
  std::wstring index;
  std::wstring klid;

  KeyboardLayoutEntry query_info() const;
};

class KeyboardLayoutPreloadRegistry : public LoggerMixin {
  std::vector<KeyboardLayoutPreloadEntry> _entries;
  KeyboardLayoutPreloadRegistry();

  std::optional<std::wstring> find_substitute(const std::wstring& klid) const;

public:
  static KeyboardLayoutPreloadRegistry& instance();
  const std::vector<KeyboardLayoutPreloadEntry>& entries() const;
};

} // namespace kbswitch
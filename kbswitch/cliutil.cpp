#include "cliutil.h"

#include <cassert>

namespace kb::cli {

std::wstring fill_right(const std::wstring &content, size_t width, wchar_t ch) {
  assert(content.size() <= width);
  return content + std::wstring(width - content.size(), ch);
}

void _basic_write(DWORD handle, const std::wstring &text) {
  auto hOutput = GetStdHandle(handle);
  int size = static_cast<int>(text.size());
  WriteConsoleW(hOutput, text.c_str(), size, nullptr, nullptr);
}

} // namespace kb::cli

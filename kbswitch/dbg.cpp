#include "dbg.h"

namespace kb::dbg {

bool _firstLog = true;

std::wstring get_time() {
  constexpr size_t BUFFER_SIZE = 32;

  std::time_t time = std::time(nullptr);
  std::tm tm;

  localtime_s(&tm, &time);

  std::unique_ptr<wchar_t[]> buffer(new wchar_t[BUFFER_SIZE]);

  _wasctime_s(buffer.get(), BUFFER_SIZE, &tm);
  std::wstring timestr(buffer.get());

  // remove '\n'
  timestr.pop_back();
  return timestr;
}

} // namespace kb::dbg

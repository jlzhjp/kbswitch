#pragma once

#include <string>

namespace kbswitch {

std::string wstr2con(const std::wstring& wstr);
std::wstring con2wstr(const std::string& str);

} // namespace kbswitch
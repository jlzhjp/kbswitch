#include "encoding.h"

#include <Windows.h>

#include <consoleapi.h>
#include <spdlog/logger.h>
#include <spdlog/sinks/stdout_color_sinks.h>

std::string kbswitch::wstr2con(const std::wstring& s) {
  int output_size
    = WideCharToMultiByte(GetConsoleOutputCP(), 0, s.c_str(), -1, nullptr, 0, nullptr, nullptr);
  // count includes the null terminator, but we don't want it
  std::string result(output_size, '\0');
  int actual_size
    = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, result.data(), output_size, nullptr, nullptr);
  if (actual_size < output_size && actual_size == 0) return "<ERROR>";
  result.resize(actual_size - 1);
  return result;
}
#include "encoding.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <spdlog/spdlog.h>

std::string kbswitch::wstr2con(const std::wstring& s) {
  int output_size = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, nullptr, 0, nullptr, nullptr);
  // count includes the null terminator, but we don't want it
  std::string result(output_size, '\0');
  int actual_size
    = WideCharToMultiByte(CP_UTF8, 0, s.c_str(), -1, result.data(), output_size, nullptr, nullptr);
  if (actual_size < output_size && actual_size == 0) {
    spdlog::error("WideCharToMultiByte failed: {}", GetLastError());
    return "<ERROR>";
  }
  result.resize(actual_size - 1);
  return result;
}

std::wstring kbswitch::con2wstr(const std::string& s) {
  int output_size = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
  // count includes the null terminator, but we don't want it
  std::wstring result(output_size, L'\0');
  int actual_size = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, result.data(), output_size);
  if (actual_size < output_size && actual_size == 0) {
    spdlog::error("MultiByteToWideChar failed: {}", GetLastError());
    return L"<ERROR>";
  }
  result.resize(actual_size - 1);
  return result;
}
#pragma once

#include <string>

#include <Windows.h>

namespace kb::cli {

void _basic_write(DWORD handle, const std::wstring &text);

template <typename TFirst, typename... TRest>
void _write_impl(DWORD handle, const TFirst &first, const TRest &...rest) {
  if constexpr (std::is_convertible_v<TFirst, std::wstring>) {
    _basic_write(handle, first);
  } else {
    _basic_write(handle, std::to_wstring(first));
  }

  if constexpr (sizeof...(TRest) != 0) {
    _basic_write(handle, L" ");
    _write_impl(handle, rest...);
  }
}

template <typename... TVar>
void write(const TVar &...args) {
  if constexpr (sizeof...(TVar) != 0) {
    _write_impl(STD_OUTPUT_HANDLE, args...);
  }
}

template <typename... TVar>
void writeln(const TVar &...args) {
  write(args..., L"\n");
}

template <typename... TVar>
int write_error(const TVar &...args) {
  if constexpr (sizeof...(TVar) != 0) {
    _write_impl(STD_ERROR_HANDLE, args...);
  }

  return -1;
}

template <typename... TVar>
int writeln_error(const TVar &...args) {
  return write_error(args..., L"\n");
}

constexpr auto KB_ERROR_PROMPT = L"[ERROR]";
constexpr auto KB_WARNING_PROMPT = L"[WARNING]";

template <typename... TVar>
int prompt_error(const TVar &...args) {
  return writeln_error(KB_ERROR_PROMPT, args...);
}

template <typename... TVar>
void prompt_warning(const TVar &...args) {
  writeln(KB_WARNING_PROMPT, args...);
}

[[nodiscard]] std::wstring fill_right(
    const std::wstring &content, size_t width, wchar_t ch = L' ');

} // namespace kb::cli

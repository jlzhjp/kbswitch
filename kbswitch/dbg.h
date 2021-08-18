#pragma once

#include <filesystem>
#include <fstream>
#include <string>

#include "cliutil.h"

namespace kb::dbg {

namespace fs = std::filesystem;

const auto LOG_FILE = fs::temp_directory_path() / L"_kbswitch_log";

extern bool _firstLog;

[[nodiscard]] std::wstring get_time();

template <typename... TVar>
void _log(const TVar &...args) {
  if constexpr (sizeof...(TVar) == 0) {
    cli::writeln();
  } else {
    cli::write(L"[DEBUG]", args...);
    cli::writeln(L"|");
  }
}

template <typename... TVar>
void _log_file(const TVar &...args) {
  std::wofstream ofs(LOG_FILE, std::ios_base::app);

  if (_firstLog) {
    ofs << L"\n";
    _firstLog = false;
  }

  if constexpr (sizeof...(TVar) == 0) {
    ofs << L"\n";
  } else {
    ofs << L"[LOG]";
    ((ofs << L" " << args), ...) << L"\n";
  }
}

} // namespace kb::dbg

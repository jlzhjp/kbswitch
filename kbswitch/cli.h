#pragma once

#include <functional>
#include <string>
#include <vector>
#include <Windows.h>

namespace kb::cli {

struct Command {

  using ArgList = std::vector<std::wstring>;
  using Action = std::function<int(ArgList::const_iterator begin, ArgList::const_iterator end)>;

  Command(std::wstring name, std::wstring desc, Action action) :
      name(name), desc(desc), action(action) {}

  int operator()(ArgList::const_iterator begin, ArgList::const_iterator end) {
    return this->action(begin, end);
  }

  std::wstring name;
  std::wstring desc;
  Action action;
};

int parse_args(int argc, wchar_t **argv);

void writeln(const std::wstring &text = L"", DWORD handle = STD_OUTPUT_HANDLE);

int writelnerr(const std::wstring &text);

void promptwrng(const std::wstring &text);

int prompterr(const std::wstring &text);

std::vector<std::wstring> convert_args(int argc, wchar_t **argv);

std::wstring generate_help_message();

} // namespace kb::cli

#include "cli.h"

#include <cassert>
#include <sstream>

#include "kbdef.h"
#include "kbutil.h"

namespace kb::cli {

constexpr size_t FILL_WIDTH = 10;
constexpr auto ERROR_PROMPT = L"[ERROR] ";
constexpr auto WARNING_PROMPT = L"[WARNING] ";

using Iter = Command::ArgList::const_iterator;
std::vector<Command> commands({
    Command(L"cache",
        L"cache the current keyboard layout state",
        [](Iter begin, Iter end) {
          if (begin != end) {
            return prompterr(L"Too many arguments");
          }
          cache_keyborad_layout_state();
          return 0;
        }),

    Command(L"restore",
        L"save the current keyboard layout state",
        [](Iter begin, Iter end) {
          if (begin != end) {
            return prompterr(L"Too many arguments.");
          }

          return restore_keyboard_layout_state() ?
                     0 :
                     prompterr(L"No cache found.");
        }),

    Command(L"set",
        L"switch to the given keyboard layout index",
        [](Iter begin, Iter end) {
          if (begin == end) {
            return prompterr(L"No <index> provided.");
          } else if (std::distance(begin, end) > 1) {
            return prompterr(L"Too many arguments.");
          }

          int index = 0;
          try {
            index = std::stoi(*begin);
          } catch (const std::invalid_argument &) {
            return prompterr(L"Invalid index: " + *begin + L".");
          }

          auto keyboards = get_user_klid_list();

          if (index < 0 ||
              static_cast<std::size_t>(index) >= keyboards.size()) {
            return prompterr(std::wstring(L"Index out of range.\n") +
                             L"The valid range is [0," +
                             std::to_wstring(keyboards.size()) + L").");
          }

          set_keyboard_layout(
              GetForegroundWindow(), keyboards[static_cast<size_t>(index)]);

          return 0;
        }),

    Command(L"list",
        L"list all installed keyboard layouts",
        [](Iter begin, Iter end) {
          if (begin != end) {
            return prompterr(L"Too many arguments.");
          }

          auto map = get_klid_to_text_map();
          auto keyboards = get_user_klid_list();

          for (size_t i = 0; i != keyboards.size(); ++i) {
            writeln(std::to_wstring(i) + L"  =>  " + map[keyboards[i]]);
          }

          return 0;
        }),

    Command(L"help",
        L"print help message",
        [](Iter, Iter) {
          writeln(generate_help_message());
          return 0;
        }),

    Command(L"version",
        L"print version",
        [](Iter, Iter) {
          writeln(KB_VER);
          return 0;
        }),
});

int parse_args(int argc, wchar_t **argv) {
  std::vector<std::wstring> args = convert_args(argc, argv);

  if (argc <= 1) {
    prompterr(L"No command provided.\n");
    writeln(generate_help_message());
    return -1;
  }

  std::wstring &commandText = args[1];
  auto itcmd = std::find_if(commands.begin(),
      commands.end(),
      [&commandText](Command &cmd) { return cmd.name == commandText; });

  if (itcmd == commands.end()) {
    return prompterr(L"No such command: " + commandText + L".");
  }

  return (*itcmd)(args.begin() + 2, args.end());
}

void writeln(const std::wstring &text, DWORD handle) {
  auto hOutput = GetStdHandle(handle);
  WriteConsole(
      hOutput, text.c_str(), static_cast<int>(text.size()), nullptr, nullptr);
  WriteConsole(hOutput, L"\n", 1, nullptr, nullptr);
}

int writelnerr(const std::wstring &text) {
  writeln(text, STD_ERROR_HANDLE);
  return -1;
}

void promptwrng(const std::wstring &text) {
  writeln(WARNING_PROMPT + text);
}

int prompterr(const std::wstring &text) {
  return writelnerr(ERROR_PROMPT + text);
}

std::vector<std::wstring> convert_args(int argc, wchar_t **argv) {
  std::vector<std::wstring> res;
  for (int i = 0; i < argc; ++i) {
    res.push_back(argv[i]);
  }
  return res;
}

std::wstring fill_right(
    const std::wstring &content, size_t width, wchar_t ch = L' ') {
  assert(content.size() <= width);
  return content + std::wstring(width - content.size(), ch);
}

std::wstring generate_help_message() {
  std::wstringstream stream;
  stream << L"usage: kbswitch.exe <command> [<args>]\n\n";

  for (auto &cmd : commands) {
    stream << fill_right(cmd.name, FILL_WIDTH) << cmd.desc << L'\n';
  }

  return stream.str();
}

} // namespace kb::cli

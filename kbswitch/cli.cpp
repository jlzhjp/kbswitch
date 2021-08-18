#include "cli.h"

#include <cassert>
#include <sstream>

#include "cliutil.h"
#include "kbdef.h"
#include "kbutil.h"

namespace kb::cli {

constexpr size_t FILL_WIDTH = 15;

using Iter = Command::ArgList::const_iterator;

std::vector<Command> commands({

    Command(L"list",
        L"list all installed keyboard layouts",
        [](Iter begin, Iter end) {
          if (begin != end) {
            return prompt_error(L"Too many arguments.");
          }

          auto map = get_klid_to_text_map();
          auto keyboards = get_user_klid_list();

          for (size_t i = 0; i != keyboards.size(); ++i) {
            writeln(i, L" => ", map[keyboards[i]]);
          }

          return 0;
        }),

    Command(L"get",
        L"get the index of current keyboard layout",
        [](Iter begin, Iter end) {
          if (begin != end) {
            return prompt_error(L"Too many arguments.");
          }
          auto keyboards = get_user_klid_list();
          std::wstring klid = get_current_klid();
          auto iter = std::find(keyboards.begin(), keyboards.end(), klid);
          if (iter == keyboards.end()) {
            return prompt_error(L"Unexpected KLID: ", klid);
          }
          writeln(std::distance(keyboards.begin(), iter));
          return 0;
        }),

    Command(L"set",
        L"switch to the given keyboard layout for the foreground window",
        [](Iter begin, Iter end) {
          if (begin == end) {
            return prompt_error(L"No <index> provided.");
          } else if (std::distance(begin, end) > 1) {
            return prompt_error(L"Too many arguments.");
          }

          int index = 0;
          try {
            index = std::stoi(*begin);
          } catch (const std::invalid_argument &) {
            return prompt_error(L"Invalid index:", *begin, L".");
          }

          auto keyboards = get_user_klid_list();

          if (index < 0 ||
              static_cast<std::size_t>(index) >= keyboards.size()) {
            prompt_error(L"Index out of range.");
            return writeln_error(
                L"The valid range is [ 0,", keyboards.size(), L").");
          }

          size_t uIndex = static_cast<size_t>(index);
          set_keyboard_layout(GetForegroundWindow(), keyboards[uIndex]);

          return 0;
        }),

    Command(L"cache",
        L"cache the current keyboard layout state of the foreground window",
        [](Iter begin, Iter end) {
          if (begin != end) {
            return prompt_error(L"Too many arguments");
          }
          cache_keyborad_layout_state();
          return 0;
        }),

    Command(L"restore",
        L"restore the current keyboard layout state of the foreground window",
        [](Iter begin, Iter end) {
          if (begin != end) {
            return prompt_error(L"Too many arguments.");
          }

          return restore_keyboard_layout_state() ?
                     0 :
                     prompt_error(L"No cache found.");
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

  KB_LOG(L"cmd/", L"execute:", ([&args]() {
    std::wstringstream res;
    for (auto iter = args.begin(); iter != std::prev(args.end()); ++iter) {
      res << *iter << L" ";
    }
    res << *std::prev(args.end());
    return res.str();
  })());

  if (argc <= 1) {
    prompt_error(L"No command provided.\n");
    writeln(generate_help_message());
    return -1;
  }

  std::wstring &commandText = args[1];
  auto itcmd = std::find_if(commands.begin(),
      commands.end(),
      [&commandText](Command &cmd) { return cmd.name == commandText; });

  if (itcmd == commands.end()) {
    return prompt_error(L"No such command:", commandText, L".");
  }

  return (*itcmd)(args.begin() + 2, args.end());
}

std::vector<std::wstring> convert_args(int argc, wchar_t **argv) {
  std::vector<std::wstring> res;
  for (int i = 0; i != argc; ++i) {
    res.push_back(argv[i]);
  }
  return res;
}

std::wstring generate_help_message() {
  std::wstringstream stream;
  stream << L"usage: kbswitch.exe <command> [<args>]\n\n";

  for (auto &cmd : commands) {
    stream << fill_right(cmd.name, FILL_WIDTH) << cmd.desc << L"\n";
  }

  return stream.str();
}

} // namespace kb::cli

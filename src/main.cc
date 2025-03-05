#include <iostream>
#include <print>

#include <argparse/argparse.hpp>
#include <spdlog/spdlog.h>

#include "command_executor.h"
#include "kbswitch/version.h"

int main(int argc, char* argv[]) {
#if (defined(NDEBUG))
  spdlog::set_level(spdlog::level::off);
#endif

  kbswitch::CommandExecutor command_executor;
  argparse::ArgumentParser program{"kbswitch", kbswitch::kVersion};

  auto& group = program.add_mutually_exclusive_group();

  group.add_argument("-l", "--list").nargs(0).flag().help("list available layouts");
  group.add_argument("--current").nargs(0).flag().help("get current keyboard layout");
  group.add_argument("-a", "--activate")
    .nargs(1)
    .metavar("KLID")
    .help("activate keyboard layout by KLID");
  group.add_argument("-c", "--cache").nargs(0).flag().help("cache current layout");
  group.add_argument("-r", "--restore").nargs(0).flag().help("restore cached layout");

  try {
    program.parse_args(argc, argv);

    if (program.is_used("--list")) {
      command_executor.query_keyboard_layouts();
    } else if (program.is_used("--current")) {
      command_executor.query_current_active_layout();
    } else if (program.is_used("--activate")) {
      std::string klid = program.get("--activate");
      command_executor.activate_keyboard_layout(klid);
    } else if (program.is_used("--cache")) {
      command_executor.cache_keyboard_layout();
    } else if (program.is_used("--restore")) {
      command_executor.restore_keyboard_layout();
    } else {
      // No arguments provided, show help
      std::cout << program << std::endl;
    }
  } catch (std::exception& e) {
    std::println(std::cerr, "{}", e.what());
    std::cerr << program << std::endl;
    return 1;
  }

  return 0;
}
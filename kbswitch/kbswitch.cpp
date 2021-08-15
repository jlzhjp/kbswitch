
#include <Windows.h>

#include "cli.h"

// input locale identifer = HKL (keyboard layout handle)

int wmain(int argc, wchar_t **argv) {
  return kb::cli::parse_args(argc, argv);
}

#include <ctime>

#include "cli.h"
#include "kbdef.h"

int wmain(int argc, wchar_t **argv) {
  KB_LOG(kb::dbg::get_time());
  return kb::cli::parse_args(argc, argv);
}

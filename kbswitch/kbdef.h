#pragma once

#define KB_VER (L"0.1.3")
#define KB_DEBUG_LOG_FILE (0)

#ifdef NDEBUG

#define KB_LOG(...) (void(0))

#else

#include "dbg.h"

#if KB_DEBUG_LOG_FILE

#define KB_LOG(...) (kb::dbg::_log_file(__VA_ARGS__))

#else

#define KB_LOG(...) (kb::dbg::_log(__VA_ARGS__))

#endif // KB_DEBUG_LOG_FILE

#endif // NDEBUG

#pragma once

#ifdef WIN32
# include <windows.h>
# define LOOG_INT64 signed __int64
# define LOOG_UINT64 unsigned __int64
#else  /* assume POSIX */
# include <stdint.h>
# define LOOG_INT64 int64_t
# define LOOG_UINT64 uint64_t
#endif

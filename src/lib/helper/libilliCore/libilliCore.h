#ifdef WIN32
# include <windows.h>
#else  /* assume POSIX */
# include <stdint.h>
#endif

#ifndef LOOG_INT64
# ifdef WIN32
#  define LOOG_INT64 signed __int64
# else
#  define LOOG_INT64 int64_t
# endif
#endif

#ifndef LOOG_UINT64
# ifdef WIN32
#  define LOOG_UINT64 unsigned __int64
# else  /* assume POSIX */
#  define LOOG_UINT64 uint64_t
# endif
#endif

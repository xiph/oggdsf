#pragma once


#ifdef WIN32
# ifdef LIBOOOGGCHEF_EXPORTS
#  define LIBOOOGGCHEF_API __declspec(dllexport)
# else
#  define LIBOOOGGCHEF_API __declspec(dllimport)
# endif
#else  /* assume POSIX */
# define LIBOOOGGCHEF_API
#endif

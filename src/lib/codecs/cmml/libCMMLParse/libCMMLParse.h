#pragma once


#ifdef WIN32
# ifdef LIBCMMLPARSE_EXPORTS
#  define LIBCMMLPARSE_API __declspec(dllexport)
# else
#  define LIBCMMLPARSE_API __declspec(dllimport)
# endif
#else
# define LIBCMMLPARSE_API
#endif


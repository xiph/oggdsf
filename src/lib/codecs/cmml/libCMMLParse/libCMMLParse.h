#pragma once


#ifdef WIN32
#define LIBCMMLPARSE_API
#ifndef CMMLPARSE_DLL
    #define LIBCMMLPARSE_API
#else
    # ifdef LIBCMMLPARSE_EXPORTS
        #  define LIBCMMLPARSE_API __declspec(dllexport)
    # else
        #  define LIBCMMLPARSE_API __declspec(dllimport)
    # endif
#endif
#else
# define LIBCMMLPARSE_API
#endif


// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LIBWINCMMLPARSE_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LIBWINCMMLPARSE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef LIBWINCMMLPARSE_EXPORTS
#define LIBWINCMMLPARSE_API __declspec(dllexport)
#else
#define LIBWINCMMLPARSE_API __declspec(dllimport)
#endif

// This class is exported from the libWinCMMLParse.dll
class LIBWINCMMLPARSE_API ClibWinCMMLParse {
public:
	ClibWinCMMLParse(void);
	// TODO: add your methods here.
};

extern LIBWINCMMLPARSE_API int nlibWinCMMLParse;

LIBWINCMMLPARSE_API int fnlibWinCMMLParse(void);

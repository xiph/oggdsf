// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LIBANXHELPER_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LIBANXHELPER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef LIBANXHELPER_EXPORTS
#define LIBANXHELPER_API __declspec(dllexport)
#else
#define LIBANXHELPER_API __declspec(dllimport)
#endif

// This class is exported from the libAnxHelper.dll
class LIBANXHELPER_API ClibAnxHelper {
public:
	ClibAnxHelper(void);
	// TODO: add your methods here.
};

extern LIBANXHELPER_API int nlibAnxHelper;

LIBANXHELPER_API int fnlibAnxHelper(void);

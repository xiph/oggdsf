// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LIBCDROM_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LIBCDROM_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef LIBCDROM_EXPORTS
#define LIBCDROM_API __declspec(dllexport)
#else
#define LIBCDROM_API __declspec(dllimport)
#endif

// This class is exported from the libCDROM.dll
class LIBCDROM_API ClibCDROM {
public:
	ClibCDROM(void);
	// TODO: add your methods here.
};

extern LIBCDROM_API int nlibCDROM;

LIBCDROM_API int fnlibCDROM(void);

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DSFCMMLRAWSOURCE_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DSFCMMLRAWSOURCE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef DSFCMMLRAWSOURCE_EXPORTS
#define DSFCMMLRAWSOURCE_API __declspec(dllexport)
#else
#define DSFCMMLRAWSOURCE_API __declspec(dllimport)
#endif

// This class is exported from the dsfCMMLRawSource.dll
class DSFCMMLRAWSOURCE_API CdsfCMMLRawSource {
public:
	CdsfCMMLRawSource(void);
	// TODO: add your methods here.
};

extern DSFCMMLRAWSOURCE_API int ndsfCMMLRawSource;

DSFCMMLRAWSOURCE_API int fndsfCMMLRawSource(void);

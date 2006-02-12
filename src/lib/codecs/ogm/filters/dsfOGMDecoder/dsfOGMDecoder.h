// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DSFOGMDECODER_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DSFOGMDECODER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef DSFOGMDECODER_EXPORTS
#define DSFOGMDECODER_API __declspec(dllexport)
#else
#define DSFOGMDECODER_API __declspec(dllimport)
#endif

// This class is exported from the dsfOGMDecoder.dll
class DSFOGMDECODER_API CdsfOGMDecoder {
public:
	CdsfOGMDecoder(void);
	// TODO: add your methods here.
};

extern DSFOGMDECODER_API int ndsfOGMDecoder;

DSFOGMDECODER_API int fndsfOGMDecoder(void);

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DSFABSTRACOGGAUDIODECODER_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DSFABSTRACOGGAUDIODECODER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef DSFABSTRACOGGAUDIODECODER_EXPORTS
#define DSFABSTRACOGGAUDIODECODER_API __declspec(dllexport)
#else
#define DSFABSTRACOGGAUDIODECODER_API __declspec(dllimport)
#endif

// This class is exported from the dsfAbstracOggAudioDecoder.dll
class DSFABSTRACOGGAUDIODECODER_API CdsfAbstracOggAudioDecoder {
public:
	CdsfAbstracOggAudioDecoder(void);
	// TODO: add your methods here.
};

extern DSFABSTRACOGGAUDIODECODER_API int ndsfAbstracOggAudioDecoder;

DSFABSTRACOGGAUDIODECODER_API int fndsfAbstracOggAudioDecoder(void);

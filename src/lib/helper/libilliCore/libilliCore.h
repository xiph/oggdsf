// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LIBILLICORE_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LIBILLICORE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef LIBILLICORE_EXPORTS
#define LIBILLICORE_API __declspec(dllexport)
#else
#define LIBILLICORE_API __declspec(dllimport)
#endif

//#include "iLE_Math.h"
//#include "iBE_Math.h"
//#include "StringHelper.h"
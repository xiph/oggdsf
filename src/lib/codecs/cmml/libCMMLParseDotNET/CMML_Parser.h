#pragma once

#pragma unmanaged
#include <string>
#include "libCMMLParse/CMMLParser.h"
using namespace std;

#pragma managed
//#include "libCMMLTagsDotNET/CMMLDoc.h"
using namespace illiminable::libCMMLTagsDotNET;
using namespace illiminable::libiWrapper;
using namespace System;

namespace illiminable {
namespace libCMMLParserDotNET {

	

//Original interface
//==============
	//bool parseClipTag(wstring inClipText, C_ClipTag* outClip);
	//bool parseHeadTag(wstring inHeadText, C_HeadTag* outHead);
	//bool parseCMMLRootTag(wstring inCMMLRootText, C_CMMLRootTag* outCMMLRoot);

	//bool parseDocFromFile(wstring inFilename, C_CMMLDoc* outCMMLDoc);

public __gc class CMML_Parser
{
public:
	CMML_Parser(void);
	~CMML_Parser(void);

	//bool parseClipTag(wstring inClipText, C_ClipTag* outClip);
	//bool parseHeadTag(wstring inHeadText, C_HeadTag* outHead);
	//bool parseCMMLRootTag(wstring inCMMLRootText, CMMLRootTag* outCMMLRoot);

	bool parseDocFromFile(String* inFileName, CMMLDoc* outCMMLDoc);
protected:
	CMMLParser* mCMMLParser;
};


}	//End libCMMLParserDotNNET
}	//End illiminable
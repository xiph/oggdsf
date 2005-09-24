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


// DLB. 21/9/2005. Allow parse error information to be sent back to caller
public __gc class CMMLError
{
public:
	CMMLError()
	{
		mCMMLError = new C_CMMLError();
	}

	void SetLineNumber(int lLineNumber)
	{
		mCMMLError->SetLineNumber(lLineNumber);
	}

	int GetLineNumber()
	{
		return mCMMLError->GetLineNumber();
	}

	C_CMMLError* getMe()
	{
		return (C_CMMLError*)mCMMLError;
	}


private:
	C_CMMLError* mCMMLError;
};


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

	// DLB. 20/9/2005. Parse the DOC using an already read-in file
	bool parseDoc(String* inBuffer, CMMLDoc* outCMMLDoc, CMMLError* outCMMLError);

protected:
	CMMLParser* mCMMLParser;
};


}	//End libCMMLParserDotNNET
}	//End illiminable
#include "StdAfx.h"
#include ".\cmml_parser.h"
#using <mscorlib.dll>


namespace illiminable {
namespace libCMMLParserDotNET {
CMML_Parser::CMML_Parser(void)
{
}

CMML_Parser::~CMML_Parser(void)
{
}

bool CMML_Parser::parseDocFromFile(String* inFileName, CMMLDoc* outCMMLDoc) 
{
	wchar_t* locWS = Wrappers::netStrToWStr(inFileName);
	wstring locFileName = locWS;

	bool retVal = mCMMLParser->parseDocFromFile(locFileName, outCMMLDoc->getMe());

	mCMMLParser = new CMMLParser();
	
	Wrappers::releaseWStr(locWS);
	
	return retVal;
}

}	//End libCMMLParserDotNNET
}	//End illiminable


#pragma once
#include <string>
#include "libCMMLTags.h"
#import <msxml3.dll> raw_interfaces_only
using namespace MSXML2;
#include <msxml.h>
using namespace std;
class LIBWINCMMLPARSE_API CMMLParser
{
public:
	CMMLParser(void);
	~CMMLParser(void);

	bool setupXMLHandles(wstring inText, MSXML2::IXMLDOMDocument** outDoc);
	//C_CMMLTag* genericParseTag(string inCMMLText);
	//bool parseCMMLDoc(string inCMMLDocText, C_CMMLDoc* outDoc);
	bool parseClipTag(wstring inClipText, C_ClipTag* outClip);
	bool parseHeadTag(wstring inHeadText, C_HeadTag* outHead);

	bool parseHeadTag(MSXML2::IXMLDOMNode* inHeadNode, C_HeadTag* outHead);
	bool parseAnchorTag(MSXML2::IXMLDOMNode* inAnchorNode, C_AnchorTag* outAnchor);
	bool parseStreamTag(MSXML2::IXMLDOMNode* inStreamNode, C_StreamTag* outStream);
	bool parseImportTag(MSXML2::IXMLDOMNode* inImportNode, C_ImportTag* outImport);
	bool parseTitleTag(MSXML2::IXMLDOMNode* inTitleNode, C_TitleTag* outTitle);
	bool parseClipTag(MSXML2::IXMLDOMNode* inClipNode, C_ClipTag* outClip);
	bool parseImageTag(MSXML2::IXMLDOMNode* inImageNode, C_ImageTag* outImage);
	bool parseMetaTag(MSXML2::IXMLDOMNode* inMetaNode, C_MetaTag* outMeta);
	bool parseDescTag(MSXML2::IXMLDOMNode* inDescNode, C_DescTag* outDesc);



protected:
	wstring getNamedAttribValue(wstring inAttribName, MSXML2::IXMLDOMNamedNodeMap* inAttribMap);
	MSXML2::IXMLDOMNode* getNamedNode(wstring inXPath, MSXML2::IXMLDOMDocument* inDoc);
	string toNarrowStr(wstring inString);
	//void createTagMap();
	//unsigned long hashFunction(string inTagName);


	//typedef map<unsigned long, string> tTagMap;
	//tTagMap mTagMap;
	//tTagMap::value_type mTagValue;

};

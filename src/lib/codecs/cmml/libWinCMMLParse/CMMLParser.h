//===========================================================================
//Copyright (C) 2004 Zentaro Kavanagh
//
//Copyright (C) 2004 Commonwealth Scientific and Industrial Research
//   Organisation (CSIRO) Australia
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions
//are met:
//
//- Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//
//- Redistributions in binary form must reproduce the above copyright
//  notice, this list of conditions and the following disclaimer in the
//  documentation and/or other materials provided with the distribution.
//
//- Neither the name of Zentaro Kavanagh nor the names of contributors 
//  may be used to endorse or promote products derived from this software 
//  without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
//PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE ORGANISATION OR
//CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//===========================================================================

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
	//string toNarrowStr(wstring inString);
	//void createTagMap();
	//unsigned long hashFunction(string inTagName);


	//typedef map<unsigned long, string> tTagMap;
	//tTagMap mTagMap;
	//tTagMap::value_type mTagValue;

};

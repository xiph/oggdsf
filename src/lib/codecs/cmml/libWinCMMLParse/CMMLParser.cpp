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
#include "StdAfx.h"
#include "cmmlparser.h"

CMMLParser::CMMLParser(void)
{
	HRESULT hr;
	hr = CoInitialize(NULL); 
}

CMMLParser::~CMMLParser(void)
{
}




bool CMMLParser::setupXMLHandles(wstring inText, MSXML2::IXMLDOMDocument** outDoc) {

	HRESULT locHR = S_FALSE;
	// Check the return value, hr...
	locHR = CoCreateInstance(__uuidof(DOMDocument30), NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument, (void**)outDoc);
	// Check the return value, hr...
	
	// Check the return value.
	
	BSTR locClipStr = SysAllocString(inText.c_str());
	VARIANT_BOOL locBool;
	locHR = (*outDoc)->loadXML(locClipStr, &locBool);


	//Should free string ??
	SysFreeString(locClipStr);
	if (locHR == S_OK) {
		return true;
	} else {
		return false;
	}
}



MSXML2::IXMLDOMNode* CMMLParser::getNamedNode(wstring inXPath, MSXML2::IXMLDOMDocument* inDoc) {
	BSTR locQuery = SysAllocString(inXPath.c_str());
	HRESULT locHR = S_FALSE;
	MSXML2::IXMLDOMNode* retNode = NULL;
	
	locHR = inDoc->selectSingleNode(locQuery, &retNode);
	

    SysFreeString(locQuery);
	return retNode;
}

wstring CMMLParser::getNamedAttribValue(wstring inAttribName, MSXML2::IXMLDOMNamedNodeMap* inAttribMap) {
	BSTR locAttribName = SysAllocString(inAttribName.c_str());
	MSXML2::IXMLDOMNode* locAttribNode = NULL;
	wstring retStr = L"";
	HRESULT locHR = S_FALSE;
	locHR = inAttribMap->getNamedItem(locAttribName, &locAttribNode);
	BSTR locBStr = NULL;
	
	if (locHR == S_OK) {
		locHR =  locAttribNode->get_text(&locBStr);
		retStr = locBStr;
	} else {
		//Not found... what to do !
	}
	
	//Cleanup
	if (locAttribNode != NULL) locAttribNode->Release();
	SysFreeString(locBStr);
	SysFreeString(locAttribName);

	return retStr;
}

bool CMMLParser::parseClipTag(MSXML2::IXMLDOMNode* inClipNode, C_ClipTag* outClip) {
	MSXML2::IXMLDOMNamedNodeMap*	locAttribMap		= NULL;
	MSXML2::IXMLDOMNode*			locNode				= NULL;
	MSXML2::IXMLDOMNodeList*		locChildNodes		= NULL;
	HRESULT							locHR				= S_FALSE;
	wstring							locNodeName;
	BSTR							locBStr				= NULL;
	long							locNumNodes			= 0;

	//---------------Attributes-----------------
	//Get the attributes
	locHR = inClipNode->get_attributes(&locAttribMap);
	
	//Other attributes are missing.
	outClip->setId(toNarrowStr(getNamedAttribValue(L"id", locAttribMap)));
	outClip->setStart(toNarrowStr(getNamedAttribValue(L"start", locAttribMap)));
	outClip->setStart(toNarrowStr(getNamedAttribValue(L"end", locAttribMap)));
	//------------------------------------------
	
	//--------------Child Nodes-----------------
	locHR = inClipNode->get_childNodes(&locChildNodes);
	locHR = locChildNodes->get_length(&locNumNodes);
	
	for (int i = 0; i < locNumNodes; i++) {
		
		locHR = locChildNodes->get_item(i, &locNode);
		locHR = locNode->get_nodeName(&locBStr);
		locNodeName = locBStr;
		
		if (locNodeName == L"a") {

			C_AnchorTag* locAnchor = new C_AnchorTag;
			parseAnchorTag(locNode, locAnchor);
			outClip->setAnchor(locAnchor);

		} else if(locNodeName == L"desc") {

			C_DescTag* locDesc = new C_DescTag;
			parseDescTag(locNode, locDesc);
			outClip->setDesc(locDesc);
			
		} else if (locNodeName == L"img") {

			C_ImageTag* locImage = new C_ImageTag;
			parseImageTag(locNode, locImage);
			outClip->setImage(locImage);

		} else if (locNodeName == L"meta") {

			C_MetaTag* locMeta = new C_MetaTag;
			parseMetaTag(locNode, locMeta);
			outClip->metaList()->addTag(locMeta);

		} else {
			//Dunno !
		}
		

	}

	SysFreeString(locBStr);
	if (locAttribMap != NULL)					locAttribMap->Release();
	if (locNode != NULL)						locNode->Release();
	if (locChildNodes != NULL)					locChildNodes->Release();
	return true;
}

bool CMMLParser::parseHeadTag(MSXML2::IXMLDOMNode* inHeadNode, C_HeadTag* outHead) {
	MSXML2::IXMLDOMNamedNodeMap* locAttribMap = NULL;
	MSXML2::IXMLDOMNodeList* locChildNodes = NULL;
	MSXML2::IXMLDOMNode* locNode = NULL;
	HRESULT locHR = S_FALSE;
	long locNumNodes = 0;
	BSTR locBStr = NULL;
	wstring locNodeName;
	//---------------Attributes-----------------
	//No attributes for now
	//------------------------------------------

	//--------------Child Nodes-----------------
	locHR = inHeadNode->get_childNodes(&locChildNodes);
	locHR = locChildNodes->get_length(&locNumNodes);
	for (int i = 0; i < locNumNodes; i++) {
		locHR = locChildNodes->get_item(i, &locNode);
		locHR = locNode->get_nodeName(&locBStr);
		locNodeName = locBStr;
		if (locNodeName == L"title") {
			C_TitleTag* locTitle = new C_TitleTag;
			parseTitleTag(locNode, locTitle);
			outHead->setTitle(locTitle);

		} else if(locNodeName == L"meta") {
			C_MetaTag* locMeta = new C_MetaTag;
			parseMetaTag(locNode, locMeta);
			outHead->metaList()->addTag(locMeta);
			
		} else {
			//Dunno !
		}
	}

	SysFreeString(locBStr);
	if (locAttribMap != NULL)					locAttribMap->Release();
	if (locNode != NULL)						locNode->Release();
	if (locChildNodes != NULL)					locChildNodes->Release();
	return true;
}

bool CMMLParser::parseStreamTag(MSXML2::IXMLDOMNode* inStreamNode, C_StreamTag* outStream) {
	MSXML2::IXMLDOMNamedNodeMap* locAttribMap = NULL;
	MSXML2::IXMLDOMNode* locNode = NULL;
	MSXML2::IXMLDOMNodeList* locChildNodes = NULL;
	HRESULT locHR = S_FALSE;
	long locNumNodes = 0;
	BSTR locBStr = NULL;
	wstring locNodeName;
	//---------------Attributes-----------------
	//Get the attributes
	locHR = inStreamNode->get_attributes(&locAttribMap);
	
	//Other attributes are missing.
	outStream->setTimebase(toNarrowStr(getNamedAttribValue(L"timebase", locAttribMap)));
	//------------------------------------------

	//--------------Child Nodes-----------------
	locHR = inStreamNode->get_childNodes(&locChildNodes);
	locHR = locChildNodes->get_length(&locNumNodes);
	for (int i = 0; i < locNumNodes; i++) {
		locHR = locChildNodes->get_item(i, &locNode);
		locHR = locNode->get_nodeName(&locBStr);
		locNodeName = locBStr;
		if(locNodeName == L"import") {
			C_ImportTag* locImport = new C_ImportTag;
			parseImportTag(locNode, locImport);
			outStream->importList()->addTag(locImport);
			
		} else {
			//Dunno !
		}
	}

	SysFreeString(locBStr);
	if (locAttribMap != NULL)					locAttribMap->Release();
	if (locNode != NULL)						locNode->Release();
	if (locChildNodes != NULL)					locChildNodes->Release();
	return true;
}

bool CMMLParser::parseImportTag(MSXML2::IXMLDOMNode* inImportNode, C_ImportTag* outImport) {
	MSXML2::IXMLDOMNamedNodeMap* locAttribMap = NULL;
	//MSXML2::IXMLDOMNode* locImportNode = NULL;
	HRESULT locHR = S_FALSE;

	//---------------Attributes-----------------
	locHR = inImportNode->get_attributes(&locAttribMap);
	outImport->setId(toNarrowStr(getNamedAttribValue(L"id", locAttribMap)));
	outImport->setContentType(toNarrowStr(getNamedAttribValue(L"contenttype", locAttribMap)));
	outImport->setSrc(toNarrowStr(getNamedAttribValue(L"src", locAttribMap)));
	outImport->setStart(toNarrowStr(getNamedAttribValue(L"start", locAttribMap)));
	outImport->setEnd(toNarrowStr(getNamedAttribValue(L"end", locAttribMap)));
	outImport->setTitle(toNarrowStr(getNamedAttribValue(L"title", locAttribMap)));
	outImport->setGranuleRate(toNarrowStr(getNamedAttribValue(L"granulerate", locAttribMap)));
	//------------------------------------------
	return true;
}

bool CMMLParser::parseImageTag(MSXML2::IXMLDOMNode* inImageNode, C_ImageTag* outImage) {
	MSXML2::IXMLDOMNamedNodeMap* locAttribMap = NULL;
	MSXML2::IXMLDOMNode* locImageNode = NULL;
	HRESULT locHR = S_FALSE;

	//---------------Attributes-----------------
	locHR = inImageNode->get_attributes(&locAttribMap);
	outImage->setSrc(toNarrowStr(getNamedAttribValue(L"src", locAttribMap)));
	return true;
}
/*
bool CMMLParser::parseMetaTag(MSXML2::IXMLDOMNode* inMetaNode, C_MetaTag* outMeta) {
	MSXML2::IXMLDOMNamedNodeMap* locAttribMap = NULL;
	MSXML2::IXMLDOMNode* locMetaNode = NULL;
	HRESULT locHR = S_FALSE;

	//---------------Attributes-----------------
	locHR = inMetaNode->get_attributes(&locAttribMap);

}
*/

bool CMMLParser::parseMetaTag(MSXML2::IXMLDOMNode* inMetaNode, C_MetaTag* outMeta) {
	MSXML2::IXMLDOMNamedNodeMap* locAttribMap = NULL;
	MSXML2::IXMLDOMNode* locMetaNode = NULL;
	HRESULT locHR = S_FALSE;

	//---------------Attributes-----------------
	locHR = inMetaNode->get_attributes(&locAttribMap);
	outMeta->setName(toNarrowStr(getNamedAttribValue(L"name", locAttribMap)));
	outMeta->setContent(toNarrowStr(getNamedAttribValue(L"content", locAttribMap)));
	outMeta->setScheme(toNarrowStr(getNamedAttribValue(L"scheme", locAttribMap)));
	//------------------------------------------
	return true;
}

string CMMLParser::toNarrowStr(wstring inString) {
	string retVal;


	for (std::wstring::const_iterator i = inString.begin(); i != inString.end(); i++) {
		retVal.append(1, *i);
	}
	
	return retVal;
}

bool CMMLParser::parseAnchorTag(MSXML2::IXMLDOMNode* inAnchorNode, C_AnchorTag* outAnchor) {
	MSXML2::IXMLDOMNamedNodeMap*		locAttribMap	= NULL;
	MSXML2::IXMLDOMNode*				locAnchorNode	= NULL;
	HRESULT								locHR			= S_FALSE;
	BSTR								locBStr			= NULL;
	wstring								locAnchorText;

	//---------------Attributes-----------------
	locHR = inAnchorNode->get_attributes(&locAttribMap);
	outAnchor->setHref(toNarrowStr(getNamedAttribValue(L"href", locAttribMap)));
	//------------------------------------------
	//Anchor text

	inAnchorNode->get_text(&locBStr);
	locAnchorText = locBStr;
	outAnchor->setText(toNarrowStr(locAnchorText));

	//Cleanup
	SysFreeString(locBStr);
    if (locAttribMap != NULL)					locAttribMap->Release();
	if (locAnchorNode != NULL)					locAnchorNode->Release();
	return true;
}

bool CMMLParser::parseTitleTag(MSXML2::IXMLDOMNode* inTitleNode, C_TitleTag* outTitle) {
	MSXML2::IXMLDOMNamedNodeMap* locAttribMap = NULL;
	BSTR locBStr = NULL;
	wstring locTitleText;
	HRESULT locHR = S_FALSE;

	//---------------Attributes-----------------
	//locHR = inTitleNode->get_attributes(&locAttribMap);
	//None for now.
	//------------------------------------------
	inTitleNode->get_text(&locBStr);
	locTitleText = locBStr;
	outTitle->setText(toNarrowStr(locTitleText));

	SysFreeString(locBStr);
	if (locAttribMap != NULL)					locAttribMap->Release();
	return true;

}

bool CMMLParser::parseDescTag(MSXML2::IXMLDOMNode* inDescNode, C_DescTag* outDesc) {
	MSXML2::IXMLDOMNamedNodeMap* locAttribMap = NULL;
	//MSXML2::IXMLDOMNode* locDescNode = NULL;
	BSTR locBStr = NULL;
	wstring locDescText;
	HRESULT locHR = S_FALSE;

	//---------------Attributes-----------------
	//locHR = inDescNode->get_attributes(&locAttribMap);
	//None for now.
	//------------------------------------------
	inDescNode->get_text(&locBStr);
	locDescText = locBStr;
	outDesc->setText(toNarrowStr(locDescText));

	SysFreeString(locBStr);
	if (locAttribMap != NULL)					locAttribMap->Release();
	return true;

}
bool CMMLParser::parseClipTag(wstring inClipText, C_ClipTag* outClip) {
	HRESULT locHR = S_FALSE;
	MSXML2::IXMLDOMDocument* locXMLClipFrag = NULL;
	MSXML2::IXMLDOMNode* locClipNode  = NULL;
		
	bool locSetupOK = setupXMLHandles(inClipText, &locXMLClipFrag);
	//Validate
	if (locSetupOK) {
		//locXMLClipFrag->selectSingleNode(locBStr, &locClipNode);
		locClipNode = getNamedNode(L"clip", locXMLClipFrag);
		//Check return

		//Now we have a node representing the clip tag and it's children.
	
		bool retVal = parseClipTag(locClipNode, outClip);

		if (locXMLClipFrag != NULL)					locXMLClipFrag->Release();
		if (locClipNode != NULL)					locClipNode->Release();
	
		return retVal;
	} else {
		return false;
	}
}

bool CMMLParser::parseHeadTag(wstring inHeadText, C_HeadTag* outHead) {
	HRESULT locHR = S_FALSE;
	MSXML2::IXMLDOMDocument* locXMLHeadFrag = NULL;
	MSXML2::IXMLDOMNode* locHeadNode  = NULL;
		
	bool retVal = setupXMLHandles(inHeadText, &locXMLHeadFrag);

	if (retVal) {
		locHeadNode = getNamedNode(L"head", locXMLHeadFrag);
		if (locHeadNode != NULL) {

			//Now we have a node representing the clip tag and it's children.
			retVal = parseHeadTag(locHeadNode, outHead);
		}
	}

	if (locXMLHeadFrag != NULL)					locXMLHeadFrag->Release();
	if (locHeadNode != NULL)					locHeadNode->Release();

	return retVal;
}
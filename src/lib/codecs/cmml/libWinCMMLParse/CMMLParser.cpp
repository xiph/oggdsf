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
	if (locHR != S_OK) {
		return false;
	}
	
	BSTR locClipStr = SysAllocString(inText.c_str());
	VARIANT_BOOL locBool;
	locHR = (*outDoc)->loadXML(locClipStr, &locBool);


	SysFreeString(locClipStr);

	if (locHR == S_OK) {
		return true;
	} else {
		return false;
	}
}

bool CMMLParser::parseDocFromFile(wstring inFilename, C_CMMLDoc* outCMMLDoc) {

	MSXML2::IXMLDOMDocument* locDoc		=	NULL;
	MSXML2::IXMLDOMNode*		locNode =	NULL;
	BSTR locQuery	= SysAllocString(L"cmml");;
	VARIANT locFilename;

	VariantInit(&locFilename);
	V_BSTR(&locFilename) = SysAllocString(inFilename.c_str());
	V_VT(&locFilename) = VT_BSTR;
	//locFilename.vt = VT_BSTR;
	//locFilename.bstrVal = SysAllocString(inFilename.c_str());


	bool retVal = true;
	HRESULT locHR = S_FALSE;

	
	locHR = CoCreateInstance(__uuidof(DOMDocument30), NULL, CLSCTX_INPROC_SERVER, IID_IXMLDOMDocument, (void**)&locDoc);
	if (locHR != S_OK) {
		retVal = false;
	} else {
	
		
		VARIANT_BOOL locBool = VARIANT_FALSE;
		locDoc->put_validateOnParse(locBool);
		
		locHR = locDoc->load(locFilename, &locBool);
		MSXML2::IXMLDOMParseError *locParseError = NULL;
		locHR = locDoc->get_parseError(&locParseError);
		long locEC = 0;
		locParseError->get_errorCode(&locEC);
		BSTR locStr;
		locParseError->get_reason(&locStr);

		

		//--------------Child Nodes-----------------
		locHR = locDoc->selectSingleNode(locQuery, &locNode);
		if (locHR != S_OK) {
			retVal = false;
		} else {

			C_CMMLRootTag* locRootTag = new C_CMMLRootTag;

			retVal = parseCMMLRootTag(locNode, locRootTag);
			if (retVal) {
				outCMMLDoc->setRoot(locRootTag);	
			} else {
				delete locRootTag;
			}
		}
	}

	//SysFreeString(locFilename.bstrVal);
	SysFreeString(locQuery);
	if (&locFilename) VariantClear(&locFilename);
	if (locDoc != NULL)			locDoc->Release();
	if (locNode != NULL)		locNode->Release();

	return retVal;
}



MSXML2::IXMLDOMNode* CMMLParser::getNamedNode(wstring inXPath, MSXML2::IXMLDOMDocument* inDoc) {
	BSTR locQuery = SysAllocString(inXPath.c_str());
	HRESULT locHR = S_FALSE;
	MSXML2::IXMLDOMNode* retNode = NULL;
	
	locHR = inDoc->selectSingleNode(locQuery, &retNode);
	
    SysFreeString(locQuery);

	//If the select fails... will return NULL
	return retNode;
}

wstring CMMLParser::getNamedAttribValue(wstring inAttribName, MSXML2::IXMLDOMNamedNodeMap* inAttribMap) {
	BSTR					locAttribName	= SysAllocString(inAttribName.c_str());
	BSTR					locBStr			= NULL;
	MSXML2::IXMLDOMNode*	locAttribNode	= NULL;
	HRESULT					locHR			= S_FALSE;
	wstring					retStr			= L"";
	
	locHR = inAttribMap->getNamedItem(locAttribName, &locAttribNode);

	if (locHR == S_OK) {
		locHR =  locAttribNode->get_text(&locBStr);
		retStr = locBStr;
	}
	
	//Cleanup
	if (locAttribNode != NULL) locAttribNode->Release();
	SysFreeString(locBStr);
	SysFreeString(locAttribName);

	//Will return "" on error
	return retStr;
}

bool CMMLParser::parseClipTag(MSXML2::IXMLDOMNode* inClipNode, C_ClipTag* outClip) {

	//Required start is not checked for, because CMML in annodex doesn't have one. May need to be
	// cvalidated elsewhere to be used in CMML documents
	MSXML2::IXMLDOMNamedNodeMap*	locAttribMap		= NULL;
	MSXML2::IXMLDOMNode*			locNode				= NULL;
	MSXML2::IXMLDOMNodeList*		locChildNodes		= NULL;
	BSTR							locBStr				= NULL;
	HRESULT							locHR				= S_FALSE;
	
	wstring							locNodeName;
	long							locNumNodes			= 0;
	
	unsigned long					locNum_a			= 0;
	unsigned long					locNum_desc			= 0;
	unsigned long					locNum_img			= 0;
	unsigned long					locNum_meta			= 0;
	unsigned long					locNumUnknown		= 0;

	//---------------Attributes-----------------
	//Get the attributes
	locHR = inClipNode->get_attributes(&locAttribMap);
	
	outClip->setId(getNamedAttribValue(L"id", locAttribMap));
	outClip->setLang(getNamedAttribValue(L"lang", locAttribMap));
	outClip->setDirn(getNamedAttribValue(L"dir", locAttribMap));
	outClip->setTrack(getNamedAttribValue(L"track", locAttribMap));
	outClip->setStart(getNamedAttribValue(L"start", locAttribMap));
	outClip->setEnd(getNamedAttribValue(L"end", locAttribMap));
	//------------------------------------------
	//--------------Child Nodes-----------------
	locHR = inClipNode->get_childNodes(&locChildNodes);
	locHR = locChildNodes->get_length(&locNumNodes);
	
	for (int i = 0; i < locNumNodes; i++) {
		
		locHR = locChildNodes->get_item(i, &locNode);
		locHR = locNode->get_nodeName(&locBStr);
		locNodeName = locBStr;
		
		if (locNodeName == L"a") {
			
			//ZERO OR ONE a elements allowed
			C_AnchorTag* locAnchor = new C_AnchorTag;

			if (parseAnchorTag(locNode, locAnchor)) {
				outClip->setAnchor(locAnchor);
				locNum_a++;
			} else {
				delete locAnchor;
				outClip->setAnchor(NULL);
			}

		} else if(locNodeName == L"desc") {
			
			//ZERO OR ONE desc elements allowed
			C_DescTag* locDesc = new C_DescTag;
			if (parseDescTag(locNode, locDesc)) {
				outClip->setDesc(locDesc);
				locNum_desc++;
			} else {
				delete locDesc;
				outClip->setDesc(NULL);
			}
			
		} else if (locNodeName == L"img") {
			
			//ZERO OR ONE img elements allowed
			C_ImageTag* locImage = new C_ImageTag;
			if (parseImageTag(locNode, locImage)) {
				outClip->setImage(locImage);
				locNum_img++;
			} else {
				delete locImage;
				outClip->setImage(locImage);
			}

		} else if (locNodeName == L"meta") {
			
			//ZERO OR *MORE* meta  elements allowed
			C_MetaTag* locMeta = new C_MetaTag;
			if (parseMetaTag(locNode, locMeta)) {
				outClip->metaList()->addTag(locMeta);
				locNum_meta++;
			} else {
				delete locMeta;
			}

		} else {
			
			//TODO::: How to handle ??? For now just ignore tags we don't know.
			locNumUnknown++;
		}
	}

	bool retVal	=		(locNum_a <= 1)
					&&	(locNum_img <= 1)
					&&	(locNum_desc <= 1);

	SysFreeString(locBStr);
	if (locAttribMap != NULL)					locAttribMap->Release();
	if (locNode != NULL)						locNode->Release();
	if (locChildNodes != NULL)					locChildNodes->Release();
	return retVal;
}

bool CMMLParser::parseClipTag(wstring inClipText, C_ClipTag* outClip) {
	HRESULT						locHR			= S_FALSE;
	MSXML2::IXMLDOMDocument*	locXMLClipFrag	= NULL;
	MSXML2::IXMLDOMNode*		locClipNode		= NULL;
		
	bool retVal = setupXMLHandles(inClipText, &locXMLClipFrag);

	if (retVal) {
		locClipNode = getNamedNode(L"clip", locXMLClipFrag);
		
		if (locClipNode != NULL) {
			//Now we have a node representing the clip tag and it's children.
			retVal = parseClipTag(locClipNode, outClip);
		} else {
			retVal = false;
		}
	}

	if (locXMLClipFrag != NULL)					locXMLClipFrag->Release();
	if (locClipNode != NULL)					locClipNode->Release();

	return retVal;
}

bool CMMLParser::parseCMMLRootTag(MSXML2::IXMLDOMNode* inCMMLRootNode, C_CMMLRootTag* outCMMLRoot) {
	MSXML2::IXMLDOMNamedNodeMap*	locAttribMap	= NULL;
	MSXML2::IXMLDOMNodeList*		locChildNodes	= NULL;
	MSXML2::IXMLDOMNode*			locNode			= NULL;
	HRESULT							locHR			= S_FALSE;
	long							locNumNodes		= 0;
	BSTR							locBStr			= NULL;
	wstring							locNodeName		= L"";


	unsigned long					locNum_clip		= 0;
	unsigned long					locNum_head		= 0;
	unsigned long					locNum_stream	= 0;
	unsigned long					locNumUnknown	= 0;

	bool retVal = true;
	
	//---------------Attributes-----------------
	locHR = inCMMLRootNode->get_attributes(&locAttribMap);

	outCMMLRoot->setId(getNamedAttribValue(L"id", locAttribMap));
	outCMMLRoot->setLang(getNamedAttribValue(L"lang", locAttribMap));
	outCMMLRoot->setDirn(getNamedAttribValue(L"dir", locAttribMap));
	
	//------------------------------------------
	//--------------Child Nodes-----------------
	locHR = inCMMLRootNode->get_childNodes(&locChildNodes);
	locHR = locChildNodes->get_length(&locNumNodes);

	for (int i = 0; i < locNumNodes; i++) {
		locHR = locChildNodes->get_item(i, &locNode);
		locHR = locNode->get_nodeName(&locBStr);
		//TODO::: Needs checks ??

		locNodeName = locBStr;
		if (locNodeName == L"stream") {
			//If it exists it must be first
			if (		(locNum_stream == 0)
					&&	(locNum_clip == 0)
					&&	(locNum_head == 0)) {
				//OPTIONALLY ONE stream tag
				C_StreamTag* locStream = new C_StreamTag;
			
				if (parseStreamTag(locNode, locStream)) {
					outCMMLRoot->setStream(locStream);
					locNum_stream++;
				} else {
					delete locStream;
					outCMMLRoot->setStream(NULL);
				}
			} else {
				retVal = false;
			}


		} else if (locNodeName == L"head") {
			
			//MUST HAVE ONE head tag
			if (		(locNum_stream <= 1)
					&&	(locNum_clip == 0)
					&&	(locNum_head == 0)) {
			
				C_HeadTag* locHead = new C_HeadTag;
			
				if (parseHeadTag(locNode, locHead)) {
					outCMMLRoot->setHead(locHead);
					locNum_head++;
				} else {
					delete locHead;	
					retVal = false;
				}
			} else {
				retVal = false;

			}
		} else if (locNodeName == L"clip") {

			//ZERO OR MORE clip tags
			if (		(locNum_stream <= 1)
					&&	(locNum_head == 1)) {

				C_ClipTag* locClip = new C_ClipTag;
			
				if(parseClipTag(locNode, locClip)) {
					outCMMLRoot->clipList()->addTag(locClip);
					locNum_clip++;
				} else {
					delete locClip;
				}
			} else { 
				retVal = false;
			}
			
		} else {
			locNumUnknown++;
		}
	}

	retVal		=		(retVal)
						&&	(locNum_stream <= 1)
						&&	(locNum_head == 1);
						


	SysFreeString(locBStr);
	if (locAttribMap != NULL)					locAttribMap->Release();
	if (locNode != NULL)						locNode->Release();
	if (locChildNodes != NULL)					locChildNodes->Release();
	return retVal;
}


bool CMMLParser::parseCMMLRootTag(wstring inCMMLRootText, C_CMMLRootTag* outCMMLRoot) {
	HRESULT						locHR				= S_FALSE;
	MSXML2::IXMLDOMDocument*	locXMLCMMLRootFrag	= NULL;
	MSXML2::IXMLDOMNode*		locCMMLRootNode			= NULL;
		
	bool retVal = setupXMLHandles(inCMMLRootText, &locXMLCMMLRootFrag);

	if (retVal) {
		locCMMLRootNode = getNamedNode(L"cmml", locXMLCMMLRootFrag);
		
		if (locCMMLRootNode != NULL) {
			
			retVal = parseCMMLRootTag(locCMMLRootNode, outCMMLRoot);
		} else {
			retVal = false;
		}
	}

	if (locXMLCMMLRootFrag != NULL)					locXMLCMMLRootFrag->Release();
	if (locCMMLRootNode != NULL)						locCMMLRootNode->Release();

	return retVal;
}
bool CMMLParser::parseHeadTag(MSXML2::IXMLDOMNode* inHeadNode, C_HeadTag* outHead) {
	MSXML2::IXMLDOMNamedNodeMap*	locAttribMap	= NULL;
	MSXML2::IXMLDOMNodeList*		locChildNodes	= NULL;
	MSXML2::IXMLDOMNode*			locNode			= NULL;
	HRESULT							locHR			= S_FALSE;
	long							locNumNodes		= 0;
	BSTR							locBStr			= NULL;
	wstring							locNodeName;
								
	unsigned long					locNum_title	= 0;
	unsigned long					locNum_meta		= 0;
	unsigned long					locNum_base		= 0;
	unsigned long					locNumUnknown	= 0;

	
	//---------------Attributes-----------------
	locHR = inHeadNode->get_attributes(&locAttribMap);

	outHead->setId(getNamedAttribValue(L"id", locAttribMap));
	outHead->setLang(getNamedAttribValue(L"lang", locAttribMap));
	outHead->setDirn(getNamedAttribValue(L"dir", locAttribMap));
	outHead->setProfile(getNamedAttribValue(L"profile", locAttribMap));
	//------------------------------------------

	//--------------Child Nodes-----------------
	locHR = inHeadNode->get_childNodes(&locChildNodes);
	locHR = locChildNodes->get_length(&locNumNodes);
	for (int i = 0; i < locNumNodes; i++) {
		locHR = locChildNodes->get_item(i, &locNode);
		locHR = locNode->get_nodeName(&locBStr);
		//TODO::: Needs checks ??

		locNodeName = locBStr;
		if (locNodeName == L"title") {
		
			//Must contain ONE title tag
			C_TitleTag* locTitle = new C_TitleTag;
			
			if (parseTitleTag(locNode, locTitle)) {
				outHead->setTitle(locTitle);
				locNum_title++;
			} else {
				delete locTitle;
				outHead->setTitle(NULL);
			}


		} else if (locNodeName == L"meta") {
			
			//Can contain ANY AMOUNT of meta tags
			C_MetaTag* locMeta = new C_MetaTag;
			
			if (parseMetaTag(locNode, locMeta)) {
				outHead->metaList()->addTag(locMeta);
				locNum_meta++;
			} else {
				delete locMeta;				
			}
			
		} else if (locNodeName == L"base") {

			//OPTIONALLY ONE base tag.
			C_BaseTag* locBase = new C_BaseTag;
			
			if(parseBaseTag(locNode, locBase)) {
                outHead->setBase(locBase);
				locNum_base++;
			} else {
				delete locBase;
				outHead->setBase(NULL);
			}
			
		} else {
			locNumUnknown++;
		}
	}

	bool retVal		=		(locNum_title == 1)
						&&	(locNum_base <= 1);


	SysFreeString(locBStr);
	if (locAttribMap != NULL)					locAttribMap->Release();
	if (locNode != NULL)						locNode->Release();
	if (locChildNodes != NULL)					locChildNodes->Release();
	return retVal;
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
		} else {
			retVal = false;
		}
	}

	if (locXMLHeadFrag != NULL)					locXMLHeadFrag->Release();
	if (locHeadNode != NULL)					locHeadNode->Release();

	return retVal;
}

bool CMMLParser::parseStreamTag(MSXML2::IXMLDOMNode* inStreamNode, C_StreamTag* outStream) {
	////-------------Initialisation-------------
	MSXML2::IXMLDOMNamedNodeMap*	locAttribMap	= NULL;
	MSXML2::IXMLDOMNode*			locNode			= NULL;
	MSXML2::IXMLDOMNodeList*		locChildNodes	= NULL;
	HRESULT							locHR			= S_FALSE;
	long							locNumNodes		= 0;
	BSTR							locBStr			= NULL;
	wstring							locNodeName		= L"";

	//---------------Attributes-----------------
	locHR = inStreamNode->get_attributes(&locAttribMap);
	//outStream->setLang(getNamedAttribValue(L"lang", locAttribMap));
	//outStream->setDirn(getNamedAttribValue(L"dir", locAttribMap));	
	outStream->setId(getNamedAttribValue(L"id", locAttribMap));
	outStream->setTimebase(getNamedAttribValue(L"timebase", locAttribMap));
	outStream->setUtc(getNamedAttribValue(L"utc", locAttribMap));
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
			if (parseImportTag(locNode, locImport)) {
				outStream->importList()->addTag(locImport);
			} else {
				//TODO::: Anything ???
			}
			
		} else {
			//TODO::: Handle this, otherwise ignore.
		}
	}

	//--------------Clean Up--------------------
	SysFreeString(locBStr);
	if (locAttribMap != NULL)		locAttribMap->Release();
	if (locNode != NULL)			locNode->Release();
	if (locChildNodes != NULL)		locChildNodes->Release();
	return true;
}

bool CMMLParser::parseImportTag(MSXML2::IXMLDOMNode* inImportNode, C_ImportTag* outImport) {

	//TODO::: Stream and import tags need checking about what to do with internationalisation
	MSXML2::IXMLDOMNamedNodeMap*	locAttribMap	= NULL;
	MSXML2::IXMLDOMNode*			locNode			= NULL;
	MSXML2::IXMLDOMNodeList*		locChildNodes	= NULL;
	HRESULT							locHR			= S_FALSE;
	long							locNumNodes		= 0;
	BSTR							locBStr			= NULL;
	wstring							locNodeName		= L"";

	bool retVal = true;

	//---------------Attributes-----------------
	locHR = inImportNode->get_attributes(&locAttribMap);

	outImport->setId(getNamedAttribValue(L"id", locAttribMap));
	outImport->setContentType(getNamedAttribValue(L"contenttype", locAttribMap));
	outImport->setSrc(getNamedAttribValue(L"src", locAttribMap));
	if (outImport->src() == L"") {
		//Source is required.
		retVal = false;
	}
	outImport->setStart(getNamedAttribValue(L"start", locAttribMap));
	outImport->setEnd(getNamedAttribValue(L"end", locAttribMap));
	outImport->setTitle(getNamedAttribValue(L"title", locAttribMap));
	outImport->setGranuleRate(getNamedAttribValue(L"granulerate", locAttribMap));
	//------------------------------------------


	locHR = inImportNode->get_childNodes(&locChildNodes);
	locHR = locChildNodes->get_length(&locNumNodes);
	for (int i = 0; i < locNumNodes; i++) {
		locHR = locChildNodes->get_item(i, &locNode);
		locHR = locNode->get_nodeName(&locBStr);
		locNodeName = locBStr;
		if(locNodeName == L"param") {
			
			C_ParamTag* locParam = new C_ParamTag;
			if (parseParamTag(locNode, locParam)) {
				outImport->paramList()->addTag(locParam);
			} else {
				//TODO::: Anything ???
			}
			
		} else {
			//TODO::: Handle this, otherwise ignore.
		}
	}

	SysFreeString(locBStr);
	if (locAttribMap != NULL)		locAttribMap->Release();
	if (locNode != NULL)			locNode->Release();
	if (locChildNodes != NULL)		locChildNodes->Release();
	return retVal;
}

bool CMMLParser::parseImageTag(MSXML2::IXMLDOMNode* inImageNode, C_ImageTag* outImage) {
	MSXML2::IXMLDOMNamedNodeMap* locAttribMap = NULL;
	//	MSXML2::IXMLDOMNode* locImageNode = NULL;
	HRESULT locHR = S_FALSE;

	bool retVal = true;
	//---------------Attributes-----------------
	locHR = inImageNode->get_attributes(&locAttribMap);
	outImage->setId(getNamedAttribValue(L"id", locAttribMap));
	outImage->setSrc(getNamedAttribValue(L"src", locAttribMap));
	if (outImage->src() == L"") {
		retVal = false;
	}
	outImage->setAlt(getNamedAttribValue(L"alt", locAttribMap));

	if (locAttribMap != NULL)		locAttribMap->Release();
	return retVal;
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
	HRESULT locHR = S_FALSE;

	bool retVal = true;
	//---------------Attributes-----------------
	locHR = inMetaNode->get_attributes(&locAttribMap);

	outMeta->setId(getNamedAttribValue(L"id", locAttribMap));
	outMeta->setLang(getNamedAttribValue(L"lang", locAttribMap));
	outMeta->setDirn(getNamedAttribValue(L"dir", locAttribMap));
	outMeta->setName(getNamedAttribValue(L"name", locAttribMap));
	if (outMeta->name() == L"") {
		retVal = false;
	} else {
		outMeta->setContent(getNamedAttribValue(L"content", locAttribMap));
		if (outMeta->content() == L"") {
			retVal = false;
		}
	}

	outMeta->setScheme(getNamedAttribValue(L"scheme", locAttribMap));
	//------------------------------------------

	if (locAttribMap != NULL)		locAttribMap->Release();
	return true;
}

//string CMMLParser::toNarrowStr(wstring inString) {
//	string retVal;
//
//
//	for (std::wstring::const_iterator i = inString.begin(); i != inString.end(); i++) {
//		retVal.append(1, *i);
//	}
//	
//	return retVal;
//}

bool CMMLParser::parseAnchorTag(MSXML2::IXMLDOMNode* inAnchorNode, C_AnchorTag* outAnchor) {
	MSXML2::IXMLDOMNamedNodeMap*		locAttribMap	= NULL;
	HRESULT								locHR			= S_FALSE;
	BSTR								locBStr			= NULL;
	wstring								locAnchorText	= L"";

	bool								retVal			= true;
	//---------------Attributes-----------------
	locHR = inAnchorNode->get_attributes(&locAttribMap);
	outAnchor->setId(getNamedAttribValue(L"id", locAttribMap));
	outAnchor->setLang(getNamedAttribValue(L"lang", locAttribMap));
	outAnchor->setDirn(getNamedAttribValue(L"dir", locAttribMap));
	outAnchor->setCls(getNamedAttribValue(L"class", locAttribMap));
	outAnchor->setHref(getNamedAttribValue(L"href", locAttribMap));
	if (outAnchor->href() == L"") {
		retVal = false;
	}
	//------------------------------------------
	//Anchor text

	inAnchorNode->get_text(&locBStr);
	locAnchorText = locBStr;

	//if (locAnchorText == L"") {
	//	retVal = false;
	//}
	outAnchor->setText(locAnchorText);

	//Cleanup
	SysFreeString(locBStr);
    if (locAttribMap != NULL)					locAttribMap->Release();
	return retVal;
}

bool CMMLParser::parseTitleTag(MSXML2::IXMLDOMNode* inTitleNode, C_TitleTag* outTitle) {
	MSXML2::IXMLDOMNamedNodeMap* locAttribMap = NULL;
	BSTR locBStr = NULL;
	wstring locTitleText;
	HRESULT locHR = S_FALSE;

	//---------------Attributes-----------------
	locHR = inTitleNode->get_attributes(&locAttribMap);
	
	outTitle->setId(getNamedAttribValue(L"id", locAttribMap));
	outTitle->setLang(getNamedAttribValue(L"lang", locAttribMap));
	outTitle->setDirn(getNamedAttribValue(L"dir", locAttribMap));
	//------------------------------------------

	inTitleNode->get_text(&locBStr);
	locTitleText = locBStr;
	outTitle->setText(locTitleText);

	SysFreeString(locBStr);
	if (locAttribMap != NULL)					locAttribMap->Release();
	return true;

}

bool CMMLParser::parseDescTag(MSXML2::IXMLDOMNode* inDescNode, C_DescTag* outDesc) {
	MSXML2::IXMLDOMNamedNodeMap* locAttribMap = NULL;
	BSTR locBStr = NULL;
	wstring locDescText;
	HRESULT locHR = S_FALSE;

	//---------------Attributes-----------------
	locHR = inDescNode->get_attributes(&locAttribMap);
	outDesc->setId(getNamedAttribValue(L"id", locAttribMap));
	outDesc->setLang(getNamedAttribValue(L"lang", locAttribMap));
	outDesc->setDirn(getNamedAttribValue(L"dir", locAttribMap));
	//------------------------------------------
	inDescNode->get_text(&locBStr);
	locDescText = locBStr;
	outDesc->setText(locDescText);

	SysFreeString(locBStr);
	if (locAttribMap != NULL)					locAttribMap->Release();
	return true;

}




bool CMMLParser::parseBaseTag(MSXML2::IXMLDOMNode* inBaseNode, C_BaseTag* outBase) {
	MSXML2::IXMLDOMNamedNodeMap* locAttribMap = NULL;
	HRESULT locHR = S_FALSE;

	bool retVal = true;
	//---------------Attributes-----------------
	locHR = inBaseNode->get_attributes(&locAttribMap);
	
	outBase->setId(getNamedAttribValue(L"id", locAttribMap));
	outBase->setHref(getNamedAttribValue(L"href", locAttribMap));
	if (outBase->href() == L"") {
		retVal = false;
	}
	//------------------------------------------

	if (locAttribMap != NULL)					locAttribMap->Release();
	return retVal;

}



bool CMMLParser::parseParamTag(MSXML2::IXMLDOMNode* inParamNode, C_ParamTag* outParam) {
	MSXML2::IXMLDOMNamedNodeMap* locAttribMap = NULL;
	HRESULT locHR = S_FALSE;

	bool retVal = true;
	//---------------Attributes-----------------
	locHR = inParamNode->get_attributes(&locAttribMap);

	outParam->setId(getNamedAttribValue(L"id", locAttribMap));
	outParam->setName(getNamedAttribValue(L"name", locAttribMap));
	if (outParam->name() == L"") {
		retVal = false;
	} else {
		outParam->setContent(getNamedAttribValue(L"value", locAttribMap));
		if (outParam->content() == L"") {
			retVal = false;
		}
	}

	
	//------------------------------------------

	if (locAttribMap != NULL)		locAttribMap->Release();
	return true;
}

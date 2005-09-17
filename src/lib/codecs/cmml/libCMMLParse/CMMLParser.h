//===========================================================================
//Copyright (C) 2004-2005 Zentaro Kavanagh
//
//Copyright (C) 2005 Commonwealth Scientific and Industrial Research
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


#include <libCMMLTags/libCMMLTags.h>
#include <libCMMLParse/libCMMLParse.h>
#include <libCMMLParse/xtag.h>

#include <string>

using namespace std;


class LIBCMMLPARSE_API CMMLParser
{
public:
	CMMLParser(void);
	~CMMLParser(void);

	/// Parse a string representation of a clip tag into the provided Clip object.
	bool parseClipTag(wstring inClipText, C_ClipTag* outClip);

	/// Parse a string representation of a head tag into the provided Head object.
	bool parseHeadTag(wstring inHeadText, C_HeadTag* outHead);

	/// Parse a string representation of a cmml root tag into the provided CMMLRoot object.
	bool parseCMMLRootTag(wstring inCMMLRootText, C_CMMLRootTag* outCMMLRoot);

	/// Parse a string representation of a cmml document into the provided CMMLDoc object.
	bool parseDocFromFile(wstring inFilename, C_CMMLDoc* outCMMLDoc);

	/// DLB. 9/9/2005. Parse a string representation of a cmml document into the provided CMMLDoc object.
	bool parseDoc(wstring inBuffer, C_CMMLDoc* outCMMLDoc);

protected:
	/// Parse an XTag representation of a cmml root tag into the provided CMMLRoot object.
	bool parseRootTag(XTag* inCMMLRootParser, C_CMMLRootTag* outCMMLRoot);

	/// Parse an XTag representation of a stream tag into the provided Stream object.
	bool parseStreamTag(XTag* inStreamParser, C_StreamTag* outStream);

	/// Parse an XTag representation of a head tag into the provided Head object.
	bool parseHeadTag(XTag* inHeadParser, C_HeadTag* outHead);

	/// Parse an XTag representation of a clip tag into the provided Clip object.
	bool parseClipTag(XTag* inClipParser, C_ClipTag* outClip);

	/// Parse an XTag representation of an import tag into the provided Import object.
	bool parseImportTag(XTag* inImportParser, C_ImportTag* outImport);

	/// Parse an XTag representation of a base tag into the provided Base object.
	bool parseBaseTag(XTag* inBaseParser, C_BaseTag* outBase);

	/// Parse an XTag representation of a title tag into the provided Title object.
	bool parseTitleTag(XTag* inTitleParser, C_TitleTag* outTitle);

	/// Parse an XTag representation of a meta tag into the provided MetaTag object.
	bool parseMetaTag(XTag* inMetaParser, C_MetaTag* outMeta);

	/// Parse an XTag representation of an anchor tag into the provided Anchor object.
	bool parseAnchorTag(XTag* inAnchorParser, C_AnchorTag* outAnchor);

	/// Parse an XTag representation of an image tag into the provided Image object.
	bool parseImageTag(XTag* inImageParser, C_ImageTag* outImage);

	/// Parse an XTag representation of a desc tag into the provided Desc object.
	bool parseDescTag(XTag* inDescParser, C_DescTag* outDesc);

	/// Parse an XTag representation of a param tag into the provided ParamTag object.
	bool parseParamTag(XTag* inParamParser, C_ParamTag* outParam);

};

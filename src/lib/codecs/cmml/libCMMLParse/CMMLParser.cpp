//===========================================================================
//Copyright (C) 2005 Zentaro Kavanagh
//
//Copyright (C) 2005 Commonwealth Scientific and Industrial Research
//                   Organisation (CSIRO) Australia
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


#include <libCMMLParse/CMMLParser.h>
#include <libCMMLParse/xtag.h>

#include <libCMMLTags/libCMMLTags.h>
#include <libilliCore/StringHelper.h>

#include <fstream>


using namespace std;

// TODO: Properly parse preamble
// TODO: i18n?

CMMLParser::CMMLParser(void)
{
}

CMMLParser::~CMMLParser(void)
{
}

bool CMMLParser::parseDocFromFile(wstring inFilename, C_CMMLDoc* outCMMLDoc)
{
	// Assume we are unsuccessful unless we explicitly change that

	bool locReturnValue = false;

	// Sanity check against a NULL output pointer
	if (!outCMMLDoc) {
		return false;
	}

	// Somebody set us up our file!

	fstream locFile;

	locFile.open(StringHelper::toNarrowStr(inFilename).c_str(),
		ios_base::in | ios_base::binary);

	// Look ma, the world's most portable file-size-getting-function-thing
	locFile.seekg(0, ios::end);
	size_t locCMMLFileSize = locFile.tellg();
	locFile.clear();

	// Read the entirety of the file into the buffer
	locFile.seekg(0);

	unsigned short BUFFER_SIZE = 8192;
	char *locBuffer = new char[locCMMLFileSize];
	size_t locBytesRead = 0;

	while (!locFile.eof()) {
		locFile.read(locBuffer + locBytesRead, BUFFER_SIZE);
		locBytesRead = locFile.gcount();
	}

	locFile.close();

	// Widen the file stream
	wstring locCMMLFileWString = StringHelper::toWStr(locBuffer);

	// Parse ourselves the CMML
	C_CMMLRootTag* locRootTag = new C_CMMLRootTag;
	locReturnValue = parseCMMLRootTag(locCMMLFileWString, locRootTag);
	if (locReturnValue) {
		// Successfully parsed the CMML
		outCMMLDoc->setRoot(locRootTag);
	} else {
		// Parsing CMML failed
		outCMMLDoc = NULL;
	}

	// Clean up
	delete [] locBuffer;

	return locReturnValue;
}


bool CMMLParser::parseCMMLRootTag(wstring inCMMLRootText, C_CMMLRootTag* outCMMLRoot)
{
	// Assume we are unsuccessful unless we explicitly change that

	bool locReturnValue = false;

	// Sanity check against a NULL output pointer
	if (!outCMMLRoot) {
		return false;
	}

	// Narrow the text given us, so we can pass it to XTag
	string locCMMLRootText = StringHelper::toNarrowStr(inCMMLRootText);

	// Look for a tag, any tag
	XTag *locRootParser = NULL;
	locRootParser = xtag_new_parse(locCMMLRootText.c_str(), (int)locCMMLRootText.size());
	if (locRootParser) {
		// Is it a <cmml> tag?
		if (strcmp(xtag_get_name(locRootParser), "cmml") == 0) {
			// Found a <cmml> tag
			locReturnValue = parseRootTag(locRootParser, outCMMLRoot);
		}
	}

	if (locRootParser) {
		xtag_free(locRootParser);
	}

	return locReturnValue;
}


bool CMMLParser::parseClipTag(wstring inClipText, C_ClipTag* outClip)
{
	// Assume we are unsuccessful unless we explicitly change that

	bool locReturnValue = false;

	// Sanity check against a NULL output pointer
	if (!outClip) {
		return false;
	}

	// Narrow the text given us, so we can pass it to XTag
	string locClipText = StringHelper::toNarrowStr(inClipText);

	// Look for a <cmml> tag
	XTag *locClipParser = NULL;
	locClipParser = xtag_new_parse(locClipText.c_str(), (int)locClipText.size());
	if (locClipParser) {
		// Found some sort of tag
		if (strcmp(xtag_get_name(locClipParser), "clip") == 0) {
			// Found a <clip> tag, go parse it
			locReturnValue = parseClipTag(locClipParser, outClip);
		}
	}

	if (locClipParser) {
		xtag_free(locClipParser);
	}

	return locReturnValue;
}


bool CMMLParser::parseHeadTag(wstring inHeadText, C_HeadTag* outHead)
{
	// Assume we are unsuccessful unless we explicitly change that

	bool locReturnValue = false;

	// Sanity check against a NULL output pointer
	if (!outHead) {
		return false;
	}

	// Narrow the text given us, so we can pass it to XTag
	string locHeadText = StringHelper::toNarrowStr(inHeadText);

	// Set up an XTag parser
	XTag *locHeadParser = NULL;
	locHeadParser = xtag_new_parse(locHeadText.c_str(), (int)locHeadText.size());
	if (locHeadParser) {
		if (strcmp(xtag_get_name(locHeadParser), "head") == 0) {
			locReturnValue = parseHeadTag(locHeadParser, outHead);
		}
	}

	if (locHeadParser) {
		xtag_free(locHeadParser);
	}

	return locReturnValue;
}


// Macros are evil, macros are evil, can't sleep, clown'll eat me ...

#define XTAG_PARSE_INTO(tagParser, parseMethod, TagType, parentTagSetter, parentTag) \
	{ \
		TagType *locTag = new TagType; \
		if (!parseMethod(tagParser, locTag)) { \
			return false; \
		} \
		parentTag->parentTagSetter(locTag); \
	};

#define XTAG_SET_ATTRIBUTE(tagParser, attributeName, tag, attributeSetter) \
	{ \
		const char *locAttributeCString = xtag_get_attribute(tagParser, attributeName); \
		if (locAttributeCString) { \
			tag->attributeSetter(StringHelper::toWStr(locAttributeCString)); \
			/* free((void *) locAttributeCString); */ \
		} \
	};

#define XTAG_REQUIRED_ATTRIBUTE(tagParser, attributeName, tag) \
	{ \
		const char *locAttributeCString = xtag_get_attribute(tagParser, attributeName); \
		if (!locAttributeCString) { \
			return false; \
		} else { \
			/* free((void *) locAttributeCString); */ \
		} \
	};

#define XTAG_PARSE_CHILD(parentParser, tagName, tagParser, tagType, setterMethod, parentTag) \
	{ \
		XTag *locParser = NULL; \
		locParser = xtag_first_child(parentParser, tagName); \
		if (locParser) { \
			XTAG_PARSE_INTO(locParser, tagParser, tagType, setterMethod, parentTag); \
		} \
	};

#define XTAG_EXACTLY_ONE_CHILD(parentParser, tagName) \
	{ \
		XTag *locParser = xtag_first_child(parentParser, tagName); \
		if (locParser != NULL) { \
			/* Found at least one child */ \
			locParser = xtag_next_child(parentParser, tagName); \
			if (locParser) { \
				/* Danger will robinson, found more than one child */ \
				return false; \
			} \
		} else { \
			/* Found no child */ \
			return false; \
		} \
	};

#define XTAG_PARSE_LIST(TagType, listTagName, tagParser, parentParser, parentTag, parentGetListMethod) \
	{ \
		XTag *locTagListParser = NULL; \
		for (	locTagListParser = xtag_first_child(parentParser, listTagName); \
				locTagListParser != NULL; \
				locTagListParser = xtag_next_child(parentParser, listTagName)) { \
			XTAG_PARSE_INTO(locTagListParser, tagParser, TagType, addTag, parentTag->parentGetListMethod()); \
		} \
	};

#define XTAG_SET_CDATA(tagParser, tag) \
	{ \
		const char *locCData = xtag_get_pcdata(tagParser); \
		if (locCData) { \
			tag->setText(StringHelper::toWStr(locCData)); \
			/* free((void *) locCData); */ \
		} \
	};


// Look ma, it's declarative programming!

bool CMMLParser::parseStreamTag(XTag* inStreamParser, C_StreamTag* outStream)
{
	XTAG_SET_ATTRIBUTE(inStreamParser, "id", outStream, setId);
	XTAG_SET_ATTRIBUTE(inStreamParser, "timebase", outStream, setTimebase);
	XTAG_SET_ATTRIBUTE(inStreamParser, "utc", outStream, setUtc);

	XTAG_PARSE_LIST(C_ImportTag, "import", parseImportTag,
		inStreamParser, outStream, importList);

	return true;
}


bool CMMLParser::parseRootTag(XTag* inCMMLRootParser, C_CMMLRootTag* outCMMLRoot)
{
	XTAG_SET_ATTRIBUTE(inCMMLRootParser, "id", outCMMLRoot, setId);

	XTAG_EXACTLY_ONE_CHILD(inCMMLRootParser, "head");
	XTAG_PARSE_CHILD(inCMMLRootParser, "head", parseHeadTag, C_HeadTag, setHead, outCMMLRoot);
	XTAG_PARSE_CHILD(inCMMLRootParser, "stream", parseStreamTag, C_StreamTag, setStream, outCMMLRoot);

	XTAG_PARSE_LIST(C_ClipTag, "clip", parseClipTag, inCMMLRootParser, outCMMLRoot, clipList);

	// i18n
	XTAG_SET_ATTRIBUTE(inCMMLRootParser, "lang", outCMMLRoot, setLang);
	XTAG_SET_ATTRIBUTE(inCMMLRootParser, "dir", outCMMLRoot, setDirn);

	return true;
}

bool CMMLParser::parseHeadTag(XTag* inHeadParser, C_HeadTag* outHead)
{
	XTAG_SET_ATTRIBUTE(inHeadParser, "id", outHead, setId);
	XTAG_SET_ATTRIBUTE(inHeadParser, "profile", outHead, setProfile);

	XTAG_EXACTLY_ONE_CHILD(inHeadParser, "title");
	XTAG_PARSE_CHILD(inHeadParser, "title", parseTitleTag, C_TitleTag, setTitle, outHead);
	XTAG_PARSE_CHILD(inHeadParser, "base", parseBaseTag, C_BaseTag, setBase, outHead);

	XTAG_PARSE_LIST(C_MetaTag, "meta", parseMetaTag, inHeadParser, outHead, metaList);

	// i18n
	XTAG_SET_ATTRIBUTE(inHeadParser, "lang", outHead, setLang);
	XTAG_SET_ATTRIBUTE(inHeadParser, "dir", outHead, setDirn);

	return true;
}

bool CMMLParser::parseTitleTag(XTag* inTitleParser, C_TitleTag* outTitle)
{
	XTAG_SET_ATTRIBUTE(inTitleParser, "id", outTitle, setId);

	XTAG_SET_CDATA(inTitleParser, outTitle);

	// i18n
	XTAG_SET_ATTRIBUTE(inTitleParser, "lang", outTitle, setLang);
	XTAG_SET_ATTRIBUTE(inTitleParser, "dir", outTitle, setDirn);

	return true;
}

bool CMMLParser::parseBaseTag(XTag* inBaseParser, C_BaseTag* outBase)
{
	XTAG_SET_ATTRIBUTE(inBaseParser, "id", outBase, setId);
	XTAG_SET_ATTRIBUTE(inBaseParser, "href", outBase, setHref);
	XTAG_REQUIRED_ATTRIBUTE(inBaseParser, "href", outBase);

	return true;
}

bool CMMLParser::parseMetaTag(XTag* inMetaParser, C_MetaTag* outMeta)
{
	XTAG_SET_ATTRIBUTE(inMetaParser, "scheme", outMeta, setScheme);
	XTAG_SET_ATTRIBUTE(inMetaParser, "content", outMeta, setContent);
	XTAG_SET_ATTRIBUTE(inMetaParser, "id", outMeta, setId);
	XTAG_SET_ATTRIBUTE(inMetaParser, "name", outMeta, setName);

	// i18n
	XTAG_SET_ATTRIBUTE(inMetaParser, "lang", outMeta, setLang);
	XTAG_SET_ATTRIBUTE(inMetaParser, "dir", outMeta, setDirn);

	return true;
}

bool CMMLParser::parseClipTag(XTag* inClipParser, C_ClipTag* outClip)
{
	XTAG_SET_ATTRIBUTE(inClipParser, "track", outClip, setTrack);
	XTAG_SET_ATTRIBUTE(inClipParser, "id", outClip, setId);
	XTAG_SET_ATTRIBUTE(inClipParser, "start", outClip, setStart);
	XTAG_REQUIRED_ATTRIBUTE(inClipParser, "start", outClip);
	XTAG_SET_ATTRIBUTE(inClipParser, "end", outClip, setEnd);

	XTAG_PARSE_LIST(C_MetaTag, "meta", parseMetaTag, inClipParser, outClip, metaList);

	XTAG_PARSE_CHILD(inClipParser, "a", parseAnchorTag, C_AnchorTag, setAnchor, outClip);
	XTAG_PARSE_CHILD(inClipParser, "img", parseImageTag, C_ImageTag, setImage, outClip);
	XTAG_PARSE_CHILD(inClipParser, "desc", parseDescTag, C_DescTag, setDesc, outClip);

	// i18n
	XTAG_SET_ATTRIBUTE(inClipParser, "lang", outClip, setLang);
	XTAG_SET_ATTRIBUTE(inClipParser, "dir", outClip, setDirn);

	return true;
}

bool CMMLParser::parseAnchorTag(XTag* inAnchorParser, C_AnchorTag* outAnchor)
{
	XTAG_SET_ATTRIBUTE(inAnchorParser, "id", outAnchor, setId);
	XTAG_SET_ATTRIBUTE(inAnchorParser, "class", outAnchor, setCls);
	XTAG_SET_ATTRIBUTE(inAnchorParser, "href", outAnchor, setHref);
	XTAG_REQUIRED_ATTRIBUTE(inAnchorParser, "href", outAnchor);

	XTAG_SET_CDATA(inAnchorParser, outAnchor);

	// i18n
	XTAG_SET_ATTRIBUTE(inAnchorParser, "lang", outAnchor, setLang);
	XTAG_SET_ATTRIBUTE(inAnchorParser, "dir", outAnchor, setDirn);

	return true;
}

bool CMMLParser::parseImageTag(XTag* inImageParser, C_ImageTag* outImage)
{
	XTAG_SET_ATTRIBUTE(inImageParser, "id", outImage, setId);
	XTAG_SET_ATTRIBUTE(inImageParser, "src", outImage, setSrc);
	XTAG_REQUIRED_ATTRIBUTE(inImageParser, "src", outImage);
	XTAG_SET_ATTRIBUTE(inImageParser, "alt", outImage, setAlt);

	// i18n
	XTAG_SET_ATTRIBUTE(inImageParser, "lang", outImage, setLang);
	XTAG_SET_ATTRIBUTE(inImageParser, "dir", outImage, setDirn);

	return true;
}

bool CMMLParser::parseDescTag(XTag* inDescParser, C_DescTag* outDesc)
{
	XTAG_SET_ATTRIBUTE(inDescParser, "id", outDesc, setId);

	XTAG_SET_CDATA(inDescParser, outDesc);

	// i18n
	XTAG_SET_ATTRIBUTE(inDescParser, "lang", outDesc, setLang);
	XTAG_SET_ATTRIBUTE(inDescParser, "dir", outDesc, setDirn);

	return true;
}

bool CMMLParser::parseImportTag(XTag* inImportParser, C_ImportTag* outImport)
{
	XTAG_SET_ATTRIBUTE(inImportParser, "granulerate", outImport, setGranuleRate);
	XTAG_SET_ATTRIBUTE(inImportParser, "contenttype", outImport, setContentType);
	XTAG_SET_ATTRIBUTE(inImportParser, "src", outImport, setSrc);
	XTAG_SET_ATTRIBUTE(inImportParser, "start", outImport, setStart);
	XTAG_SET_ATTRIBUTE(inImportParser, "end", outImport, setEnd);
	XTAG_SET_ATTRIBUTE(inImportParser, "title", outImport, setTitle);

	XTAG_PARSE_LIST(C_ParamTag, "param", parseParamTag, inImportParser, outImport, paramList);

	return true;
}

bool CMMLParser::parseParamTag(XTag* inParamParser, C_ParamTag* outParam)
{
	XTAG_SET_ATTRIBUTE(inParamParser, "id", outParam, setId);
	XTAG_SET_ATTRIBUTE(inParamParser, "name", outParam, setName);
	XTAG_REQUIRED_ATTRIBUTE(inParamParser, "name", outParam);
	XTAG_SET_ATTRIBUTE(inParamParser, "value", outParam, setContent);
	XTAG_REQUIRED_ATTRIBUTE(inParamParser, "value", outParam);

	return true;
}

#undef XTAG_REQUIRED_ATTRIBUTE

#undef XTAG_SET_ATTRIBUTE

#undef XTAG_PARSE_INTO

#undef XTAG_SET_CDATA

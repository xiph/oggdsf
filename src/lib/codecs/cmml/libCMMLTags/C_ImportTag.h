/*
   Copyright (C) 2003 Zentaro Kavanagh
   
   Copyright (C) 2003 Commonwealth Scientific and Industrial Research
   Organisation (CSIRO) Australia

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   - Neither the name of CSIRO Australia nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
   PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE ORGANISATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once

//STL Include Files
#include <string>
using namespace std;

//Local Include Files
#include "C_HumReadCMMLTag.h"
#include "C_ParamTagList.h"
#include "C_CMMLTime.h"

class LIBCMMLTAGS_API C_ImportTag
	//Derived Classes
	: public C_HumReadCMMLTag
{
public:
	//Constructors
	C_ImportTag(void);
	virtual ~C_ImportTag(void);

	//Accessors
	string granuleRate();
	string contentType();
	string src();
	string start();
	string end();
	string title();
	C_ParamTagList* paramList();

	//Mutators
	void setGranuleRate(string inGranuleRate);
	void setContentType(string inContentType);
	void setSrc(string inSrc);
	void setStart(string inStart);
	void setEnd(string inEnd);
	void setTitle(string inTitle);
	void C_ImportTag::setParamList(C_ParamTagList* inParamList);

	//Other
	virtual string toString();
	C_ImportTag* clone();
	virtual C_CMMLTag* genericClone();

protected:
	//Property Data

	//FIX ::: Change this to appropriate integer type ??
	string mGranuleRate;
	string mContentType;
	string mSrc;
	string mStart;
	string mEnd;
	string mTitle;
	C_ParamTagList* mParamList;

	//Protected Helper Methods
	virtual void privateClone(C_CMMLTag* outTag);


};


	//Original structure
	//
	//typedef struct {
	//	char *id;               /**< id of import tag */
	//	char *lang;             /**< language code of import tag */
	//	char *dir;              /**< directionality of lang (ltr/rtl) */
	//	char *granulerate;      /**< base temporal resolution in Hz */
	//	char *contenttype;      /**< content type of the import bitstream */
	//	char *src;              /**< URI to import document */
	//	CMML_Time * start_time; /**< insertion time in annodex bitstream */
	//	CMML_Time * end_time;   /**< end time of this logical  bitstream */
	//	char *title;            /**< comment on the import bitstream */
	//	CMML_List * param;      /**< list of optional further nam-value
	//		     metadata for the import bitstreams */
	//} CMML_ImportElement;
	//

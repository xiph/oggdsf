/*
   Copyright (C) 2003, 2004 Zentaro Kavanagh
   
   Copyright (C) 2003, 2004 Commonwealth Scientific and Industrial Research
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
#include "C_MappedTag.h"

class LIBCMMLTAGS_API C_MetaTag
	//Derived Classes
	: public C_MappedTag
{
public:
	//Constructors
	C_MetaTag(void);
	virtual ~C_MetaTag(void);

	//Accessors
	wstring scheme();

	//Mutators
	void setScheme(wstring inScheme);

	//Other
	virtual wstring toString();
	C_MetaTag* clone();
	virtual C_CMMLTag* genericClone();
	virtual C_MappedTag* mappedClone();

protected:
	//Property Data
	wstring mScheme;

	//Protected Helper Methods
	virtual void privateClone(C_CMMLTag* outTag);


};


	//typedef struct {
	//	char *id;      /**< id attribute of meta element */
	//	char *lang;    /**< language code of meta element */
	//	char *dir;     /**< directionality of lang (ltr/rtl) */
	//	char *name;    /**< property name of meta element */
	//	char *content; /**< property value of meta element */
	//	char *scheme;  /**< scheme name of meta element */
	//} CMML_MetaElement;

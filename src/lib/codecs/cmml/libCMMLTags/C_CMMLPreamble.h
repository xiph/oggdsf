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

class LIBCMMLTAGS_API C_CMMLPreamble
{
public:
	//Constructors
	C_CMMLPreamble(void);
	virtual ~C_CMMLPreamble(void);



	//Accessors
	wstring xmlVersion();
	wstring xmlEncoding();
	wstring xmlStandAlone();
	
	//Mutators
	void setXmlVersion(wstring inVersion);
	void setXmlEncoding(wstring inEncoding);
	void setXmlStandAlone(wstring inStandAlone);

	//Others
	virtual wstring toString();
	C_CMMLPreamble* clone();


protected:
	//Original Structure
	//
	//typedef struct {
	//	char *xml_version;      /**< version attribute of xml proc instr */
	//	char *xml_encoding;     /**< encoding attribute of xml proc instr */
	//	int   xml_standalone;   /**< standalone attribute of xml proc instr */
	//	int   doctype_declared; /**< was doctype declared */
	//	char *cmml_lang;        /**< lang attribute of cmml tag */
	//	char *cmml_dir;         /**< dir attribute of cmml tag */
	//	char *cmml_id;          /**< id attribute of cmml tag */
	//	char *cmml_xmlns;       /**< xmlns attribute of cmml tag */
	//} CMML_Preamble;

	//Property Data
	wstring mXmlVersion;
	wstring mXmlEncoding;
	wstring mXmlStandAlone;
};

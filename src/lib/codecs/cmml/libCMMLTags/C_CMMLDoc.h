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
#include <libCMMLTags/C_CMMLPreamble.h>
#include <libCMMLTags/C_CMMLRootTag.h>


class LIBCMMLTAGS_API C_CMMLDoc
{
public:
	//Constructors
	C_CMMLDoc(void);
	virtual ~C_CMMLDoc(void);

	/// Returns an internal pointer to the preamble element. Can manipulate but not delete.
	C_CMMLPreamble* preamble();

	/// Returns and internal pointer to the cmml root tag. Can manipulate but not delete.
	C_CMMLRootTag* root();

	/// Sets the root cmml tag for this document.
	void setRoot(C_CMMLRootTag* inRootTag);

	/// Returns an xml string representing the complete document.
	virtual wstring toString();


	C_CMMLDoc* clone();

protected:
	C_CMMLPreamble* mPreamble;
	C_CMMLRootTag* mRoot;
};

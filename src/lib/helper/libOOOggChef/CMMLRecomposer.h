//===========================================================================
//Copyright (C) 2005 Zentaro Kavanagh
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


#pragma once


#include "IRecomposer.h"

#include <libOOOgg/libOOOgg.h>
#include <libOOOggChef/libOOOggChef.h>

#include <string>
#include <vector>

using namespace std;

class LIBOOOGGCHEF_API CMMLRecomposer : public IRecomposer, public IOggCallback
{
public:
	CMMLRecomposer(void);
#ifdef UNICODE
	CMMLRecomposer(wstring inCMMLFilename, BufferWriter inBufferWriter, void* inBufferWriterUserData);
#else
	CMMLRecomposer(string inCMMLFilename, BufferWriter inBufferWriter, void* inBufferWriterUserData);
#endif
	~CMMLRecomposer(void);

	bool recomposeStreamFrom(double inStartingTimeOffset, const vector<string>* inWantedMIMETypes);
	bool acceptOggPage(OggPage* inOggPage);

    CMMLRecomposer(const CMMLRecomposer&);  // Don't copy me
    CMMLRecomposer &operator=(const CMMLRecomposer&);  // Don't assign men

protected:

	BufferWriter mBufferWriter;
	void* mBufferWriterUserData;

#ifdef UNICODE
	wstring mCMMLFilename;
#else
	string mCMMLFilename;
#endif

	const vector<string>* mWantedMIMETypes;
};

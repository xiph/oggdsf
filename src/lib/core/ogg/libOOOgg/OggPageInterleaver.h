//===========================================================================
//Copyright (C) 2003, 2004 Zentaro Kavanagh
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
#include "IOggCallback.h"
#include "INotifyComplete.h"
#include "OggMuxStream.h"
#include <vector>

//DEBUG ONLY
//#include <fstream>
//
using namespace std;
class LIBOOOGG_API OggPageInterleaver
	:	public INotifyArrival
{
public:
	//TODO::: Shuoldn't be called file writer... just output or something.
	OggPageInterleaver(IOggCallback* inFileWriter, INotifyComplete* inNotifier);
	virtual ~OggPageInterleaver(void);

	virtual OggMuxStream* newStream();
	virtual void processData();
	virtual void writeLowest();

	virtual bool isProcessable();
	virtual bool isAllEOS();
	virtual bool isAllEmpty();

	//INotifyArrival Implementation
	virtual void notifyArrival();

protected:
	vector<OggMuxStream*> mInputStreams;
	IOggCallback* mFileWriter;		//TODO::: Shuoldn't be called filewriter.
	INotifyComplete* mNotifier;

	//DEBUG ONLY
	//fstream debugLog;
	//
private:
	OggPageInterleaver& operator=(const OggPageInterleaver& other);  /* Don't assign me */
	OggPageInterleaver(const OggPageInterleaver& other); /* Don't copy me */
};

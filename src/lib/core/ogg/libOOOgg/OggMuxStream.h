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

#include "OggPage.h"
#include "IOggCallback.h"
#include "INotifyArrival.h"
#include <deque>
using namespace std;
class LIBOOOGG_API OggMuxStream
	:	public IOggCallback
{
public:
	OggMuxStream(INotifyArrival* inNotifier);
	virtual ~OggMuxStream(void);
	static const __int64 INT64_MAX = 9223372036854775807;

	virtual bool acceptOggPage(OggPage* inOggPage);
	virtual OggPage* popFront();
	virtual OggPage* peekFront();
	virtual __int64 frontTime();
	virtual bool isEmpty();
	virtual bool isEOS();
	virtual bool isProcessable();
	virtual void setIsEOS(bool inIsEOS);
	virtual bool isActive();
	virtual void setIsActive(bool inIsActive);
	
protected:
	bool mIsEOS;
	bool mIsActive;
	deque<OggPage*> mPageQueue;
	INotifyArrival* mNotifier;

};
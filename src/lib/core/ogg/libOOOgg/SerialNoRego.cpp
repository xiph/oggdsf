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

#include "StdAfx.h"
#include ".\serialnorego.h"

SerialNoRego::SerialNoRego(unsigned long inSerialNo)
{
	mSerialNo = inSerialNo;
}

SerialNoRego::~SerialNoRego(void)
{
}

bool SerialNoRego::registerCB(OggCallbackRego* inCBRego) {
	//Put this callback registration in the list
	if (inCBRego != NULL) {
		mCBList.push_back(inCBRego);
		return true;
	} else {
		return false;
	}
}
void SerialNoRego::dispatch(OggPage* inOggPage) {
	//Loop through all our callbacks and fire the page off to all of them.
	for (unsigned long i = 0; i < mCBList.size(); i ++) {
		mCBList[i]->dispatch(inOggPage);
	}
	//NOTE : It is up to the OggCallbackRego to make copies if nescessary
	//		 Perhaps need different callbacks for const read only callbacks
	//		 Also maybe want to reference count.
}

unsigned long SerialNoRego::numCallbacks() {
	return (unsigned long)mCBList.size();
}
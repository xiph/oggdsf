//===========================================================================
//Copyright (C) 2003, 2004 Zentaro Kavanagh
//
//Copyright (C) 2003, 2004 Commonwealth Scientific and Industrial Research
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

#include "StdAfx.h"
#include "anxstreammapper.h"

AnxStreamMapper::AnxStreamMapper(OggDemuxSourceFilter* inOwningFilter)
	:	OggStreamMapper(inOwningFilter)
	,	mAnnodexSerial(0)
	,	mSeenAnnodexBOS(false)
	,	mReadyForCodecs(false)
	,	mSeenCMML(false)
{
}

AnxStreamMapper::~AnxStreamMapper(void)
{
}

bool AnxStreamMapper::isReady() {
	bool retVal = true;
	bool locWasAny = false;
	//Use 1 instead of 0... cmml is always ready and terminates the graph creation early.
	//We want to know when everything else is ready.
	for (unsigned long i = 1; i < mStreamList.size(); i++) {
		locWasAny = true;
		retVal = retVal && mStreamList[i]->streamReady();
	}
	return locWasAny && retVal;
}
bool AnxStreamMapper::acceptOggPage(OggPage* inOggPage) 
{
	//ANXTOFIX::: This was changed in the ogg demux.
	if(!isReady()) {
		mDataStartsAt += inOggPage->pageSize();
	}

	if (!mReadyForCodecs) {
		if (inOggPage->header()->isBOS()) {
			if (!mSeenAnnodexBOS) {
				//This is an annodex BOS page
				//Need to verify it's an annodex page too
				mAnnodexSerial = inOggPage->header()->StreamSerialNo();
				mSeenAnnodexBOS = true;
				
				//Need to save the data from the header here.
			} else {
				//This is anxdata
				

				if ( (mSeenCMML == false) ) {
					//This is a really nasty way to do it ! Fix this up properly later.
					char* locStr = (char*)(inOggPage->getPacket(0)->packetData() + 28);
					if (strstr(locStr, "text/x-cmml") != NULL) {
						mSeenCMML = true;
						OggStream* locStream = new CMMLStream(inOggPage, mOwningFilter, true);//OggStreamFactory::CreateStream(inOggPage, mOwningFilter);
						if (locStream != NULL) {
							mStreamList.push_back(locStream);
						}
					}
				} else {
				//Need to save header data here.
					mSeenStreams.push_back(inOggPage->header()->StreamSerialNo());
				}
			}
		}


		if (inOggPage->header()->isEOS()) {
			if (mSeenAnnodexBOS) {
				//This is the annodex EOS page
				mReadyForCodecs = true;
			} else {
				//ERROR... got an EOS before we've seen the annodex BOS
				return false;
			}
		}
		return true;
	} else {
		vector<unsigned long>::iterator it;
		int i;
		for(i = 0, it = mSeenStreams.begin(); it != mSeenStreams.end(); i++, ++it) {
		//for (int i = 0; i < mSeenStreams.size(); i++) {
			if (mSeenStreams[i] == inOggPage->header()->StreamSerialNo()) {
				//If the page is a BOS we need to start a new stream
				OggStream* locStream = OggStreamFactory::CreateStream(inOggPage, mOwningFilter, false);
				//FIX::: Need to check for NULL
				if (locStream != NULL) {
					mStreamList.push_back(locStream);
				}
				mSeenStreams.erase(it);
				return true;
			}
		}

		//If we are here, the stream is not in the list.
		//At the moment we assume it's because it's been seen, and removed... this is a bad assumption !

		return dispatchPage(inOggPage);
	}
}
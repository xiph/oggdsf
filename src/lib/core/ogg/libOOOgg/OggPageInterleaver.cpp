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
#include "oggpageinterleaver.h"

OggPageInterleaver::OggPageInterleaver(IOggCallback* inFileWriter, INotifyComplete* inNotifier)
	:	mFileWriter(inFileWriter)
	,	mNotifier(inNotifier)
{
	//debugLog.open("G:\\logs\\interleaver.log", ios_base::out);
}

OggPageInterleaver::~OggPageInterleaver(void)
{
	//debugLog.close();
}

OggMuxStream* OggPageInterleaver::newStream() {
	OggMuxStream* retStream = new OggMuxStream(this);
	mInputStreams.push_back(retStream);
	return retStream;
}

void OggPageInterleaver::notifyArrival() {
	processData();
}
void OggPageInterleaver::processData() {
	/*
		IF ALL EOS THEN
			FINSH UP
			SIGNAL END OF STREAMS
		ELSE
			WHILE IS PROCESSABLE
				lowestStream = NULL
				FOR EACH stream in mInputStreams
					IF NOT stream IS EMPTY THEN
						IF lowestStream = NULL THEN
							lowestStream = stream
						ELSE IF stream.frontTime < lowestStream.frontTime THEN
							lowestStream = stream
						END IF
					END IF
				NEXT stream
				IF lowestStream = NULL THEN
					CURSE LOUDLY
				ELSE
					WRITE lowestStream.frontPage
				END IF
			WEND
		END IF
	*/
	//
	//Temp


	//debugLog<<endl<<"Process Data : "<<endl;
	//debugLog<<"==============="<<endl;
	if (isAllEOS()) {
		//debugLog<<"Process Data : All are EOS."<<endl;
		//Finish up
		while (!isAllEmpty()) {
			//debugLog<<"Process Data : Finishing - Still not empty..."<<endl;
			writeLowest();
		}
		//debugLog<<"Process Data : Notifying completion... 1"<<endl;
		mNotifier->NotifyComplete();
	} else {
		//debugLog<<"Process Data : *NOT* all EOS"<<endl;
		while (isProcessable()) {
			//debugLog<<"Process Data : Still processable data..."<<endl;
			writeLowest();
		}
		//debugLog<<"Process Data : No more processable data..."<<endl;
		if (isAllEOS() && isAllEmpty()) {
			//debugLog<<"Process Data : All EOS and all Empty... Notifying complete 2..."<<endl;
			mNotifier->NotifyComplete();
		}
	}
	//debugLog<<"==============="<<endl;

}

void OggPageInterleaver::writeLowest() {
		OggMuxStream* locLowestStream = NULL;
		for (int i = 0; i < mInputStreams.size(); i++) {
			if (!mInputStreams[i]->isEmpty() && mInputStreams[i]->isActive()) {
				if (locLowestStream == NULL) {
					locLowestStream = mInputStreams[i];
					//debugLog<<"writeLowest : Defaulting stream "<<i<<" @ Gran = "<<locLowestStream->frontTime()<<" & Time = "<<locLowestStream->scaledFrontTime()<<endl;
					//debugLog<<"writeLowest : Defaulting stream "<<i<<endl;
				} else {
					__int64 locCurrLowTime = locLowestStream->scaledFrontTime();
					__int64 locTestLowTime = mInputStreams[i]->scaledFrontTime();
					//debugLog<<"writeLowest : Curr = "<<locCurrLowTime<<" -- Test["<<i<<"] = "<<locTestLowTime<<endl;
					
					//In english this means... any bos pages go first... then any no gran pos pages (-1 gran pos).. then
					// whoevers got the lowest time.
					if (
						(	(mInputStreams[i]->peekFront() != NULL) && 
							(mInputStreams[i]->peekFront()->header()->isBOS()) ) ||
						
						(	(mInputStreams[i]->peekFront() != NULL) && 
							((mInputStreams[i]->peekFront()->header()->GranulePos()->value()) == -1) ) ||
						(locTestLowTime < locCurrLowTime)
						) 
					{
						
						locLowestStream = mInputStreams[i];
						//debugLog<<"writeLowest : Selecting stream "<<i<<" @ Gran = "<<locLowestStream->frontTime()<<" & Time = "<<locLowestStream->scaledFrontTime()<<endl;
					}
				}
			}
		}
		if (locLowestStream == NULL) {
			throw 0;
		} else {
			//debugLog<<"writeLowest : Writing..."<<endl;
			mFileWriter->acceptOggPage(locLowestStream->popFront());
		}

}
bool OggPageInterleaver::isProcessable() {
	bool retVal = true;
	//ASSERT(mInputStreams.size() >= 1)
	for (int i = 0; i < mInputStreams.size(); i++) {
		retVal = retVal && (mInputStreams[i]->isProcessable());
	}
	return retVal;
}
bool OggPageInterleaver::isAllEOS() {
	bool retVal = true;
	//ASSERT(mInputStreams.size() >= 1)
	for (int i = 0; i < mInputStreams.size(); i++) {
		if (mInputStreams[i]->isEOS()) {
			//debugLog<<"*****                  Stream "<<i<<" is EOS"<<endl;
		} else {
			//debugLog<<"*****                  Stream "<<i<<" not EOS"<<endl;
		}
		retVal = retVal && (mInputStreams[i]->isEOS() || !mInputStreams[i]->isActive());
	}
	return retVal;
}

bool OggPageInterleaver::isAllEmpty() {
	bool retVal = true;
	//ASSERT(mInputStreams.size() >= 1)
	for (int i = 0; i < mInputStreams.size(); i++) {
		retVal = retVal && (mInputStreams[i]->isEmpty());
	}
	return retVal;
}
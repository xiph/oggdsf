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

}

OggPageInterleaver::~OggPageInterleaver(void)
{
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

	if (isAllEOS()) {
		//Finish up
		while (!isAllEmpty()) {
			writeLowest();
		}
		mNotifier->NotifyComplete();
	} else {
		while (isProcessable()) {
			writeLowest();
		}
		if (isAllEOS() && isAllEmpty()) {
			mNotifier->NotifyComplete();
		}
	}

}

void OggPageInterleaver::writeLowest() {
		OggMuxStream* locLowestStream = NULL;
		for (int i = 0; i < mInputStreams.size(); i++) {
			if (!mInputStreams[i]->isEmpty() && mInputStreams[i]->isActive()) {
				if (locLowestStream == NULL) {
					locLowestStream = mInputStreams[i];
				} else if (mInputStreams[i]->frontTime() < locLowestStream->frontTime()) {
					locLowestStream = mInputStreams[i];
				}
			}
		}
		if (locLowestStream == NULL) {
			throw 0;
		} else {
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
		retVal = retVal && (mInputStreams[i]->isEOS());
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
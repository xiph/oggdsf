#include "StdAfx.h"
#include "oggpageinterleaver.h"

OggPageInterleaver::OggPageInterleaver(IOggCallback* inFileWriter)
	:	mFileWriter(inFileWriter)
{

}

OggPageInterleaver::~OggPageInterleaver(void)
{
}

OggMuxStream* OggPageInterleaver::newStream() {
	OggMuxStream* retStream = new OggMuxStream;
	mInputStreams.push_back(retStream);
	return retStream;
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
	} else {
		while (isProcessable()) {
			writeLowest();
		}
	}

}

void OggPageInterleaver::writeLowest() {
		OggMuxStream* locLowestStream = NULL;
		for (int i = 0; i < mInputStreams.size(); i++) {
			if (!mInputStreams[i]->isEmpty()) {
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
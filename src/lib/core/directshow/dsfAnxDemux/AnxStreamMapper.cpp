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

#include "stdafx.h"
#include "anxstreammapper.h"

//#define OGGCODECS_LOGGING
AnxStreamMapper::AnxStreamMapper(OggDemuxSourceFilter* inOwningFilter)
	:	OggStreamMapper(inOwningFilter)
	,	mAnnodexSerial(0)
	,	mSeenAnnodexBOS(false)
	,	mReadyForCodecs(false)
	,	mSeenCMML(false)
	,	mDemuxState(eAnxDemuxState::SEEN_NOTHING)
	,	mAnxVersion(0)
	,	mAnnodexHeader(NULL)
{
#ifdef OGGCODECS_LOGGING
	debugLog.open("G:\\logs\\anxmapper.log", ios_base::out);
#endif
}

AnxStreamMapper::~AnxStreamMapper(void)
{
	debugLog.close();
}

bool AnxStreamMapper::isReady() {
	if (mAnxVersion == 0) {
		return false;
	} else if (mAnxVersion == ANX_VERSION_3_0) {
		return OggStreamMapper::isReady();
	} else {
		bool retVal = true;
		bool locWasAny = false;
		//Use 1 instead of 0... cmml is always ready and terminates the graph creation early.
		//We want to know when everything else is ready.

		//XXXXXXXXXXXXXX:::: Big dirty hack to deal with badly ordered header packets !!
		//=======================================================================
		//The side effect is... a file will hang if it has unknown streams.
		//=======================================================================
		if (mSeenStreams.size() == 0) {
			retVal = true;
		} else {
			retVal = false;
		}
		
		for (unsigned long i = 1; i < mStreamList.size(); i++) {
			locWasAny = true;
			retVal = retVal && mStreamList[i]->streamReady();
			//debugLog<<"Stream "<<i<<" ";
			if (retVal) {
				//debugLog<<"READY !!!!!!"<<endl;
			} else {
				//debugLog<<"NOT READY !!!!"<<endl;
			}
		}
		//=======================================================================
		if (locWasAny && retVal) {
			debugLog<<"Streams READY"<<endl;
		} else {
			debugLog<<"Streams NOT READY"<<endl;
		}
		return locWasAny && retVal;
	}
}

bool AnxStreamMapper::isAnnodexEOS(OggPage* inOggPage) {
	if ((inOggPage->header()->StreamSerialNo() == mAnnodexSerial) &&
				(inOggPage->header()->isEOS())) {
		return true;
	} else {
		return false;
	}

}
bool AnxStreamMapper::isAnnodexBOS(OggPage* inOggPage) {
	if  ((inOggPage->numPackets() != 1) ||
				(inOggPage->getPacket(0)->packetSize() < 12) ||
				(strncmp((const char*)inOggPage->getPacket(0)->packetData(), "Annodex\0", 8) != 0) || 
				(!inOggPage->header()->isBOS())) {
		return false;
	} else {
		return true;
	}
}

bool AnxStreamMapper::isFisheadBOS(OggPage* inOggPage) {
	if  ((inOggPage->numPackets() != 1) ||
				//(inOggPage->getPacket(0)->packetSize() < 12) ||
				(strncmp((const char*)inOggPage->getPacket(0)->packetData(), "fishead\0", 8) != 0) || 
				(!inOggPage->header()->isBOS())) {
		return false;
	} else {
		return true;
	}
}
bool AnxStreamMapper::isAnxDataPage(OggPage* inOggPage, bool inAnxDataAreBOS) {

	//isBOS			inAnxDataAreBOS		isValid
	//
	//true			true				true
	//true			false				false
	//false			true				false
	//false			false				true
	if  ((inOggPage->numPackets() != 1) ||
				(inOggPage->getPacket(0)->packetSize() < 12) ||
				(strncmp((const char*)inOggPage->getPacket(0)->packetData(), "AnxData\0", 8) != 0) || 
				
				( (inOggPage->header()->isBOS()) != inAnxDataAreBOS )) {
		return false;
	} else {
		return true;
	}
}
/**
 *	Returns a 32 number indicating the version... major<<16 + minor
 */
unsigned long AnxStreamMapper::getAnxVersion(OggPage* inOggPage) {
	if (isAnnodexBOS(inOggPage)) {
			//8 and 10 are the offsets into the header for version number.
			return ((iLE_Math::charArrToUShort(inOggPage->getPacket(0)->packetData() + 8)) << 16) + iLE_Math::charArrToUShort(inOggPage->getPacket(0)->packetData() + 10);
	} else if (isFisheadBOS(inOggPage)) {
			return ((iLE_Math::charArrToUShort(inOggPage->getPacket(0)->packetData() + 8)) << 16) + iLE_Math::charArrToUShort(inOggPage->getPacket(0)->packetData() + 10);
	} else {
		return 0;
	}

	

}

bool AnxStreamMapper::handleAnxVersion_2_0(OggPage* inOggPage) {
	// LEAK::: Check memory deleting.
	const bool ANXDATA_ARE_BOS = true;

	bool locTemp = false;

	vector<unsigned long>::iterator it;
	int i = 0;
	debugLog<<"handleAnxVersion_2_0 : State = "<<mDemuxState<<endl;
	switch (mDemuxState) {
		case SEEN_NOTHING:
			//debugLog<<"SEEN_NOTHING"<<endl;
			//We must find an annodex BOS page.
			if (!isAnnodexBOS(inOggPage)) {
				mDemuxState = INVALID_STATE;
				delete inOggPage;
				return false;
			} else {
				delete mAnnodexHeader;
				mAnnodexHeader = inOggPage->getPacket(0)->clone();
				mAnnodexSerial = inOggPage->header()->StreamSerialNo();
				mDemuxState = SEEN_ANNODEX_BOS;
				return true;
			}
			break;		//Can never be here.

		case SEEN_ANNODEX_BOS:
			//debugLog<<"SEEN_ANNODEX_BOS"<<endl;
			//We should be getting Anxdata headers here.
			if (!isAnxDataPage(inOggPage, ANXDATA_ARE_BOS)) {
				mDemuxState = INVALID_STATE;
				return false;
			} else {
				//This should be the CMML AnxData header.
				mSeenStreams.clear();
				mAnxDataHeaders.clear();
				//mSeenStreams.push_back(inOggPage->header()->StreamSerialNo());
				mAnxDataHeaders.push_back(inOggPage->getPacket(0)->clone());
				mDemuxState = SEEN_AN_ANXDATA;

				
				//Add the CMML Stream
				OggStream* locStream = new CMMLStream(inOggPage, mOwningFilter, true);	//The page is only given for viewing
				if (locStream != NULL) {
					//debugLog<<"Adding CMML Stream"<<endl;
					mStreamList.push_back(locStream);
				}

				return true;
			}
				

			break;		//Can never be here.

		case SEEN_AN_ANXDATA:
			//debugLog<<"SEEN_AN_ANXDATA"<<endl;
			if (isAnnodexEOS(inOggPage)) {
				//This is the end of the stream headers.
				mDemuxState = OGG_STATE;
				//debugLog<<"Found an Annodex EOS... transitioning."<<endl;
				return true;
			} else if (isAnxDataPage(inOggPage, ANXDATA_ARE_BOS)) {
				//handle another anxdata page.
				//debugLog<<"Found another anxdata..."<<endl;
				mSeenStreams.push_back(inOggPage->header()->StreamSerialNo());
				mAnxDataHeaders.push_back(inOggPage->getPacket(0)->clone());
				mDemuxState = SEEN_AN_ANXDATA;
				return true;
			} else {
				mDemuxState = INVALID_STATE;
				return false;
				//Invalid.
			}
			break;
		case OGG_STATE:
			//debugLog<<"OGG_STATE"<<endl;
			//We've seen the annodex EOS... so we can proceed as if it's a normal ogg file.
			// The CMML stream is already made.

			for(i = 0, it = mSeenStreams.begin(); it != mSeenStreams.end(); i++, ++it) {
				debugLog<<"handleAnxVersion_2_0 : Checking seen stream "<<i<<endl;
				if (mSeenStreams[i] == inOggPage->header()->StreamSerialNo()) {
					//If the page is a BOS we need to start a new stream
					const bool ALLOW_OTHERS_TO_SEEK = true;
					debugLog<<"handleAnxVersion_2_0 : Creating stream "<<endl;
					OggStream* locStream = OggStreamFactory::CreateStream(inOggPage, mOwningFilter, ALLOW_OTHERS_TO_SEEK);
					//FIX::: Need to check for NULL
					if (locStream != NULL) {
						debugLog<<"handleAnxVersion_2_0 : Stream Created "<<endl;
						mStreamList.push_back(locStream);
					} else {
						debugLog<<"handleAnxVersion_2_0 : ***** Stream NOT Created *****"<<endl;
					}
					mSeenStreams.erase(it);
					delete inOggPage;
					return true;
				}
			}

			//If we are here, the stream is not in the list.
			//At the moment we assume it's because it's been seen, and removed... this is a bad assumption !
			debugLog<<"handleAnxVersion_2_0 : Dispatching page "<<endl;

			locTemp = dispatchPage(inOggPage);
			if (locTemp) {
				debugLog<<"handleAnxVersion_2_0 : Sispatch oK "<<endl;
			} else {
				debugLog<<"handleAnxVersion_2_0 : Dispatch faild "<<endl;
			}
			return locTemp;
			break;
		case INVALID_STATE:
		default:
			debugLog<<"handleAnxVersion_2_0 : INVALID STATE "<<endl;
			return false;
			break;



	}
}

bool AnxStreamMapper::handleAnxVersion_3_0(OggPage* inOggPage) 
{
	mDemuxState = OGG_STATE;
	return OggStreamMapper::acceptOggPage(inOggPage);
	//switch (mDemuxState) {
	//	case SEEN_NOTHING:
	//		//We must find an annodex BOS page.
	//		break;
	//	case SEEN_ANNODEX_BOS:
	//		break;
	//	case SEEN_AN_ANXDATA:
	//		break;
	//	case OGG_STATE:
	//		break;
	//	case INVALID_STATE:
	//	default:
	//		break;



	//}
	//return false;
}

bool AnxStreamMapper::acceptOggPage(OggPage* inOggPage)			//Deletes or gives away page.
{

	if (mDemuxState == SEEN_NOTHING) {
		mAnxVersion = getAnxVersion(inOggPage);
		//debugLog<<"Version is "<<mAnxVersion<<endl;
	}
	//debugLog<<"Accepting Page..."<<endl;
	switch (mAnxVersion) {
		case ANX_VERSION_2_0:
			//Potential memory leaks here !
			
			return handleAnxVersion_2_0(inOggPage);
			break;
		case ANX_VERSION_3_0:
			debugLog<<"handleAnxVersion_3_0 "<<endl;
			return handleAnxVersion_3_0(inOggPage);
		default:
			mDemuxState = INVALID_STATE;
			return false;

	}



	//debugLog<<endl<<"-------- accepting page ------"<<endl;
	//if (!mReadyForCodecs) {
	//	if (inOggPage->header()->isBOS()) {
	//		if (!mSeenAnnodexBOS) {
	//			//This is an annodex BOS page
	//			//Need to verify it's an annodex page too
	//			mAnnodexSerial = inOggPage->header()->StreamSerialNo();
	//			mSeenAnnodexBOS = true;
	//			
	//			//Need to save the data from the header here.
	//		} else {
	//			//This is anxdata
	//			

	//			if ( (mSeenCMML == false) ) {
	//				//This is a really nasty way to do it ! Fix this up properly later.
	//				char* locStr = (char*)(inOggPage->getPacket(0)->packetData() + 28);
	//				if (strstr(locStr, "text/x-cmml") != NULL) {
	//					mSeenCMML = true;
	//					OggStream* locStream = new CMMLStream(inOggPage, mOwningFilter, true);	//The page is only given for viewing
	//					if (locStream != NULL) {
	//						mStreamList.push_back(locStream);
	//					}
	//				}
	//			} else {
	//			//Need to save header data here.
	//				mSeenStreams.push_back(inOggPage->header()->StreamSerialNo());
	//			}
	//		}
	//	}


	//	if (inOggPage->header()->isEOS()) {
	//		if (mSeenAnnodexBOS) {
	//			//This is the annodex EOS page
	//			mReadyForCodecs = true;
	//		} else {
	//			//ERROR... got an EOS before we've seen the annodex BOS
	//			delete inOggPage;
	//			return false;
	//		}
	//	}
	//	delete inOggPage;
	//	return true;
	//} else {
	//	vector<unsigned long>::iterator it;
	//	int i;
	//	for(i = 0, it = mSeenStreams.begin(); it != mSeenStreams.end(); i++, ++it) {
	//	//for (int i = 0; i < mSeenStreams.size(); i++) {
	//		if (mSeenStreams[i] == inOggPage->header()->StreamSerialNo()) {
	//			//If the page is a BOS we need to start a new stream
	//			const bool ALLOW_OTHERS_TO_SEEK = true;
	//			OggStream* locStream = OggStreamFactory::CreateStream(inOggPage, mOwningFilter, ALLOW_OTHERS_TO_SEEK);
	//			//FIX::: Need to check for NULL
	//			if (locStream != NULL) {
	//				mStreamList.push_back(locStream);
	//			}
	//			mSeenStreams.erase(it);
	//			delete inOggPage;
	//			return true;
	//		}
	//	}

	//	//If we are here, the stream is not in the list.
	//	//At the moment we assume it's because it's been seen, and removed... this is a bad assumption !

	//	return dispatchPage(inOggPage);
	//}
}

bool AnxStreamMapper::toStartOfData() {

	if (mAnxVersion == ANX_VERSION_3_0) {
		return OggStreamMapper::toStartOfData();
	} else {
		debugLog<<"toStartOfData : S "<<endl;
		//Specialise for anx... adds one extra ignore packet to the flush to account for the anxdata pages only
		// if it's 2.0 version.
		//debugLog<<"ANX::: To start of data size = "<<mStreamList.size()<<endl;
		if (isReady()) {  //CHECK::: Should check for allow dsipatch ???
			for (unsigned long i = 0; i < mStreamList.size(); i++) {
				//Flush each stream, then ignore the codec headers.
				if (mAnxVersion == ANX_VERSION_2_0) {
					//debugLog<<"Flushing stream "<<i<<" for "<<mStreamList[i]->numCodecHeaders() + 1<<endl;
					mStreamList[i]->flush((unsigned short)(mStreamList[i]->numCodecHeaders() + 1));  //+1 = AnxData Header...
				} else {
					mStreamList[i]->flush((unsigned short)(mStreamList[i]->numCodecHeaders()));
				}
			}	
			return true;
		} else {
			//debugLog<<"Something bad happened !!!!&&&&"<<endl;
			return false;
		}
	}
}
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
#include "oggpaginator.h"


//Checksum tables from libogg
static unsigned long crc_lookup[256]={
  0x00000000,0x04c11db7,0x09823b6e,0x0d4326d9,
  0x130476dc,0x17c56b6b,0x1a864db2,0x1e475005,
  0x2608edb8,0x22c9f00f,0x2f8ad6d6,0x2b4bcb61,
  0x350c9b64,0x31cd86d3,0x3c8ea00a,0x384fbdbd,
  0x4c11db70,0x48d0c6c7,0x4593e01e,0x4152fda9,
  0x5f15adac,0x5bd4b01b,0x569796c2,0x52568b75,
  0x6a1936c8,0x6ed82b7f,0x639b0da6,0x675a1011,
  0x791d4014,0x7ddc5da3,0x709f7b7a,0x745e66cd,
  0x9823b6e0,0x9ce2ab57,0x91a18d8e,0x95609039,
  0x8b27c03c,0x8fe6dd8b,0x82a5fb52,0x8664e6e5,
  0xbe2b5b58,0xbaea46ef,0xb7a96036,0xb3687d81,
  0xad2f2d84,0xa9ee3033,0xa4ad16ea,0xa06c0b5d,
  0xd4326d90,0xd0f37027,0xddb056fe,0xd9714b49,
  0xc7361b4c,0xc3f706fb,0xceb42022,0xca753d95,
  0xf23a8028,0xf6fb9d9f,0xfbb8bb46,0xff79a6f1,
  0xe13ef6f4,0xe5ffeb43,0xe8bccd9a,0xec7dd02d,
  0x34867077,0x30476dc0,0x3d044b19,0x39c556ae,
  0x278206ab,0x23431b1c,0x2e003dc5,0x2ac12072,
  0x128e9dcf,0x164f8078,0x1b0ca6a1,0x1fcdbb16,
  0x018aeb13,0x054bf6a4,0x0808d07d,0x0cc9cdca,
  0x7897ab07,0x7c56b6b0,0x71159069,0x75d48dde,
  0x6b93dddb,0x6f52c06c,0x6211e6b5,0x66d0fb02,
  0x5e9f46bf,0x5a5e5b08,0x571d7dd1,0x53dc6066,
  0x4d9b3063,0x495a2dd4,0x44190b0d,0x40d816ba,
  0xaca5c697,0xa864db20,0xa527fdf9,0xa1e6e04e,
  0xbfa1b04b,0xbb60adfc,0xb6238b25,0xb2e29692,
  0x8aad2b2f,0x8e6c3698,0x832f1041,0x87ee0df6,
  0x99a95df3,0x9d684044,0x902b669d,0x94ea7b2a,
  0xe0b41de7,0xe4750050,0xe9362689,0xedf73b3e,
  0xf3b06b3b,0xf771768c,0xfa325055,0xfef34de2,
  0xc6bcf05f,0xc27dede8,0xcf3ecb31,0xcbffd686,
  0xd5b88683,0xd1799b34,0xdc3abded,0xd8fba05a,
  0x690ce0ee,0x6dcdfd59,0x608edb80,0x644fc637,
  0x7a089632,0x7ec98b85,0x738aad5c,0x774bb0eb,
  0x4f040d56,0x4bc510e1,0x46863638,0x42472b8f,
  0x5c007b8a,0x58c1663d,0x558240e4,0x51435d53,
  0x251d3b9e,0x21dc2629,0x2c9f00f0,0x285e1d47,
  0x36194d42,0x32d850f5,0x3f9b762c,0x3b5a6b9b,
  0x0315d626,0x07d4cb91,0x0a97ed48,0x0e56f0ff,
  0x1011a0fa,0x14d0bd4d,0x19939b94,0x1d528623,
  0xf12f560e,0xf5ee4bb9,0xf8ad6d60,0xfc6c70d7,
  0xe22b20d2,0xe6ea3d65,0xeba91bbc,0xef68060b,
  0xd727bbb6,0xd3e6a601,0xdea580d8,0xda649d6f,
  0xc423cd6a,0xc0e2d0dd,0xcda1f604,0xc960ebb3,
  0xbd3e8d7e,0xb9ff90c9,0xb4bcb610,0xb07daba7,
  0xae3afba2,0xaafbe615,0xa7b8c0cc,0xa379dd7b,
  0x9b3660c6,0x9ff77d71,0x92b45ba8,0x9675461f,
  0x8832161a,0x8cf30bad,0x81b02d74,0x857130c3,
  0x5d8a9099,0x594b8d2e,0x5408abf7,0x50c9b640,
  0x4e8ee645,0x4a4ffbf2,0x470cdd2b,0x43cdc09c,
  0x7b827d21,0x7f436096,0x7200464f,0x76c15bf8,
  0x68860bfd,0x6c47164a,0x61043093,0x65c52d24,
  0x119b4be9,0x155a565e,0x18197087,0x1cd86d30,
  0x029f3d35,0x065e2082,0x0b1d065b,0x0fdc1bec,
  0x3793a651,0x3352bbe6,0x3e119d3f,0x3ad08088,
  0x2497d08d,0x2056cd3a,0x2d15ebe3,0x29d4f654,
  0xc5a92679,0xc1683bce,0xcc2b1d17,0xc8ea00a0,
  0xd6ad50a5,0xd26c4d12,0xdf2f6bcb,0xdbee767c,
  0xe3a1cbc1,0xe760d676,0xea23f0af,0xeee2ed18,
  0xf0a5bd1d,0xf464a0aa,0xf9278673,0xfde69bc4,
  0x89b8fd09,0x8d79e0be,0x803ac667,0x84fbdbd0,
  0x9abc8bd5,0x9e7d9662,0x933eb0bb,0x97ffad0c,
  0xafb010b1,0xab710d06,0xa6322bdf,0xa2f33668,
  0xbcb4666d,0xb8757bda,0xb5365d03,0xb1f740b4
};

  //End libogg tables

OggPaginator::OggPaginator(void)
	:	mPageCallback(NULL)
	,	mSettings(NULL)
	,	mPendingPage(NULL)
	,	mCurrentPageSize(0)
	,	mSegmentTableSize(0)
	,	mPendingPageHasData(false)
	,	mSequenceNo(0)
	,	mPacketCount(0)
{
	debugLog.open("G:\\logs\\paginator.log", ios_base::out);
	
}

OggPaginator::~OggPaginator(void)
{
	debugLog.close();
}


bool OggPaginator::setParameters(OggPaginatorSettings* inSettings) {
	delete mSettings;
    mSettings = inSettings;
	createFreshPage();
	return true;
}

bool OggPaginator::acceptStampedOggPacket(StampedOggPacket* inOggPacket) {
	//unsigned long locPotentialSize = mCurrentPageSize + inOggPacket->packetSize();
	//bool locCouldWriteNow = false;

	//if (mCurrentPageSize >= mSettings->mMinPageSize) {
	//	locCouldWriteNow = true;
	//}

	//if (locPotentialSize > mSettings->mMaxPageSize) {
	//	if (locCouldWriteNow) {
	//		//CASE 1 : New page would be bigger than max but current size is greater than min
	//		//
	//		//Write page
	//		//Start a new page
	//		//Put this packet into page
	//	} else {
	//		//CASE 2 : New page would be bigger than max and current is smaller than min
	//		//
	//		//Cut to target size
	//		//Write page
	//		//Start new page
	//		//Put remainder into page
	//	}
	//} else {
	//	if (locPotentialSize >= mSettings->mMinPageSize) {
	//		//CASE 3 : New page is smaller than max and bigger than min, ie ready to go.
	//		//
	//		//Put this packet into page
	//		//Write page
	//		//Start new page

	//	} else {
	//		//CASE 4 : New page is smaller than both min and max... not enough data yet
	//		//
	//		//Put this packet into page

	//	}
	//}

	debugLog<<"Accepting packet"<<endl;
	addPacketToPage(inOggPacket);

	return true;
}
bool OggPaginator::finishStream() {
	mPendingPage->header()->setHeaderFlags(mPendingPage->header()->HeaderFlags() | 4);
	deliverCurrentPage();
	return true;
}

bool OggPaginator::setChecksum() {
		unsigned long locChecksum = 0;
		unsigned long locTemp = 0;


	   //ogg_uint32_t crc_reg=0;
    //int i;

    ///* safety; needed for API behavior, but not framing code */
    //og->header[22]=0;
    //og->header[23]=0;
    //og->header[24]=0;
    //og->header[25]=0;
    //
    //for(i=0;i<og->header_len;i++)
    //  crc_reg=(crc_reg<<8)^crc_lookup[((crc_reg >> 24)&0xff)^og->header[i]];
    //for(i=0;i<og->body_len;i++)
    //  crc_reg=(crc_reg<<8)^crc_lookup[((crc_reg >> 24)&0xff)^og->body[i]];
    //
    //og->header[22]=crc_reg&0xff;
    //og->header[23]=(crc_reg>>8)&0xff;
    //og->header[24]=(crc_reg>>16)&0xff;
    //og->header[25]=(crc_reg>>24)&0xff;
	if (mPendingPage != NULL) {
	
		mPendingPage->header()->setCRCChecksum((unsigned long)0);

		unsigned char* locBuff = new unsigned char[300];
		mPendingPage->header()->rawData(locBuff, 300);

		for(unsigned long i = 0; i < mPendingPage->headerSize(); i++) {
			//Create the index we use for the lookup
			locTemp = ((locChecksum >> 24) & 0xff) ^ locBuff[i];
			//XOR the lookup value with the the current checksum shifted left 8 bits.
			locChecksum=(locChecksum << 8) ^ crc_lookup[locTemp];
		}

		delete[] locBuff;
		locBuff = NULL;

		for(unsigned long i = 0; i < mPendingPage->numPackets(); i++) {
			locBuff = mPendingPage->getPacket(i)->packetData();

			for (unsigned long j = 0; j < mPendingPage->getPacket(i)->packetSize(); j++) {
				locTemp = ((locChecksum >> 24) & 0xff) ^ locBuff[j];
                locChecksum = (locChecksum << 8) ^ crc_lookup[locTemp];
			}
		}

		mPendingPage->header()->setCRCChecksum(locChecksum);
	}
	return true;

}
bool OggPaginator::deliverCurrentPage() {
	debugLog<<"Delivering page"<<endl;
	mPendingPage->header()->setSegmentTable((const unsigned char*)mSegmentTable, mSegmentTableSize);
	mPendingPage->header()->setDataSize(mCurrentPageSize - mPendingPage->headerSize());  //This is odd


	//mPendingPage->header()->setHeaderSize(OggPageHeader::OGG_BASE_HEADER_SIZE + mSegmentTableSize);
	//mPendingPage->header()->setNumPageSegments(mSegmentTableSize);
	
	//if (mPendingPage->header()->GranulePos()->value() == -1) {
	//	mPendingPage->header()->setHeaderFlags(mPendingPage->header()->HeaderFlags() | 1);	
	//}
	setChecksum();
	
	//TODO::: Should catch and propagate return value.
	mPageCallback->acceptOggPage(mPendingPage);
	createFreshPage();
	return true;

}
bool OggPaginator::createFreshPage() {
	debugLog<<"Creating fresh page"<<endl;
	mPendingPage = new OggPage;
	mCurrentPageSize = OggPageHeader::OGG_BASE_HEADER_SIZE;
	mPendingPageHasData = false;
	mSegmentTableSize = 0;
	//mPendingPage->header()->setStructureVersion(0);
	mPendingPage->header()->setStreamSerialNo(mSettings->mSerialNo);
	//mPendingPage->header()->setDataSize(0);
	mPendingPage->header()->setPageSequenceNo(mSequenceNo);
	//mPendingPage->header()->setCRCChecksum((unsigned long)0);
	if (mSequenceNo == 0) {
		mPendingPage->header()->setHeaderFlags(2);
	} else {
		mPendingPage->header()->setHeaderFlags(0);
	}

	mPendingPage->header()->setGranulePos(-1);
	mSequenceNo++;
	return true;

}
bool OggPaginator::addPacketToPage(StampedOggPacket* inOggPacket) {

	debugLog<<"Add packet to page"<<endl;
	mPendingPageHasData = true;
	//while some packet left
	//	add as much as possible
	//	update how much packet is left
	//wend


	//ADD AS MUCH AS POSSIBLE
	//howmuchToAdd = MIN(numSegsLeft * 255, maxsize - current size)
	//add part of packet
	//if page has enough deliver it
	//return how much was added.

	//Get some counter variables ready
	long locPacketRemaining = inOggPacket->packetSize();
	unsigned long locPacketStartPoint = 0;
	unsigned long locConsumed = 0;

	//While there is still more packet not added to the page
	while (locPacketRemaining > 0) {
		debugLog<<"Packet remaining = "<<locPacketRemaining<<endl;
		locConsumed = addAsMuchPacketAsPossible(inOggPacket, locPacketStartPoint, locPacketRemaining);
		debugLog<<"Consumed = "<<locConsumed<<endl;
		locPacketStartPoint += locConsumed;
		locPacketRemaining -= locConsumed;
	}

	//To ensure you get vorbis comments and codebook ending a page.
	//if ((mPacketCount == 2) && (mPendingPageHasData)) {
	//	deliverCurrentPage();
	//}

	//This will ensure that any header packet appears on it's own page...
	//
	// An added benefit, is that comment packets appear on their own page, this makes it
	// significantly easier to add/modify comments without bumping data across a page which could
	// require changing of all the headers in all the pages.
	if ((mPacketCount < mSettings->mNumHeaders) && (mPendingPageHasData)) {
		debugLog<<"Flushing a header page..."<<endl;
		debugLog<<"PacketCount = "<<mPacketCount<<endl;
		debugLog<<"Num Headers = "<<mSettings->mNumHeaders<<endl;
		deliverCurrentPage();
	}
	mPacketCount++;
	return true;
}


unsigned long OggPaginator::addAsMuchPacketAsPossible(StampedOggPacket* inOggPacket, unsigned long inStartAt, long inRemaining) {
	//Take 1 so when it adds the packet it doesn't try to consume one extra segment which doesn't exist.

	//The amount of space left in the page is the minimum of
	// a) The number of segments left * 255
	// b) The number of bytes less than the desired maximum page size.

	debugLog<<"Remains in packet = "<<inRemaining<<endl;
	debugLog<<"Start at = "<<inStartAt<<endl;
	debugLog<<"Segtable size = "<<mSegmentTableSize<<endl;
	debugLog<<"Max page size = "<<mSettings->mMaxPageSize<<endl;
	debugLog<<"Current page size = "<<mCurrentPageSize<<endl;

    unsigned long locSpaceLeft =	MIN(((255 - mSegmentTableSize) * 255) - 1, mSettings->mMaxPageSize - mCurrentPageSize);

	debugLog<<"Space left = "<<locSpaceLeft<<endl;
	//debugLog<<"Space left = "<<locSpaceLeft<<endl;
	//Round down to nearest multiple of 255
	//
	//This is important when the packet gets broken because inRemaining is gt locSpace left
	// In this case where the packet gets broken the final segment on the page must be 255.
	locSpaceLeft -= (locSpaceLeft % 255);
	debugLog<<"Adjust space left = "<<locSpaceLeft<<endl;

	//How much we add is the minimum of
	// a) How much space is left
	// b) The amount of packet remaining.

	//If (a) is the minimum then we know that the how much we are adding is a multiple of 255.
	unsigned long locHowMuchToAdd = MIN(locSpaceLeft, inRemaining);

	debugLog<<"How much to add..."<<endl;
	//debugLog<<"How much to add = "<<locHowMuchToAdd<<endl;
	
	//mPending page has data is useless, it was set before this function is called... need to fix that. maybe move into add part of pack into apge
	if ((!mPendingPageHasData) && (inStartAt != 0)) {
		mPendingPage->header()->setHeaderFlags(mPendingPage->header()->HeaderFlags() | 1);	
	}

	if (locHowMuchToAdd > 0) {
		addPartOfPacketToPage(inOggPacket, inStartAt, locHowMuchToAdd);
	}


	//This puts only a single packet on the first page...
	if ((mCurrentPageSize >= mSettings->mMinPageSize) || (mPendingPage->header()->PageSequenceNo() == 0) || (locHowMuchToAdd == 0)) {
		deliverCurrentPage();
	}
	return locHowMuchToAdd;

}

bool OggPaginator::addPartOfPacketToPage(StampedOggPacket* inOggPacket, unsigned long inStartFrom, unsigned long inLength) {

	//debugLog<<"Add part of packet to page"<<endl;

	//Buffer the amount of the packet we are going to add.
	unsigned char* locBuff = new unsigned char[inLength];
	memcpy((void*)locBuff, (const void*)(inOggPacket->packetData() + inStartFrom), inLength);

	//unsigned long locBytesOfPacketRemaining = inOggPacket->packetSize() - (((locNumSegsNeeded - (255 - mSegmentTableSize)) * 255);
	//unsigned long locRemainingPacketStartsAt = inOggPacket->packetSize() - locBytesOfPacketRemaining + 1;
	//

	//Its the last part of the packet start point plus how much we are adding of it is the same
	// as the total packet size.
	bool locIsLastOfPacket = (inStartFrom + inLength == inOggPacket->packetSize());

	//Create a new packet
	StampedOggPacket* locPartialPacket = new StampedOggPacket(	locBuff, 
																inLength, 
																locIsLastOfPacket,
																false,   //Not continuation
																inOggPacket->startTime(), 
																inOggPacket->endTime(), 
																inOggPacket->mStampType);

	//Add the packet to the page.
	mPendingPage->addPacket(locPartialPacket);

	//CASES
	//========
	//length                 segs           segs if not end
	//  0                     1                   N/A
	//  1                     1                   N/A
	// 255                    2                    1
	// 256                    2
	// 510                    3                    2
	//  n                     (n / 255) + 1

	//Now do the segment table bookkeeping.
	unsigned long locNumSegsNeeded = (inLength / 255);

	//debugLog<<"Amount to add = "<<inLength<<endl;
	//debugLog<<"Segs needed = "<<locNumSegsNeeded<<endl;

	//Always do one less than the total... the last segment is a special case
	//We fill all but the last segemnt with 255
	for (unsigned long i = 0; i < locNumSegsNeeded; i++) {
		mSegmentTable[mSegmentTableSize] = 255;
		mSegmentTableSize++;
	}

	//If it's not the last of the packet, it will be a multiple of 255, hence we don't put a terminating 0 on.
	//If it is the last of the packet this last segment will be between 0 and 254 inclusive.
	if (locIsLastOfPacket) {
		//Its the last part of the packet... so we need one extra segemnt... to hold the last part.
		// The last part will be between 0-254
		//debugLog<<"Is last of packet... adding terminator "<<(unsigned long)(inLength % 255)<<endl;
		mSegmentTable[mSegmentTableSize] = (unsigned char)(inLength % 255);
		mSegmentTableSize++;

		//This is used in a calculation below.
		locNumSegsNeeded++;
	} else {
		//If it's not the last part of the packet it should be a multiple of 255. the calling function needs to ensure this.
		//Since if it was the last part of the packet we've already added all the segments, then we do nothing.

		//ASSERT((inLength % 255) == 0);
		//if ((inLength % 255) != 0) {
		//	debugLog<<"ASSERTION FAILED !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endl;
		//}
		//mSegmentTable[mSegmentTableSize] = (unsigned char)(255);
		//mSegmentTableSize++;
		//locNumSegsNeeded++;
	}
		

	//debugLog<<"Seg table is "<<(unsigned long)mSegmentTableSize<<endl;

	mCurrentPageSize += (locNumSegsNeeded + inLength);

	//debugLog<<"Page is now "<<mCurrentPageSize<<endl;
	if (locIsLastOfPacket) {
		//debugLog<<"Setting time for end of page"<<endl;
		mPendingPage->header()->setGranulePos(inOggPacket->endTime());
	}

	return true;
}




	
//	unsigned long locNumSegsNeeded = ((inOggPacket->packetSize()) / 255) + 1;
//	if (locNumSegsNeeded + mSegmentTableSize > 255) {
//		//Must split the packet... not enough segments
//		
//
//		
//		deliverCurrentPage();
//		createFreshPage();
//		addPartOfPacketToPage(inOggPacket, 0, locRemainingPacketStartsAt);
//		
//
//
//	} else {
//		//Just add the packet
//		mPendingPage->addPacket(inOggPacket->clone());
//		for (int i = 0; i < locNumSegsNeeded - 1; i++) {
//			mSegmentTable[mSegmentTableSize] = 255;
//			mSegmentTableSize++;
//		}
//		mSegmentTableSize++;
//		mSegmentTable[mSegmentTableSize] = (inOggPacket->packetSize() % 255);
//		mCurrentPageSize += (locNumSegsNeeded + inOggPacket->packetSize());
//		OggInt64* locGranulePos = new OggInt64;
//		locGranulePos->setValue(inOggPacket->endTime());
//		mPendingPage->header()->setGranulePos(locGranulePos);
//	}
//	
//}


bool OggPaginator::setPageCallback(IOggCallback* inPageCallback) {
	mPageCallback = inPageCallback;
	return true;
}

void OggPaginator::setNumHeaders(unsigned long inNumHeaders) {
	mSettings->mNumHeaders = inNumHeaders;
}
unsigned long OggPaginator::numHeaders() {
	return mSettings->mNumHeaders;

}


//===========================================================================
//Copyright (C) 2003, 2004 Zentaro Kavanagh
//
//Copyright (C) 2004 Commonwealth Scientific and Industrial Research
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
#include ".\fishskeleton.h"

FishSkeleton::FishSkeleton(void)
{
}

FishSkeleton::~FishSkeleton(void)
{
}

StampedOggPacket* FishSkeleton::makeCMMLBOS()
{
	unsigned char* locPackBuff = new unsigned char[8];
	locPackBuff[0] = 'C';
	locPackBuff[1] = 'M';
	locPackBuff[2] = 'M';
	locPackBuff[3] = 'L';
	locPackBuff[4] = 3;
	locPackBuff[5] = 0;
	locPackBuff[6] = 0;
	locPackBuff[7] = 0;

	StampedOggPacket* locPacket = new StampedOggPacket(locPackBuff, 8, false, false, 0, 0, StampedOggPacket::OGG_BOTH);
	return locPacket;
}
OggPage* FishSkeleton::makeFishHeadBOS_3_0	(		unsigned long inSerialNo
												,	unsigned short inVersionMajor
												,	unsigned short inVersionMinor
												,	unsigned __int64 inPresentTimeNum
												,	unsigned __int64 inPresentTimeDenom
												,	unsigned __int64 inTimebaseNum
												,	unsigned __int64 inTimebaseDenom
												,	const unsigned char* inUTC
														
											)
{
	unsigned char* locBuff = NULL;
	StampedOggPacket* locPack = NULL;
	OggPage* retPage = NULL;
	unsigned char* locSegTable = NULL;
	switch (inVersionMajor) {
		case 3:
			locBuff = new unsigned char[FishSkeleton::FISHEAD_3_0_PACKET_SIZE];

			//	0	-	7		fishead\0
			//  8	-   9		Version Major
			// 10	-  11		Version Minor
			// 12	-  19		Presentation Num
			// 20	-  27		Presentation Denom
			// 28	-  35		Basetime Num
			// 36	-  43	
			// 44	-  63		UTC
			locBuff[0] = 'f';
			locBuff[1] = 'i';
			locBuff[2] = 's';
			locBuff[3] = 'h';
			locBuff[4] = 'e';
			locBuff[5] = 'a';
			locBuff[6] = 'd';
			locBuff[7] = 0;
			iLE_Math::UShortToCharArr(inVersionMajor, locBuff + 8);
			iLE_Math::UShortToCharArr(inVersionMinor, locBuff + 10);
			iLE_Math::Int64ToCharArr(inPresentTimeNum, locBuff + 12);
			iLE_Math::Int64ToCharArr(inPresentTimeDenom, locBuff + 20);
			iLE_Math::Int64ToCharArr(inTimebaseNum, locBuff + 28);
			iLE_Math::Int64ToCharArr(inTimebaseDenom, locBuff + 36);
			for (int i = 0; i < 20; i++) {
				locBuff[44 + i] = inUTC[i];
			}
			
			locPack = new StampedOggPacket(locBuff, FishSkeleton::FISHEAD_3_0_PACKET_SIZE, false, false, 0, 0, StampedOggPacket::OGG_END_ONLY);

			retPage = new OggPage;
			retPage->header()->setHeaderFlags(2);
			retPage->header()->setGranulePos((__int64)0);
			retPage->header()->setNumPageSegments( 1);
			locSegTable = new unsigned char[1];
			locSegTable[0] = FishSkeleton::FISHEAD_3_0_PACKET_SIZE;
			retPage->header()->setSegmentTable(locSegTable, 1);
			retPage->header()->setHeaderSize(28);
			retPage->header()->setDataSize(FishSkeleton::FISHEAD_3_0_PACKET_SIZE);

			retPage->header()->setStreamSerialNo(inSerialNo);
			retPage->addPacket(locPack);
			
			return retPage;

		default:
			return NULL;


	}
}

OggPage* FishSkeleton::makeFishEOS (unsigned long inSerialNo) {
	OggPage* retPage = new OggPage();
	StampedOggPacket* locDudPacket = new StampedOggPacket(NULL, 0, false, false, 0, 0, StampedOggPacket::OGG_BOTH);



	retPage->header()->setNumPageSegments(1);
	unsigned char* locSegTable = new unsigned char[1];

	locSegTable[0] = 0;
	

	retPage->header()->setHeaderFlags(4);
	retPage->header()->setSegmentTable(locSegTable, 1);
	retPage->header()->setHeaderSize(28);
	retPage->header()->setDataSize(0);

	retPage->header()->setStreamSerialNo(inSerialNo);
	retPage->addPacket(locDudPacket);

	return retPage;

}
OggPage* FishSkeleton::makeFishBone_3_0_Page (StampedOggPacket* inFishBonePacket, unsigned long inFishStreamSerial)
{
			StampedOggPacket* locPack = inFishBonePacket;
			OggPage* retPage = new OggPage;
			retPage->header()->setHeaderFlags(0);
			retPage->header()->setGranulePos((__int64)0);
			
			unsigned long locDataSize = locPack->packetSize();

			

			unsigned long locNumSegs = ((locDataSize / 255) + 1);

			unsigned char locLastSeg = locDataSize % 255;

			retPage->header()->setNumPageSegments(locNumSegs);
			unsigned char* locSegTable = new unsigned char[locNumSegs];

			for (int i = 0; i < locNumSegs - 1; i++) {
				locSegTable[i] = 255;
			}

			locSegTable[locNumSegs - 1] = locLastSeg;
			
			retPage->header()->setSegmentTable(locSegTable, locNumSegs);
			retPage->header()->setHeaderSize(27 + locNumSegs);
			retPage->header()->setDataSize(locDataSize);

			retPage->header()->setStreamSerialNo(inFishStreamSerial);
			retPage->addPacket(locPack);

			
			return retPage;
}



//OggPage* FishSkeleton::makeFishBone_3_0_Page			(		unsigned __int64 inGranuleRateNum
//														,	unsigned __int64 inGranuleRateDenom
//														,	unsigned __int64 inBaseGranule
//														,	unsigned long inNumSecHeaders
//														,	unsigned long inSerialNo
//														,	unsigned short inGranuleShift
//														,	unsigned char inPreroll
//														,	vector<string> inMessageHeaders
//														,	unsigned long inFishStreamSerial
//													)
//{
//			StampedOggPacket* locPack = makeFishBone_3_0(inGranuleRateNum, inGranuleRateDenom, inBaseGranule, inNumSecHeaders, inSerialNo, inGranuleShift, inPreroll,inMessageHeaders);
//			OggPage* retPage = new OggPage;
//			retPage->header()->setHeaderFlags(0);
//			retPage->header()->setGranulePos((__int64)0);
//			
//			unsigned long locDataSize = locPack->packetSize();
//
//			
//
//			unsigned long locNumSegs = ((locDataSize / 255) + 1);
//
//			unsigned char locLastSeg = locDataSize % 255;
//
//			retPage->header()->setNumPageSegments(locNumSegs);
//			unsigned char* locSegTable = new unsigned char[locNumSegs];
//
//			for (int i = 0; i < locNumSegs - 1; i++) {
//				locSegTable[i] = 255;
//			}
//
//			locSegTable[locNumSegs - 1] = locLastSeg;
//			
//			retPage->header()->setSegmentTable(locSegTable, locNumSegs);
//			retPage->header()->setHeaderSize(27 + locNumSegs);
//			retPage->header()->setDataSize(locDataSize);
//
//			retPage->header()->setStreamSerialNo(inFishStreamSerial);
//			retPage->addPacket(locPack);
//
//			return retPage;
//}
StampedOggPacket* FishSkeleton::makeFishBone_3_0	(		unsigned __int64 inGranuleRateNum
														,	unsigned __int64 inGranuleRateDenom
														,	unsigned __int64 inBaseGranule
														,	unsigned long inNumSecHeaders
														,	unsigned long inSerialNo
														,	unsigned char inGranuleShift
														,	unsigned long inPreroll
														,	vector<string> inMessageHeaders
													) 
{

	//	0	-	7		:	fisbone/0
	//	8	-	11		:	Bytes to message headers
	//	12	-	15		:	Serial NO
	//	16	-	19		:	Num Headers
	//	20	-	27		:	Granule Numerator
	//	28	-	35		:	Granule Denominator
	//	36	-	43		:	Base granule
	//	44	-	47		:	Preroll
	//	48	-	48		:	Granule shift
	//	49	-	51		:	*** PADDING ***
	//	52	-			:	Message Headers



		unsigned long locPacketSize = 52;  //Base header size
		for (size_t i = 0; i < inMessageHeaders.size(); i++) {
			//2 is the crlf
			locPacketSize += (unsigned long)(inMessageHeaders[i].size() + 2);
		}

		//terminating crlf
		locPacketSize += 2;

		unsigned char* locBuff = new unsigned char[locPacketSize];

		locBuff[0] = 'f';
		locBuff[1] = 'i';
		locBuff[2] = 's';
		locBuff[3] = 'b';
		locBuff[4] = 'o';
		locBuff[5] = 'n';
		locBuff[6] = 'e';
		locBuff[7] = 0;

		iLE_Math::ULongToCharArr(44, locBuff + 8);
		iLE_Math::ULongToCharArr(inSerialNo, locBuff + 12);
		iLE_Math::ULongToCharArr(inNumSecHeaders, locBuff + 16);

		iLE_Math::Int64ToCharArr(inGranuleRateNum, locBuff + 20);
		iLE_Math::Int64ToCharArr(inGranuleRateDenom, locBuff + 28);

		iLE_Math::Int64ToCharArr(inBaseGranule, locBuff + 36);
		iLE_Math::ULongToCharArr(inPreroll, locBuff + 44);

		locBuff[48] = inGranuleShift;
		locBuff[49] = 0;
		locBuff[50] = 0;
		locBuff[51] = 0;

		

		
		unsigned long locUpto = 52;
		for (size_t i = 0; i < inMessageHeaders.size(); i++) {
			memcpy((void*)(locBuff + locUpto), (const void*)(inMessageHeaders[i].c_str()), inMessageHeaders[i].size());
			locUpto += (unsigned long)(inMessageHeaders[i].size());
			//TODO::: How terminated ??
			locBuff[locUpto++] = '\r';	
			locBuff[locUpto++] = '\n';
		}

		locBuff[locUpto++] = '\r';
		locBuff[locUpto++] = '\n';
		
		StampedOggPacket* locPack = new StampedOggPacket(locBuff, locPacketSize, false, false, 0, 0, StampedOggPacket::OGG_END_ONLY);
		return locPack;




}
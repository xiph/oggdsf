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
#include ".\anxpageinterleaver.h"

AnxPageInterleaver::AnxPageInterleaver(IOggCallback* inFileWriter, INotifyComplete* inNotifier, unsigned long inVersionMajor, unsigned long inVersionMinor, AnxMuxFilter* inParentFilter)
	:	OggPageInterleaver(inFileWriter, inNotifier)
	,	mParentFilter(inParentFilter)
	,	mVersionMajor(inVersionMajor)
	,	mVersionMinor(inVersionMinor)
	,	mIsAnxSetup(false)
{
	//This is the Annodex Logical Stream
	//mInputStreams.push_back(newStream());

	//Just make a random serial number
	mAnxSerialNo = 999;
}

AnxPageInterleaver::~AnxPageInterleaver(void)
{

}

void AnxPageInterleaver::addAnnodex_2_0_BOS() {
	//Find the CMML data
	//TODO::: Need to grab the data from CMML page

	unsigned char* locUTC = new unsigned char[20];
	memset((void*)locUTC, 0, 20);

	OggPage* locBOSPage =			AnxPacketMaker::makeAnnodexBOS_2_0(		mAnxSerialNo
																	,	2
																	,	0
																	,	0
																	,	0
																	,	locUTC);

	
	//Put the annodex BOS out to the file.
	mBytesWritten += locBOSPage->pageSize();
	mFileWriter->acceptOggPage(locBOSPage);
	delete locUTC;

}

void AnxPageInterleaver::addAllAnxData_2_0_BOS() {
	OggPage* locOggPage = NULL;
	for (size_t i = 0; i < mInputStreams.size() - 1; i++) {
		locOggPage = mInputStreams[i]->popFront();
		mBytesWritten += locOggPage->pageSize();
		mFileWriter->acceptOggPage(locOggPage);
		mInputStreams[i]->setNumHeaders(mInputStreams[i]->numHeaders() + 1);
	}

}

void AnxPageInterleaver::addAnnodexEOS() {
	//Make the EOS page and dump it
    
	OggPage* locEOSPage = new OggPage;

	locEOSPage->header()->setStreamSerialNo(mAnxSerialNo);
	locEOSPage->header()->setHeaderFlags(4);
	

	StampedOggPacket* locDudPacket = new StampedOggPacket(NULL, 0, false, false, 0, 0, StampedOggPacket::OGG_BOTH);

	locEOSPage->header()->setNumPageSegments(1);
	unsigned char* locSegTable = new unsigned char[1];

	locSegTable[0] = 0;
	
	
	locEOSPage->header()->setSegmentTable(locSegTable, 1);
	locEOSPage->header()->setHeaderSize(28);
	locEOSPage->header()->setDataSize(0);
	
	locEOSPage->addPacket(locDudPacket);

	AnxPacketMaker::setChecksum(locEOSPage);
	mBytesWritten += locEOSPage->pageSize();
	mFileWriter->acceptOggPage(locEOSPage);
}

//ANX3::: Need to make a new version of this which only returns true, when we actually have *ALL* the
// header for the stream not just the first one.
bool AnxPageInterleaver::gotAllHeaders() {
	//TODO::: The isActive needs to be clarified so we don't start empty streams because wasany goes to true

	bool locWasAny = false;
	bool locIsOK = true;
	for (size_t i = 0; i < mInputStreams.size(); i++) {
		if (mInputStreams[i]->isActive()) {
			
			//if ((mInputStreams[i]->peekFront() != NULL) || (!mInputStreams[i]->isActive())) {
			if (mInputStreams[i]->numAvail() > 1) {
				locWasAny = true;
				locIsOK = locIsOK && true;
			} else {
				locIsOK = false;
			}
		}
	}

	return locWasAny && locIsOK;
}

bool AnxPageInterleaver::gotAllSecondaryHeaders() {
	//TODO::: The isActive needs to be clarified so we don't start empty streams because wasany goes to true

	bool locWasAny = false;
	bool locIsOK = true;
	for (size_t i = 0; i < mInputStreams.size(); i++) {
		if (mInputStreams[i]->isActive()) {
			
			//if ((mInputStreams[i]->peekFront() != NULL) || (!mInputStreams[i]->isActive())) {
			if (mInputStreams[i]->numAvail() >= mInputStreams[i]->numHeaders()) {
				locWasAny = true;
				locIsOK = locIsOK && true;
			} else {
				locIsOK = false;
			}
		}
	}

	return locWasAny && locIsOK;
}

void AnxPageInterleaver::letsGetFishy()
{
	
	unsigned long locSerialNo = 888;
	
	//Make the fishhead
	unsigned char* locUTC = new unsigned char[20];
	memset((void*)locUTC, 0, 20);
	OggPage* locFishHead = FishSkeleton::makeFishHeadBOS_3_0(locSerialNo, mVersionMajor, mVersionMinor, 0,0,0,0,locUTC);

	//Write out the fishHead
	mFileWriter->acceptOggPage(locFishHead);

	//Write out the BOS pages
	for (size_t i = 0; i < mInputStreams.size(); i++) {
		if (mInputStreams[i]->isActive()) {
			mFileWriter->acceptOggPage(mInputStreams[i]->popFront());
		}
	}

	//Write out the fishbones.
	for (unsigned long i = 0; i < mParentFilter->GetPinCount() - 1; i++) {
		AnxMuxInputPin* locPin = (AnxMuxInputPin*)mParentFilter->GetPin(i);
		OggPage* locFishBonePage = FishSkeleton::makeFishBone_3_0_Page(locPin->mFishBonePacket, locSerialNo);
		mFileWriter->acceptOggPage(locFishBonePage);
	}

	//Write out all the secondary headers
	for (size_t stream = 0; stream < mInputStreams.size(); stream++) {
		if (mInputStreams[stream]->isActive()) {
			for (unsigned long pack = 1; pack < mInputStreams[stream]->numHeaders(); pack++) {
				OggPage* locSecHead = mInputStreams[stream]->popFront();
				mFileWriter->acceptOggPage(locSecHead);
			}
		}
	}

	//Write the fish skeleton EOS
	mFileWriter->acceptOggPage(FishSkeleton::makeFishEOS(locSerialNo));



}
void AnxPageInterleaver::processData()
{
	if ((mVersionMajor == 2) && (mVersionMinor == 0)) {
		//IF seen all headers
		//		CREATE ANNODEX BOS
		//		CREATE ANXDATA BOS's
		//		CREATE ANNODEX EOS
		//ELSE
		//		DO NOTHING
		//ENDIF
		//
	
		if (!mIsAnxSetup) {
			if (gotAllHeaders()) {
				
				mIsAnxSetup = true;
				addAnnodex_2_0_BOS();
				addAllAnxData_2_0_BOS();
				addAnnodexEOS();	
				//TODO::: Pump out these start pages.
			}
		} else {
			OggPageInterleaver::processData();
		}
	} else if ((mVersionMajor == 3) && (mVersionMinor == 0)) {
		if (!mIsAnxSetup) {
			if (gotAllSecondaryHeaders()) {
				//ANX3::: We need to make sure every stream has all of their headers
				//
				//Then we write :
				//Fishhead
				//Codec CMML BOS
				//Codec 2 BOS
				//Fishbones...
				//All other codec secondary headers...

				letsGetFishy();

				mIsAnxSetup = true;
				//addAnnodex_2_0_BOS();
				//addAllAnxData_2_0_BOS();
				//addAnnodexEOS();	

				//addFishyBits();

				//TODO::: Pump out these start pages.
			}
		} else {
			OggPageInterleaver::processData();
		}

	} else {
		throw 0;
	}
}

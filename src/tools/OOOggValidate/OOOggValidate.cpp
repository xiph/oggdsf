// OOOggValidate.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "OggValidationState.h"
#include "libOOOgg.h"
enum eOggValidationErrors {
	OVE_OK,
	OVE_STRUCTURE_VERSION_NOT_ZERO,

};

OggValidationState gValidationState;
void error_write(eOggValidationErrors inErrNo, OggPage* inOggPage, OggStreamValidationState* inStreamState) {

	string locErrorString = "";
	switch (inErrNo) {
		case OVE_OK:
			//Do nothing
			return;
			break;
		case OVE_STRUCTURE_VERSION_NOT_ZERO:
			locErrorString = "Structure version("<<(int)inOggPage->header()->StructureVersion()<<") is not 0, (RFC 3533 Sec. 6.1)";
			inStreamState->mErrorCount++;
			break;
		case OVE_MORE_THEN_ONE_BOS:
			locErrorString = "More than one BOS page in stream.";
			inStreamState->mErrorCount++;
			break;
		case OVE_MORE_THEN_ONE_EOS:
			locErrorString = "More than one EOS page in stream.";
			break;
		case OVE_BOS_PAGE_NOT_AT_START:
			locErrorString = "BOS found not as first page in stream.";
			break;
		case OVE_EOS_BEFORE_BOS:
			locErrorString - "EOS page found before BOS page";
			break;

	}

	cout << "ERROR "<<(int)inErrNo<<":  "<<locErrorString<<endl;

}

bool pageCB(OggPage* inOggPage) {
	//Validate the page header

	OggStreamValidationState* locStreamState = gValidationState.getStreamBySerialNo(inOggPage->header()->StreamSerialNo());

	if (locStreamState == NULL) {
		locStreamState = new OggStreamValidationState;
	}

	OggPageHeader* locHeader = inOggPage->header();

	bool isValid = true;
	
	//----------------------------------------------------------------------
	//Verify structure version is 0 (RFC 3533 Sec. 6.1)
	if (locHeader->StructureVersion() != 0) {
		error_write(OVE_STRUCTURE_VERSION_NOT_ZERO, inOggPage);
		isValid = false;
	}
	//----------------------------------------------------------------------
	//Verify BOS integrity.
	
	if (locHeader->isBOS()) {
		//Check is BOS is first page in stream
		if (locStreamState->mSeenAnything == true) {
			error_write(OVE_BOS_PAGE_NOT_AT_START, locStreamState);
			isValid = false;
		}
		//Check if this is the second or more BOS page.
		if (locStreamState->mSeenBOS) {
			error_write(OVE_MORE_THEN_ONE_BOS, locStreamState);
			isValid = false;
		}

		locStreamState->mSeenBOS = true;

	}

	//----------------------------------------------------------------------
	//Verify EOS integrity
	if (locHeader->isEOS()) {
		//Check is BOS has been seen before this EOS
		if (locStreamState->mSeenBOS == false) {
			error_write(OVE_EOS_BEFORE_BOS, locStreamState);
			isValid = false;
		}
		//Check if this is the second or more BOS page.
		if (locStreamState->mSeenEOS) {
			error_write(OVE_MORE_THEN_ONE_EOS, locStreamState);
			isValid = false;
		}
	
		locStreamState->mSeenEOS = true;
	}

	//----------------------------------------------------------------------
	//Verify Granule pos
	if (locHeader->GranulePos() != -1) {
		//Check if the granule pos decreased.
		if (locHeader->GranulePos() < locStreamState->mGranulePosUpto) {
			error_write(OVE_GRANULE_POS_DECREASED, locStreamState);
			isValid = false;
		}

		
		locStreamState->mGranulePosUpto = locHeader->GranulePos();

	} else {
		//No packets end here
	}

    	
	//----------------------------------------------------------------------

	//Verify sequence No
	if (locHeader->PageSequenceNo() == locStreamState->mSequenceNoUpto) {
		error_write(OVE_SEQUENCE_NO_REPEATED, locStreamState);
		isValid = false;
	} else if (locHeader->PageSequenceNo() < locStreamState->mSequenceNoUpto) {
		error_write(OVE_SEQUENCE_NO_DECREASED, locStreamState);
		isValid = false;
	} else {
		locStreamState->mSequenceNoUpto = locHeader->PageSequenceNo();
	}
	//----------------------------------------------------------------------

	//----------------------------------------------------------------------
	//----------------------------------------------------------------------
	//----------------------------------------------------------------------


	return isValid;;
}


int __cdecl _tmain(int argc, _TCHAR* argv[])
{
		//This program just dumps the pages out of a file in ogg format.
	// Currently does not error checking. Check your command line carefully !
	// USAGE :: OOOggValidate <OggFile>
	//
	bytePos = 0;
	if (argc < 2) {
		cout<<"Usage : OOOggValidate <filename>"<<endl;
	} else {
		OggDataBuffer testOggBuff;
		OggCallbackRego* locCBRego = new OggCallbackRego(&pageCB);
		const BUFF_SIZE = 8092;
		testOggBuff.registerPageCallback(locCBRego);

		fstream testFile;
		testFile.open(argv[1], ios_base::in | ios_base::binary);
		char* locBuff = new char[BUFF_SIZE];
		while (!testFile.eof()) {
			testFile.read(locBuff, BUFF_SIZE);
			unsigned long locBytesRead = testFile.gcount();
    		testOggBuff.feed(locBuff, locBytesRead);
		}

		delete locBuff;
	}

	
	return 0;
}


// OOOggValidate.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "OggValidationState.h"
#include "libOOOgg.h"
#include "libOOOgg.h"
enum eOggValidationErrors {
	OVE_OK,

	OVE_STRUCTURE_VERSION_NOT_ZERO,

	OVE_GRANULE_POS_DECREASED,
	
	OVE_MORE_THEN_ONE_BOS,
	OVE_BOS_PAGE_NOT_AT_START,
	
	OVE_MORE_THEN_ONE_EOS,
	OVE_EOS_BEFORE_BOS,

	OVE_DATA_AFTER_VALID_STREAM,
	
	OVE_SEQUENCE_NO_REPEATED,
	OVE_SEQUENCE_NO_DECREASED
};

OggValidationState gValidationState;
bool isValid = true;
void error_write(short int inErrNo, OggPage* inOggPage, OggStreamValidationState* inStreamState) {

	string locErrorString = "";
	switch (inErrNo) {
		case OVE_OK:
			//Do nothing
			return;
			break;
		case OVE_STRUCTURE_VERSION_NOT_ZERO:
			locErrorString = (int)inOggPage->header()->StructureVersion();
			locErrorString = "Structure version(" + locErrorString +") is not 0, (RFC 3533 Sec. 6.1)";
			break;
		case OVE_MORE_THEN_ONE_BOS:
			locErrorString = "More than one BOS page in stream.";
			break;
		case OVE_MORE_THEN_ONE_EOS:
			locErrorString = "More than one EOS page in stream.";
			break;
		case OVE_BOS_PAGE_NOT_AT_START:
			locErrorString = "BOS found not as first page in stream.";
			break;
		case OVE_EOS_BEFORE_BOS:
			locErrorString = "EOS page found before BOS page";
			break;
		case OVE_DATA_AFTER_VALID_STREAM:
			locErrorString = "More data was received for a stream that was previously complete and valid.";
			break;



		//Process errors
		case OggDataBuffer::PROCESS_FAILED_TO_SET_HEADER:
			locErrorString = "Lost Sync.";
			break;
		default:
			locErrorString = "Unnamed Error"; 
			break;

	}

	if (inStreamState != NULL) {
        inStreamState->mErrorCount++;
	}

	cout << "ERROR "<<(int)inErrNo<<":  "<<locErrorString<<endl;

}

bool pageCB(OggPage* inOggPage) {
	//Validate the page header

	
	OggStreamValidationState* locStreamState = gValidationState.getStreamBySerialNo(inOggPage->header()->StreamSerialNo());

	OggPageHeader* locHeader = inOggPage->header();

	//If this stream previously was valid, it should have no more data... so it's no longer valid.
	if (locStreamState->mState == OggStreamValidationState::VS_FULLY_VALID) {
		error_write(OVE_DATA_AFTER_VALID_STREAM,inOggPage,  locStreamState);
		isValid = false;
		locStreamState->mState = OggStreamValidationState::VS_INVALID;
	}
	
	
	//----------------------------------------------------------------------
	//Verify structure version is 0 (RFC 3533 Sec. 6.1)
	if (locHeader->StructureVersion() != 0) {
		error_write(OVE_STRUCTURE_VERSION_NOT_ZERO, inOggPage, locStreamState);
		isValid = false;
	}
	//----------------------------------------------------------------------
	//Verify BOS integrity.
	
	if (locHeader->isBOS()) {
		if (locStreamState->mState == OggStreamValidationState::VS_SEEN_NOTHING) {
			//Valid case.
			locStreamState->mState = OggStreamValidationState::VS_SEEN_BOS;
		}



		//Check is BOS is first page in stream
		if (locStreamState->mSeenAnything == true) {
			error_write(OVE_BOS_PAGE_NOT_AT_START,inOggPage,  locStreamState);
			isValid = false;
		}
		//Check if this is the second or more BOS page.
		if (locStreamState->mSeenBOS) {
			error_write(OVE_MORE_THEN_ONE_BOS,inOggPage, locStreamState);
			isValid = false;
		}

		locStreamState->mSeenBOS = true;

	}

	//----------------------------------------------------------------------
	//Verify EOS integrity
	if (locHeader->isEOS()) {
		
		if (locStreamState->mState == OggStreamValidationState::VS_SEEN_BOS) {
			//Valid case.
			locStreamState->mState = OggStreamValidationState::VS_FULLY_VALID;
		}


		//Check is BOS has been seen before this EOS
		if (locStreamState->mSeenBOS == false) {
			error_write(OVE_EOS_BEFORE_BOS,inOggPage, locStreamState);
			isValid = false;
		}
		//Check if this is the second or more BOS page.
		if (locStreamState->mSeenEOS) {
			error_write(OVE_MORE_THEN_ONE_EOS, inOggPage, locStreamState);
			isValid = false;
		}
	
		locStreamState->mSeenEOS = true;
	}

	//----------------------------------------------------------------------
	//Verify Granule pos
	if (locHeader->GranulePos() != -1) {
		//Check if the granule pos decreased.
		if (locHeader->GranulePos() < locStreamState->mGranulePosUpto) {
			error_write(OVE_GRANULE_POS_DECREASED, inOggPage, locStreamState);
			isValid = false;
		}

		
		locStreamState->mGranulePosUpto = locHeader->GranulePos();

	} else {
		//No packets end here
	}

    	
	//----------------------------------------------------------------------

	//Verify sequence No
	if (locHeader->PageSequenceNo() == locStreamState->mSequenceNoUpto) {
		//error_write(OVE_SEQUENCE_NO_REPEATED, inOggPage, locStreamState);
		//isValid = false;
	} else if (locHeader->PageSequenceNo() < locStreamState->mSequenceNoUpto) {
		error_write(OVE_SEQUENCE_NO_DECREASED, inOggPage, locStreamState);
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
	//bytePos = 0;

	int x;
	cin>>x;

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
		OggDataBuffer::eFeedResult locResult;
		while (!testFile.eof()) {
			testFile.read(locBuff, BUFF_SIZE);
			unsigned long locBytesRead = testFile.gcount();
    		locResult = testOggBuff.feed((const unsigned char*)locBuff, locBytesRead);
			switch(locResult) {
				case OVE_OK:
					break;
				
				case OVE_STRUCTURE_VERSION_NOT_ZERO:
					error_write(OVE_STRUCTURE_VERSION_NOT_ZERO, NULL , NULL);
					return OVE_STRUCTURE_VERSION_NOT_ZERO;
				
				case OVE_MORE_THEN_ONE_BOS:
					error_write(OVE_MORE_THEN_ONE_BOS, NULL, NULL);
					return OVE_MORE_THEN_ONE_BOS;
				
				case OVE_MORE_THEN_ONE_EOS:
					error_write(OVE_MORE_THEN_ONE_EOS, NULL, NULL);
					return OVE_MORE_THEN_ONE_EOS;
					
				case OVE_BOS_PAGE_NOT_AT_START:
					error_write(OVE_BOS_PAGE_NOT_AT_START, NULL, NULL);
					return OVE_BOS_PAGE_NOT_AT_START;

				case OVE_EOS_BEFORE_BOS:
					error_write(OVE_EOS_BEFORE_BOS, NULL, NULL);
					return OVE_EOS_BEFORE_BOS;
				default:
					error_write(locResult, NULL, NULL);
					return locResult;

			}
		}

		if (isValid) {
			cout<<"File was valid."<<endl;
		}
		delete locBuff;
	}

	
	return 0;
}


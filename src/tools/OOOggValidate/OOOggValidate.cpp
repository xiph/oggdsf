// OOOggValidate.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "OggValidationState.h"
#include "libOOOgg.h"





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
		OggValidationState testValidator;
		OggDataBuffer testOggBuff;
		testOggBuff.registerVirtualCallback(&testValidator);

	
		const BUFF_SIZE = 8092;
	
		fstream testFile;
		testFile.open(argv[1], ios_base::in | ios_base::binary);
		char* locBuff = new char[BUFF_SIZE];
		OggDataBuffer::eFeedResult locResult;
		while (!testFile.eof()) {
			testFile.read(locBuff, BUFF_SIZE);
			unsigned long locBytesRead = testFile.gcount();
    		locResult = testOggBuff.feed((const unsigned char*)locBuff, locBytesRead);
			switch(locResult) {
				case OggValidationState::OVE_OK:
					break;
				
				case OggValidationState::OVE_STRUCTURE_VERSION_NOT_ZERO:
					testValidator.error_write(OggValidationState::OVE_STRUCTURE_VERSION_NOT_ZERO, NULL , NULL);
					return OggValidationState::OVE_STRUCTURE_VERSION_NOT_ZERO;
				
				case OggValidationState::OVE_MORE_THAN_ONE_BOS:
					testValidator.error_write(OggValidationState::OVE_MORE_THAN_ONE_BOS, NULL, NULL);
					return OggValidationState::OVE_MORE_THAN_ONE_BOS;
				
				case OggValidationState::OVE_MORE_THAN_ONE_EOS:
					testValidator.error_write(OggValidationState::OVE_MORE_THAN_ONE_EOS, NULL, NULL);
					return OggValidationState::OVE_MORE_THAN_ONE_EOS;
					
				case OggValidationState::OVE_BOS_PAGE_NOT_AT_START:
					testValidator.error_write(OggValidationState::OVE_BOS_PAGE_NOT_AT_START, NULL, NULL);
					return OggValidationState::OVE_BOS_PAGE_NOT_AT_START;

				case OggValidationState::OVE_EOS_BEFORE_BOS:
					testValidator.error_write(OggValidationState::OVE_EOS_BEFORE_BOS, NULL, NULL);
					return OggValidationState::OVE_EOS_BEFORE_BOS;
				default:
					testValidator.error_write(locResult, NULL, NULL);
					return locResult;

			}
		}

		if (testValidator.isValid()) {
			cout<<"File was valid."<<endl;
		} else {
			cout<<"File is invalid."<<endl;
		}
		delete locBuff;
	}

	
	return 0;
}


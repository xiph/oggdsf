//===========================================================================
//Copyright (C) 2005 Zentaro Kavanagh
//Copyright (C) 2005 Commonwealth Scientific and Industrial Research
//                   Organisation (CSIRO) Australia
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

#include <libOOOggChef/utils.h>

#include <fstream>
#include <string>
#include <vector>


bool wantOnlyCMML(const vector<string>* inWantedMIMETypes)
{
	return (	inWantedMIMETypes->size() == 1
			&&	inWantedMIMETypes->at(0) == "text/x-cmml");
}

#ifdef UNICODE
bool fileExists(const wstring inFilename)
#else
bool fileExists(const string inFilename)
#endif
{
	// Behold, the world's most C++-portable filename-checking mechanism!

	fstream locFile;

	locFile.open(inFilename.c_str(), ios_base::in | ios_base::binary);
	if (locFile.is_open()) {
		locFile.close();
		return true;
	} else {
		locFile.close();
		return false;
	}
}



bool wantOnlyPacketBody(const vector<string>* inWantedMIMETypes)
{
	// TODO: This should check for packet bodies generally, not text/x-cmml

	return (	inWantedMIMETypes->size() == 1
			&&	inWantedMIMETypes->at(0) == "text/x-cmml");
}

#ifdef UNICODE
bool sendFile(const wstring inFilename, BufferWriter inBufferWriter, void* inBufferWriterUserData)
#else
bool sendFile(const string inFilename, BufferWriter inBufferWriter, void* inBufferWriterUserData)
#endif
{
	// If I had a dollar for every single time I've had to write this silly loop ...

	fstream locFile;

	locFile.open(inFilename.c_str(), ios_base::in | ios_base::binary);
	
	const unsigned short BUFFER_SIZE = 8192;
	unsigned char *locBuffer = new unsigned char[BUFFER_SIZE];
	while (!locFile.eof()) {
		locFile.read((char *)locBuffer, BUFFER_SIZE);

		unsigned long locBytesRead = locFile.gcount();
		inBufferWriter(locBuffer, locBytesRead, inBufferWriterUserData);
	}

	delete [] locBuffer;

	return true;
}
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

#pragma once


#include <libOOOggChef/libOOOggChef.h>

#include <string>
#include <vector>

// TODO: Wrap this up in a static class or a namespace or something, to prevent
// identifier clashes

/// Often used by IRecomposer's subclasses' constructor, and elsewhere
typedef bool (*BufferWriter) (unsigned char *buffer, unsigned long bufferSize, void *userData);

/// Check whether the user requested only CMML in the list of MIME types
bool LIBOOOGGCHEF_API wantOnlyCMML(const vector<string>* inWantedMIMETypes);

/// Check whether we should serve out only the packet bodies given the list of MIME types
bool LIBOOOGGCHEF_API wantOnlyPacketBody(const vector<string>* inWantedMIMETypes);

/// a.k.a. World's most stupid stat() function
#ifdef UNICODE
bool LIBOOOGGCHEF_API fileExists(const wstring inFilename);
#else
bool LIBOOOGGCHEF_API fileExists(const string inFilename);
#endif

/// Push out the entire contents of file to the inBufferWriter function

#ifdef UNICODE
bool LIBOOOGGCHEF_API sendFile(const wstring inFilename, BufferWriter inBufferWriter, void* inBufferWriterUserData);
#else
bool LIBOOOGGCHEF_API sendFile(const string inFilename, BufferWriter inBufferWriter, void* inBufferWriterUserData);
#endif

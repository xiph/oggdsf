//===========================================================================
//Copyright (C) 2003-2006 Zentaro Kavanagh
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
#include "CustomSourceClass.h"

CustomSourceClass::CustomSourceClass(void)
{
}

CustomSourceClass::~CustomSourceClass(void)
{
	mSourceFile.close();
}

unsigned long CustomSourceClass::seek(unsigned long inPos) 
{
	mSourceFile.clear();
	mSourceFile.seekg(inPos, ios_base::beg);
	return mSourceFile.tellg();
}
void CustomSourceClass::close() {
	mSourceFile.close();
}
bool CustomSourceClass::open(string inSourceLocation, unsigned long) {
	mSourceFile.open(inSourceLocation.c_str(), ios_base::in|ios_base::binary);
	return mSourceFile.is_open();
}
void CustomSourceClass::clear() {
	mSourceFile.clear();
}
bool CustomSourceClass::isEOF() {
	return mSourceFile.eof();
}
unsigned long CustomSourceClass::read(char* outBuffer, unsigned long inNumBytes) {
	//If you were going to decrypt your data, you would do that here
	mSourceFile.read(outBuffer, inNumBytes);
	return mSourceFile.gcount();
}


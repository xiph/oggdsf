#include "StdAfx.h"
#include "filterfilesource.h"

FilterFileSource::FilterFileSource(void)
{
}

FilterFileSource::~FilterFileSource(void)
{
}

unsigned long FilterFileSource::seek(unsigned long inPos) {
	mSourceFile.seekg(inPos, ios_base::beg);
	return mSourceFile.tellg();
}
void FilterFileSource::close() {
	mSourceFile.close();
}
bool FilterFileSource::open(string inSourceLocation) {
	mSourceFile.open(inSourceLocation.c_str(), ios_base::in|ios_base::binary);
	return mSourceFile.is_open();
}
void FilterFileSource::clear() {
	mSourceFile.clear();
}
bool FilterFileSource::isEOF() {
	return mSourceFile.eof();
}
unsigned long FilterFileSource::read(char* outBuffer, unsigned long inNumBytes) {
	mSourceFile.read(outBuffer, inNumBytes);
	return mSourceFile.gcount();
}

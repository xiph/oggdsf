#include "StdAfx.h"
#include ".\DNVorbisComments.h"
#using <mscorlib.dll>


namespace illiminable {
	namespace Ogg {
		namespace libVorbisCommentDotNET {
DNVorbisComments::DNVorbisComments(void)
	:	mNativeClass(NULL)
{
	mNativeClass = new ::VorbisComments;
}

DNVorbisComments::DNVorbisComments(::VorbisComments* inNativeClass) {
	mNativeClass = inNativeClass;
}

DNVorbisComments::~DNVorbisComments(void)
{
	delete mNativeClass;
}



String* DNVorbisComments::vendorString() {
	return Wrappers::CStrToNetStr(mNativeClass->vendorString().c_str());
}
bool DNVorbisComments::setVendorString(String* inVendorString) {
	char* locCS = Wrappers::netStrToCStr(inVendorString);
	mNativeClass->setVendorString(locCS);
	Wrappers::releaseCStr(locCS);
	return true;
}

Int64 DNVorbisComments::numUserComments() {
	Int64 locNum = mNativeClass->numUserComments();
	return locNum;
}
DNSingleVorbisComment* DNVorbisComments::getUserComment(Int64 inIndex) {
	unsigned long locIndex = inIndex;

	//FIX::: Need to clone this... or bad things will happen when garbage colelcted.
	return new illiminable::Ogg::libVorbisCommentDotNET::DNSingleVorbisComment((::SingleVorbisComment*)mNativeClass->getUserComment(locIndex));
}
	
	//vector<SingleVorbisComment> getCommentsByKey(String* inKey);

//bool DNVorbisComments::addComment(SingleVorbisComment* inComment) {
//
//}
bool DNVorbisComments::addComment(String* inKey, String* inValue) {
	char* locCS1 = Wrappers::netStrToCStr(inKey);
	char* locCS2 = Wrappers::netStrToCStr(inValue);
	mNativeClass->addComment(locCS1, locCS2);
	Wrappers::releaseCStr(locCS1);
	Wrappers::releaseCStr(locCS2);
	return true;
}

	//bool parseOggPacket(OggPacket* inPacket, unsigned long inStartOffset);
	//OggPacket* toOggPacket();
String* DNVorbisComments::toString() {
	return Wrappers::CStrToNetStr(mNativeClass->toString().c_str());
}

Int64 DNVorbisComments::size() {
	Int64 locNum = mNativeClass->size();
	return locNum;
}

		}}}
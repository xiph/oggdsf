#include "StdAfx.h"
#include ".\vorbiscomments.h"
#using <mscorlib.dll>


namespace illiminable {
	namespace Ogg {
		namespace libVorbisCommentDotNET {
VorbisComments::VorbisComments(void)
	:	mNativeClass(NULL)
{
	mNativeClass = new ::VorbisComments;
}

VorbisComments::VorbisComments(::VorbisComments* inNativeClass) {
	mNativeClass = inNativeClass;
}

VorbisComments::~VorbisComments(void)
{
	delete mNativeClass;
}



String* VorbisComments::vendorString() {
	return Wrappers::CStrToNetStr(mNativeClass->vendorString().c_str());
}
bool VorbisComments::setVendorString(String* inVendorString) {
	char* locCS = Wrappers::netStrToCStr(inVendorString);
	mNativeClass->setVendorString(locCS);
	Wrappers::releaseCStr(locCS);
	return true;
}

Int64 VorbisComments::numUserComments() {
	Int64 locNum = mNativeClass->numUserComments();
	return locNum;
}
SingleVorbisComment* VorbisComments::getUserComment(Int64 inIndex) {
	unsigned long locIndex = inIndex;

	//FIX::: Need to clone this... or bad things will happen when garbage colelcted.
	return new illiminable::Ogg::libVorbisCommentDotNET::SingleVorbisComment((::SingleVorbisComment*)mNativeClass->getUserComment(locIndex));
}
	
	//vector<SingleVorbisComment> getCommentsByKey(String* inKey);

//bool VorbisComments::addComment(SingleVorbisComment* inComment) {
//
//}
bool VorbisComments::addComment(String* inKey, String* inValue) {
	char* locCS1 = Wrappers::netStrToCStr(inKey);
	char* locCS2 = Wrappers::netStrToCStr(inValue);
	mNativeClass->addComment(locCS1, locCS2);
	Wrappers::releaseCStr(locCS1);
	Wrappers::releaseCStr(locCS2);
	return true;
}

	//bool parseOggPacket(OggPacket* inPacket, unsigned long inStartOffset);
	//OggPacket* toOggPacket();
String* VorbisComments::toString() {
	return Wrappers::CStrToNetStr(mNativeClass->toString().c_str());
}

Int64 VorbisComments::size() {
	Int64 locNum = mNativeClass->size();
	return locNum;
}

		}}}
#pragma once
#pragma unmanaged
#include "dllstuff.h"
#include "libVorbisComment/VorbisComments.h"

#pragma managed
using namespace System;
using namespace illiminable::libiWrapper;
namespace illiminable {
	namespace Ogg {
		namespace libVorbisCommentDotNET {

//NATIVE CLASS
//class LIBVORBISCOMMENT_API VorbisComments
//{
//public:
//	VorbisComments(void);
//	~VorbisComments(void);
//
//	string vendorString();
//	bool setVendorString(string inVendorString);
//
//	unsigned long numUserComments();
//	SingleVorbisComment getUserComment(unsigned long inIndex);
//	
//	vector<SingleVorbisComment> getCommentsByKey(string inKey);
//
//	bool addComment(SingleVorbisComment inComment);
//	bool addComment(string inKey, string inValue);
//
//	bool parseOggPacket(OggPacket* inPacket, unsigned long inStartOffset);
//	OggPacket* toOggPacket();
//	string toString();
//
//	unsigned long size();
//protected:
//	string mVendorString;
//	vector<SingleVorbisComment> mCommentList;
//};




public __gc class DNVorbisComments
{
public:
	DNVorbisComments(void);
	DNVorbisComments(::VorbisComments* inNativeClass);
	~DNVorbisComments(void);

	String* vendorString();
	bool setVendorString(String* inVendorString);

	Int64 numUserComments();
	DNSingleVorbisComment* getUserComment(Int64 inIndex);
	
	//vector<SingleVorbisComment> getCommentsByKey(String* inKey);

	bool addComment(SingleVorbisComment* inComment);
	bool addComment(String* inKey, String* inValue);
	
	//bool parseOggPacket(OggPacket* inPacket, unsigned long inStartOffset);
	//OggPacket* toOggPacket();
	String* toString();

	Int64 size();

protected:
	::VorbisComments* mNativeClass;

private:
	
};


		}}}
#pragma once

#pragma unmanaged
//The directory is necessary to avoid recursive inclusion. native and DN class have same names.
#include "libVorbisComment/SingleVorbisComment.h"

#pragma managed

using namespace illiminable::libiWrapper;
using namespace System;

namespace illiminable {
	namespace Ogg {
		namespace libVorbisCommentDotNET {

//NATIVE CLASS
//class LIBVORBISCOMMENT_API SingleVorbisComment
//{
//public:
//	SingleVorbisComment(void);
//	~SingleVorbisComment(void);
//
//	string key();
//	bool setKey(string inKey);
//
//	string value();
//	bool setValue(string inValue);
//
//	string toString();
//
//	unsigned long length();
//
//	bool parseComment(string inCommentString);
//
//protected:
//	string mKey;
//	string mValue;
//};


public __gc class DNSingleVorbisComment
{
public:
	DNSingleVorbisComment(void);
	DNSingleVorbisComment(::SingleVorbisComment* inNativeClass);
	~DNSingleVorbisComment(void);

	String* key();
	bool setKey(String* inKey);

	String* value();
	bool setValue(String* inValue);

	String* toString();

	Int64 length();

protected:

	::SingleVorbisComment* mNativeClass;
	//bool parseComment(String inCommentString);
};


		}}}
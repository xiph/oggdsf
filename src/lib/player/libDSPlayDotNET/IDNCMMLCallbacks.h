#pragma once
#pragma unmanaged
#include "libCMMLTags.h"

#pragma managed
#using "libCMMLTagsDotNET.dll"
using namespace illiminable::libCMMLTagsDotNET;

namespace illiminable {
namespace libDSPlayDotNET {
public __gc __interface IDNCMMLCallbacks {
	//class ICMMLCallbacks
	//{
	//public:
	//	virtual bool clipCallback(C_ClipTag* inClipTag) = 0;
	//	virtual bool headCallback(C_HeadTag* inHeadTag) = 0;
	//};
public:
	virtual bool clipCallback(ClipTag* inClipTag) = 0;
	virtual bool headCallback(HeadTag* inHeadTag) = 0;


	
	

};

}
}
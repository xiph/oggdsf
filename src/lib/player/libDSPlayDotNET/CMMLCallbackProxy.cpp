#include "StdAfx.h"
#include ".\cmmlcallbackproxy.h"
#using <mscorlib.dll>

CMMLCallbackProxy::CMMLCallbackProxy(void)
	:	mDNCMMLCallbacks(NULL)
{
}

CMMLCallbackProxy::~CMMLCallbackProxy(void)
{
}

bool CMMLCallbackProxy::setManagedDelegate(gcroot<IDNCMMLCallbacks*> inManagedCallbacks) {
	mDNCMMLCallbacks = inManagedCallbacks;

}
bool CMMLCallbackProxy::clipCallback(C_ClipTag* inClipTag) {
	if (mDNCMMLCallbacks != NULL) {
		gcroot<ClipTag*> locClip = new ClipTag(inClipTag->clone());
	
		return mDNCMMLCallbacks->clipCallback(locClip);
	} else {
		return false;
	}
}
bool CMMLCallbackProxy::headCallback(C_HeadTag* inHeadTag) {
	if (mDNCMMLCallbacks != NULL) {
		gcroot<HeadTag*> locHead = new HeadTag(inHeadTag->clone());
	
		return mDNCMMLCallbacks->headCallback(locHead);
	} else {
		return false;
	}

}
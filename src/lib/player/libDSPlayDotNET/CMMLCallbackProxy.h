#pragma once
#include "ICMMLCallbacks.h"
#include "libCMMLTags.h"
#include "IDNCMMLCallbacks.h"
#include <vcclr.h>

using namespace illiminable::libDSPlayDotNET;
class CMMLCallbackProxy
	:	public ICMMLCallbacks
{
public:
	CMMLCallbackProxy(void);
	~CMMLCallbackProxy(void);

	virtual bool clipCallback(C_ClipTag* inClipTag);
	virtual bool headCallback(C_HeadTag* inHeadTag);

	bool setManagedDelegate(gcroot<IDNCMMLCallbacks*> inManagedCallbacks);

protected:
	gcroot<IDNCMMLCallbacks*> mDNCMMLCallbacks;
};

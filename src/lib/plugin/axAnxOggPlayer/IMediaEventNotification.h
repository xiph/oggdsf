#pragma once

class IMediaEventNotification
{
public:
	IMediaEventNotification(void);
	virtual ~IMediaEventNotification(void);

	virtual bool eventNotification(int inEventCode, int inParam1, int inParam2) = 0;
};

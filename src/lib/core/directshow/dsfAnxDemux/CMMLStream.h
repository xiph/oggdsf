#pragma once

#include "anxdllstuff.h"
#include "OggPacket.h"
#include "OggStream.h"
class CMMLStream
	:	public OggStream
{
public:
	CMMLStream(OggPage* inBOSPage, OggDemuxSourceFilter* inOwningFilter);
	virtual ~CMMLStream(void);
	virtual void setLastEndGranPos(__int64 inGranPos);


	virtual bool InitCodec(StampedOggPacket* inOggPacket);
	virtual BYTE* getFormatBlock();
	virtual unsigned long getFormatBlockSize();

	virtual GUID getMajorTypeGUID();
	virtual GUID getFormatGUID();
	virtual GUID getSubtypeGUID();
	virtual wstring getPinName();
	virtual bool createFormatBlock();

	virtual LONGLONG getCurrentPos();

protected:
	OggPacket* mAnxDataPacket;
	sCMMLFormatBlock* mCMMLFormatBlock;
};

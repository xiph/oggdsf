#pragma once

#include "OggMuxInputPin.h"
class AnxMuxInputPin
	:	public OggMuxInputPin
{
public:
	AnxMuxInputPin(void);
	~AnxMuxInputPin(void);


	virtual HRESULT OggMuxInputPin::SetMediaType(const CMediaType* inMediaType);
};

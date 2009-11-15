#pragma once

#include "IFilterDataSource.h"

DECLARE_INTERFACE_(ICustomSource, IUnknown)
{
public:
	virtual HRESULT setCustomSourceAndLoad(IFilterDataSource* inDataSource) = 0;
};

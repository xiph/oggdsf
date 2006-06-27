#pragma once
#include "IFilterDataSource.h"
class ICustomSource {
public:
	virtual HRESULT setCustomSourceAndLoad(IFilterDataSource* inDataSource) = 0;
};
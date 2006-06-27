#pragma once

#include "IFilterDataSource.h"
class CustomOggChainGranuleSeekTable
	:	public AutoOggChainGranuleSeekTable
{
public:
	CustomOggChainGranuleSeekTable(IFilterDataSource* inDataSource);
	virtual ~CustomOggChainGranuleSeekTable(void);

	virtual bool buildTable();

protected:
	IFilterDataSource* mCustomSource;

};

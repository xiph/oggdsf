#include "StdAfx.h"
#include "httpfilesource.h"

HTTPFileSource::HTTPFileSource(void)
{
	WORD locWinsockVersion = MAKEWORD(1,1);
	WSADATA locWinsockData;
	int locRet= 0;

	locRet = WSAStartup(locWinsockVersion, &locWinsockData);
	if ((locRet != 0) || (locWinsockData.wVersion != locWinsockVersion)) {
		//Failed to setup.
		WSACleanup();
		throw 0;
	}



}

HTTPFileSource::~HTTPFileSource(void)
{
	WSACleanup();
}

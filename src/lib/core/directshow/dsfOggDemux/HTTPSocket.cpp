//===========================================================================
//Copyright (C) 2003, 2004 Zentaro Kavanagh
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions
//are met:
//
//- Redistributions of source code must retain the above copyright
//  notice, this list of conditions and the following disclaimer.
//
//- Redistributions in binary form must reproduce the above copyright
//  notice, this list of conditions and the following disclaimer in the
//  documentation and/or other materials provided with the distribution.
//
//- Neither the name of Zentaro Kavanagh nor the names of contributors 
//  may be used to endorse or promote products derived from this software 
//  without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
//PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE ORGANISATION OR
//CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//===========================================================================
#include "stdafx.h"
#include ".\httpsocket.h"

HTTPSocket::HTTPSocket(void)
	:	mWasError(false)
	,	mIsEOF(false)
	,	mIsOpen(false)
	,	mSeenResponse(false)
{
	//debugLog2.open("G:\\logs\\httpsocket.log", ios_base::out);

	//Setup the socket API
	WORD locWinsockVersion = MAKEWORD(1,1);
	WSADATA locWinsockData;
	int locRet= 0;

	locRet = WSAStartup(locWinsockVersion, &locWinsockData);
	if ((locRet != 0) || (locWinsockData.wVersion != locWinsockVersion)) {
		//Failed to setup.
		//debugLog2<<"Failed to start winsock V "<<locWinsockData.wVersion<<endl;
		WSACleanup();
		throw 0;
	}

	//debugLog2<<"Winsock started"<<endl;
}

HTTPSocket::~HTTPSocket(void)
{
	//debugLog2<<"Winsock ended"<<endl;
	//debugLog2.close();
	
	WSACleanup();
}


bool HTTPSocket::setupSocket(string inSourceLocation) {
	
	//debugLog2<<"Setup Socket:"<<endl;
	IN_ADDR locAddress;  //iaHost
	LPHOSTENT locHostData;;  //lpHost

	bool locValidURL = splitURL(inSourceLocation);

	locAddress.S_un.S_addr = inet_addr(mServerName.c_str());
	

	if (locAddress.S_un.S_addr == INADDR_NONE) {
		locHostData = gethostbyname(mServerName.c_str());
	} else {
		locHostData = gethostbyaddr((const char*)&locAddress, sizeof(struct in_addr), AF_INET);
	}



	if (locHostData == NULL) {
		//debugLog2<<"LocHostData is NULL"<<endl;
		//Failed
		return false;
	}

	mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mSocket == INVALID_SOCKET) {
		//debugLog2<<"Socket Invalid"<<endl;
		//Failed
		return false;
	}


	LPSERVENT locServiceData; //lpServEnt
	SOCKADDR_IN locServiceSocketAddr; //saServer
	
	if (mPort == 0) {
		locServiceData = getservbyname("http", "tcp");
		if (locServiceData == NULL) {
			locServiceSocketAddr.sin_port = htons(80);
		} else {
			locServiceSocketAddr.sin_port = locServiceData->s_port;
		}
	} else {
		//Explicit port
		locServiceSocketAddr.sin_port = htons(mPort);
	}



	locServiceSocketAddr.sin_family = AF_INET;
	locServiceSocketAddr.sin_addr = *((LPIN_ADDR)*locHostData->h_addr_list);


	int locRetVal = 0;
	locRetVal = connect(mSocket, (LPSOCKADDR)&locServiceSocketAddr, sizeof(SOCKADDR_IN));
	if (locRetVal == SOCKET_ERROR) {
		//debugLog2<<"Failed to connect..."<<endl;
		closesocket(mSocket);
		return false;
	}

	return true;


}

string HTTPSocket::assembleRequest(string inFilePath) {
	string retRequest;
	retRequest = "GET " + inFilePath+ " HTTP/1.1\r\n" + "Host: " + mServerName+ "\r\n" + "Connection: close" + "\r\n\r\n";
	//debugLog2<<"Assembled Req : "<<endl<<retRequest<<endl;
	return retRequest;
}

bool HTTPSocket::httpRequest(string inRequest) {
	//debugLog2<<"Http Request:"<<endl;
	int locRetVal = send(mSocket, inRequest.c_str(), (int)inRequest.length(), 0);

	if (locRetVal == SOCKET_ERROR) {
		//debugLog2<<"Socket error on send"<<endl;
		closesocket(mSocket);
		return false;
	}
	return true;
}

bool HTTPSocket::splitURL(string inURL) {
	//debugLog2<<"Split url:"<<endl;
	string locProtocol;
	string locServerName;
	string locPath;
	string locPort;
	string locTemp;
	size_t locPos2;
	size_t locPos = inURL.find(':');
	if (locPos == string::npos) {
		//No colon... not a url or file... failure.
		return false;
	} else {
		locProtocol = inURL.substr(0, locPos);
		locTemp = inURL.substr(locPos+1);
		locPos = locTemp.find("//");
		if ((locPos == string::npos) || (locPos != 0)) {
			return false;
		} else {
            locTemp = locTemp.substr(locPos+2);
			locPos = locTemp.find('/');
			if (locPos == string::npos) {
				return false;
			} else {
				locPos2 = locTemp.find(':');
				if (locPos2 == string::npos) {
					locServerName = locTemp.substr(0, locPos);
					locPath = locTemp.substr(locPos);
				} else if (locPos2 < locPos) {
					//Explicit port specification
					locPort = locTemp.substr(locPos2 + 1, locPos - locPos2 - 1);
					locServerName = locTemp.substr(0, locPos2);
					locPath = locTemp.substr(locPos);
				}

			}
		}
		
	}

	mServerName = locServerName;
	mFileName = locPath;
	if (locPort != "") {
		//TODO::: Error checking needed
		mPort = atoi(locPort.c_str());
	} else {
		mPort = 0;
	}
	//debugLog2<<"Proto : "<<locProtocol<<endl<<"Server : "<<locServerName<<endl<<" Path : "<<mFileName<<" Port : "<<mPort<<endl;
	return true;

}
void HTTPSocket::closeSocket() {
	//debugLog2<<"Close Socket:"<<endl;
	closesocket(mSocket);
}
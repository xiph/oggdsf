#include "StdAfx.h"
#include "httpfilesource.h"

HTTPFileSource::HTTPFileSource(void)
	:	mWasError(false)
	,	mIsEOF(false)
	,	mIsOpen(false)
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

void HTTPFileSource::DataProcessLoop() {

	int locNumRead = 0;
	char* locBuff = NULL;
	const unsigned long RECV_BUFF_SIZE = 4096;
	locBuff = new char[RECV_BUFF_SIZE];
	while(true) {

		locNumRead = recv(mSocket, locBuff, RECV_BUFF_SIZE, 0);
		if (locNumRead == SOCKET_ERROR) {
			mWasError = true;
			break;
		}

		if (locNumRead == 0) {
			mIsEOF = true;
			break;
		}
		//Add to buffer
		mStreamBuffer.write(locBuff, locNumRead);
	}

	delete locBuff;


}

bool HTTPFileSource::setupSocket(string inSourceLocation) {
	

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
		//Failed
		return false;
	}

	mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mSocket = INVALID_SOCKET) {
		//Failed
		return false;
	}


	LPSERVENT locServiceData; //lpServEnt
	SOCKADDR_IN locServiceSocketAddr; //saServer

	locServiceData = getservbyname("http", "tcp");
	if (locServiceData == NULL) {
		locServiceSocketAddr.sin_port = htons(80);
	} else {
		locServiceSocketAddr.sin_port = locServiceData->s_port;
	}



	locServiceSocketAddr.sin_family = AF_INET;
	locServiceSocketAddr.sin_addr = *((LPIN_ADDR)*locHostData->h_addr_list);


	int locRetVal = 0;
	locRetVal = connect(mSocket, (LPSOCKADDR)&locServiceSocketAddr, sizeof(SOCKADDR_IN));
	if (locRetVal == SOCKET_ERROR) {
		closesocket(mSocket);
		return false;
	}




}

bool HTTPFileSource::httpRequest(string inRequest) {
	
	int locRetVal = send(mSocket, inRequest.c_str(), inRequest.length(), 0);

	if (locRetVal == SOCKET_ERROR) {
		closesocket(mSocket);
		return false;
	}
}
DWORD HTTPFileSource::ThreadProc(void) {
	
	while(true) {
		DWORD locThreadCommand = GetRequest();
		
		switch(locThreadCommand) {
			case THREAD_EXIT:
				
				Reply(S_OK);
				return S_OK;



			case THREAD_RUN:
				
				Reply(S_OK);
				DataProcessLoop();
				break;
		}
	
	
	}
	return S_OK;
}
unsigned long HTTPFileSource::seek(unsigned long inPos) {
	//Close the socket down
	//Open up a new one to the same place.
	//Make the partial content request.
	return 0;
}

bool HTTPFileSource::splitURL(string inURL) {
	return false;
}
void HTTPFileSource::closeSocket() {
	closesocket(mSocket);
}
void HTTPFileSource::close() {
	//Close the socket down.
	closeSocket();
}

bool HTTPFileSource::startThread() {
	if (ThreadExists() == FALSE) {
		Create();
	}
	CallWorker(THREAD_RUN);
	return true;
}
bool HTTPFileSource::open(string inSourceLocation) {
	//Open network connection and start feeding data into a buffer
	//

	bool locIsOK = setupSocket(inSourceLocation);

	if (!locIsOK) {
		closeSocket();
		return false;
	}
	locIsOK = startThread();


	return locIsOK;
}
void HTTPFileSource::clear() {
	//Reset flags.
	mIsEOF = false;
	mWasError = false;
}
bool HTTPFileSource::isEOF() {
	if ((mStreamBuffer.tellp() - mStreamBuffer.tellg() == 0) && mIsEOF) {
		return true;
	} else {
		return false;
	}
}
unsigned long HTTPFileSource::read(char* outBuffer, unsigned long inNumBytes) {
	//Reads from the buffer, will return 0 if nothing in buffer.
	// If it returns 0 check the isEOF flag to see if it was the end of file or the network is just slow.

	if(mIsEOF || mWasError) {
		return 0;
	} else {
		mStreamBuffer.read(outBuffer, inNumBytes);
		return mStreamBuffer.gcount();
	}

}

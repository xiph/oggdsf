#pragma once
#include "oggdllstuff.h"
#include <winsock.h>
//#include <stdlib.h>
#include <string>
using namespace std;
class OGG_DEMUX_API HTTPFileSource
	:	public IFilterDataSource
	,	public CAMThread
{
public:
	HTTPFileSource(void);
	~HTTPFileSource(void);

	//Thread commands
	static const int THREAD_RUN = 0;
	static const int THREAD_EXIT = 1;
	//

	//IFilterDataSource Interface
	virtual unsigned long seek(unsigned long inPos);
	virtual void close() ;
	virtual bool open(string inSourceLocation);
	virtual void clear();
	virtual bool isEOF();
	virtual unsigned long read(char* outBuffer, unsigned long inNumBytes);
	//

	//CAMThread pure virtuals
	DWORD HTTPFileSource::ThreadProc();
	//


protected:
	virtual bool setupSocket(string inSourceLocation);
	virtual void closeSocket();
	virtual bool splitURL(string inURL);
	bool httpRequest(string inRequest);
	bool HTTPFileSource::startThread();
	void DataProcessLoop();
	string mServerName;
	string mFileName;
	SOCKET mSocket;
	stringstream mStreamBuffer;

	bool mIsEOF;
	bool mWasError;
	bool mIsOpen;
};

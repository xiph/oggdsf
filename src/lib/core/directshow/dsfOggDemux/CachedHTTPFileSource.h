#pragma once



class CachedHTTPFileSource
	:	public IFilterDataSource
	,	public CAMThread
	,	protected HTTPSocket
{
public:
	CachedHTTPFileSource(void);
	virtual ~CachedHTTPFileSource(void);

	//Thread commands
	static const int THREAD_RUN = 0;
	static const int THREAD_EXIT = 1;
	//

	//IFilterDataSource
	virtual unsigned long seek(unsigned long inPos);
	virtual void close();
	virtual bool open(string inSourceLocation);
	virtual void clear();
	virtual bool isEOF();
	virtual unsigned long read(char* outBuffer, unsigned long inNumBytes);

	//CAMThread pure virtuals
	DWORD ThreadProc();
protected:
	typedef pair<__int64, wstring> tMapValue;
	typedef pair<__int64, tMapValue> tMapEntry;

	typedef map<__int64, tMapValue> tRangeMap;

	tRangeMap mRangeMap;

	SingleMediaFileCache mReadFile;
	SingleMediaFileCache mWriteFile;

	tMapEntry mCurrentReadRange;
	tMapEntry mCurrentWriteRange;

	//Cache helpers
	CachedHTTPFileSource::tMapEntry findNextHoleInData(__int64 inUpto);
	bool inRange(CachedHTTPFileSource::tMapEntry inTestRange, __int64 inTestValue);

	bool startThread();
	void DataProcessLoop();
	CCritSec* mBufferLock;

};

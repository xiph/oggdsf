#pragma once

class HTTPFileSource
	:	public IFilterDataSource
{
public:
	HTTPFileSource(void);
	~HTTPFileSource(void);


	virtual unsigned long seek(unsigned long inPos);
	virtual void close() ;
	virtual bool open(string inSourceLocation);
	virtual void clear();
	virtual bool isEOF();
	virtual unsigned long read(char* outBuffer, unsigned long inNumBytes);

	virtual bool setupSocket(string inSourceLocation);
};

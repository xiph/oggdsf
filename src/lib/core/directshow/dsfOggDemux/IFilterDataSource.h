#pragma once

class OGG_DEMUX_API IFilterDataSource
{
public:
	virtual unsigned long seek(unsigned long inPos) = 0;
	virtual void close()  = 0;
	virtual bool open(string inSourceLocation) = 0;
	virtual void clear() = 0;
	virtual bool isEOF() = 0;
	virtual unsigned long read(char* outBuffer, unsigned long inNumBytes) = 0;
};

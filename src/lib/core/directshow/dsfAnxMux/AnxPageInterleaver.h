#pragma once

#include "OggPageInterleaver.h"
#include "AnxPacketMaker.h"
class AnxPageInterleaver
	:	public OggPageInterleaver
{
public:
	AnxPageInterleaver(IOggCallback* inFileWriter, INotifyComplete* inNotifier, unsigned long inVersionMajor, unsigned long inVersionMinor);
	virtual ~AnxPageInterleaver(void);

	
	
	virtual void processData();


protected:
	void addAnnodex_2_0_BOS();
	void addAllAnxData_2_0_BOS();
	void addAnnodexEOS();
	bool gotAllHeaders();

	bool mIsAnxSetup;
	
	unsigned long mVersionMajor;
	unsigned long mVersionMinor;

	unsigned long mAnxSerialNo;
};

#pragma once

class OggDemuxFilter
	:	public CBaseFilter
{
public:
	OggDemuxFilter(void);
	~OggDemuxFilter(void);


	//CBaseFilter methods
	virtual CBasePin* GetPin(int inPinNo);
	virtual int GetPinCount();

	//IMediaFilter methods
	STDMETHODIMP Run( REFERENCE_TIME inBaseRefTime );
	STDMETHODIMP Pause ();
	STDMETHODIMP Stop();


protected:
	vector<OggDemuxOutputPin*> mOutputPinList;

};

#include "StdAfx.h"
#include ".\basicseekable.h"

BasicSeekable::BasicSeekable(IMediaSeeking* inDelegate)
	//: CUnknown(NAME("Basic Seekable Thing"), NULL, &mHR)
{
	mSeekDelegate = inDelegate;
	mSeekTimeBase = 0;

		mSeekingCap = 0;
	mSeekingCap =		AM_SEEKING_CanSeekAbsolute |
						AM_SEEKING_CanSeekForwards |
						AM_SEEKING_CanSeekBackwards |
						AM_SEEKING_CanGetCurrentPos |
						AM_SEEKING_CanGetStopPos |
						AM_SEEKING_CanGetDuration;

	seekDebug.open("C:\\temp\\seeker.log", ios_base::out|ios_base::binary);

}

BasicSeekable::BasicSeekable(void)
	//: CUnknown(NAME("Basic Seekable Thing"), NULL, &mHR) 
{
	mSeekTimeBase = 0;
	mSeekDelegate = NULL;	
	mSeekingCap =		AM_SEEKING_CanSeekAbsolute |
						AM_SEEKING_CanSeekForwards |
						AM_SEEKING_CanSeekBackwards |
						AM_SEEKING_CanGetCurrentPos |
						AM_SEEKING_CanGetStopPos |
						AM_SEEKING_CanGetDuration;
	
	seekDebug.open("C:\\temp\\seeker.log", ios_base::out|ios_base::binary);
	

}



bool BasicSeekable::SetDelegate(IMediaSeeking* inDelegate) {
	mSeekDelegate = inDelegate;
	return true;
}
BasicSeekable::~BasicSeekable(void)
{
	
	seekDebug.close();
}
//IMediaSeeking Interface
STDMETHODIMP BasicSeekable::GetCapabilities(DWORD* inCapabilities) {
//typedef 
//enum AM_SEEKING_SeekingCapabilities {
//    AM_SEEKING_CanSeekAbsolute        = 0x1,
//    AM_SEEKING_CanSeekForwards        = 0x2,
//    AM_SEEKING_CanSeekBackwards       = 0x4,
//    AM_SEEKING_CanGetCurrentPos       = 0x8,
//    AM_SEEKING_CanGetStopPos          = 0x10,
//    AM_SEEKING_CanGetDuration         = 0x20,
//    AM_SEEKING_CanPlayBackwards       = 0x40,
//    AM_SEEKING_CanDoSegments          = 0x80,
//    AM_SEEKING_Source                 = 0x100
//}   AM_SEEKING_SEEKING_CAPABILITIES;

	
	//*inCapabilities	=	mSeekingCap;
	//return S_OK;
	
	if (mSeekDelegate != NULL) {
		seekDebug<<"GetCaps : Passed on..."<<endl;
		return mSeekDelegate->GetCapabilities(inCapabilities);
	} else {
		seekDebug<<"GetCaps : NULL Delegate"<<endl;
		//This is probably wrong.
		return E_NOTIMPL;
	}

}
STDMETHODIMP BasicSeekable::CheckCapabilities(DWORD* outCapabilities) {
	
	
	//if (inCapabilities == NULL) {
	//	return E_POINTER;
	//}
	//*inCapabilities &= mSeekingCap;

	//if (*inCapabilities == mSeekingCap) {
	//	return S_OK;
	//} else {
	//	return E_NOTIMPL;
	//}


	if (mSeekDelegate != NULL) {
		seekDebug<<"CheckCaps : Passed on..."<<endl;
		return mSeekDelegate->CheckCapabilities(outCapabilities);
	} else {
		seekDebug<<"CheckCaps : NULL Delegate"<<endl;
		//This is probably wrong.
		return E_NOTIMPL;
	}

}
STDMETHODIMP BasicSeekable::IsFormatSupported(const GUID* inFormat) {

	//if (inFormat == NULL) {
	//	return E_POINTER;
	//}
	//if (*inFormat == TIME_FORMAT_MEDIA_TIME) {
	//	return S_OK;
	//} else {
	//	return S_FALSE;
	//}

	if (mSeekDelegate != NULL) {
		seekDebug<<"IsFromatsupp : Passed on..."<<endl;
		return mSeekDelegate->IsFormatSupported(inFormat);
	} else {
		seekDebug<<"IsFromatsupp : NULL Delegate"<<endl;
		//This is probably wrong.
		return E_NOTIMPL;
	}

}
STDMETHODIMP BasicSeekable::QueryPreferredFormat(GUID* outFormat) {
	//if (inFormat == NULL) {
	//	return E_POINTER;
	//}

	//*outFormat = TIME_FORMAT_MEDIA_TIME;

	//return S_OK;
	if (mSeekDelegate != NULL) {
		seekDebug<<"Query preff : Passed on..."<<endl;
		return mSeekDelegate->QueryPreferredFormat(outFormat);
	} else {
		seekDebug<<"Query preff : NULL Delegate"<<endl;
		//This is probably wrong.
		return E_NOTIMPL;
	}

}
STDMETHODIMP BasicSeekable::SetTimeFormat(const GUID* inFormat) {

	if (mSeekDelegate != NULL) {
		seekDebug<<"SetTimeformat :Passed on..."<<endl;
		return mSeekDelegate->SetTimeFormat(inFormat);
	} else {
		seekDebug<<"SetTimeformat : NULL Delegate"<<endl;
		//This is probably wrong.
		return E_NOTIMPL;
	}
}

STDMETHODIMP  BasicSeekable::GetTimeFormat( GUID *outFormat) {
	if (mSeekDelegate != NULL) {
		seekDebug<<"Gettimeformat : Passed on..."<<endl;
		return mSeekDelegate->GetTimeFormat(outFormat);
	} else {
		seekDebug<<"Gettimeformat : NULL Delegate"<<endl;
		//This is probably wrong.
		return E_NOTIMPL;
	}
}
STDMETHODIMP BasicSeekable::GetDuration(LONGLONG* outDuration) {

	if (mSeekDelegate != NULL) {
		seekDebug<<"Getdureation : Passed on..."<<endl;
		return mSeekDelegate->GetDuration(outDuration);
	} else {
		seekDebug<<"Getdureation : NULL Delegate"<<endl;
		//This is probably wrong.
		return E_NOTIMPL;
	}
	//return E_NOTIMPL;
}
STDMETHODIMP BasicSeekable::GetStopPosition(LONGLONG* inStop) {
	if (mSeekDelegate != NULL) {
		seekDebug<<"Getstoppos : Passed on..."<<endl;
		return mSeekDelegate->GetStopPosition(inStop);
	} else {
		seekDebug<<"Getstoppos : NULL Delegate"<<endl;
		//This is probably wrong.
		return E_NOTIMPL;
	}
	//return E_NOTIMPL;
}
STDMETHODIMP BasicSeekable::GetCurrentPosition(LONGLONG* outCurrent) {
	//return E_NOTIMPL;
	
	if (mSeekDelegate != NULL) {
		seekDebug<<"Getcurrpos : Passed on..."<<endl;
		return mSeekDelegate->GetCurrentPosition(outCurrent);
	} else {
		seekDebug<<"Getcurrpos : NULL Delegate"<<endl;
		//This is probably wrong.
		return E_NOTIMPL;
	}
}
STDMETHODIMP BasicSeekable::ConvertTimeFormat(LONGLONG* outTarget, const GUID* inTargetFormat, LONGLONG inSource, const GUID* inSourceFormat) {
	//return E_NOTIMPL;
	if (mSeekDelegate != NULL) {
		seekDebug<<"ConvTimeformat : Passed on..."<<endl;
		return mSeekDelegate->ConvertTimeFormat(outTarget, inTargetFormat, inSource, inSourceFormat);
	} else {
		seekDebug<<"ConvTimeformat : NULL Delegate"<<endl;
		//This is probably wrong.
		return E_NOTIMPL;
	}
}	
STDMETHODIMP BasicSeekable::SetPositions(LONGLONG* inoutCurrent, DWORD inCurrentFlags, LONGLONG* inStop, DWORD inStopFlags) {
	if (mSeekDelegate != NULL) {
		seekDebug<<"Set pos : Requested Time : "<<*inoutCurrent<<endl;
		HRESULT locHR = mSeekDelegate->SetPositions(inoutCurrent, inCurrentFlags, inStop, inStopFlags);
		if (locHR == S_OK) {
			mSeekTimeBase = *inoutCurrent;
			seekDebug<<"Set Pos : Actual Time   : "<<mSeekTimeBase<<endl;
			
		}
		return locHR;
	} else {
		seekDebug<<"Set pos : NULL Delegate"<<endl;
		//This is probably wrong.
		return E_NOTIMPL;
	}

}
STDMETHODIMP BasicSeekable::GetPositions(LONGLONG* outCurrent, LONGLONG* outStop) {
	if (mSeekDelegate != NULL) {
		seekDebug<<"GetPos : Passed on..."<<endl;
		return mSeekDelegate->GetPositions(outCurrent, outStop);
	} else {
		seekDebug<<"GetPos : NULL Delegate"<<endl;
		//This is probably wrong.
		return E_NOTIMPL;
	}

}
STDMETHODIMP BasicSeekable::GetAvailable(LONGLONG* outEarliest, LONGLONG* outLatest) {
	if (mSeekDelegate != NULL) {
		seekDebug<<"Getavail : Passed on..."<<endl;
		return mSeekDelegate->GetAvailable(outEarliest, outLatest);
	} else {
		seekDebug<<"Getavail : NULL Delegate"<<endl;
		//This is probably wrong.
		return E_NOTIMPL;
	}
}
STDMETHODIMP BasicSeekable::SetRate(double inRate) {

	if (mSeekDelegate != NULL) {
		seekDebug<<"Setrate : Passed on..."<<endl;
		return mSeekDelegate->SetRate(inRate);
	} else {
		seekDebug<<"Setrate : NULL Delegate"<<endl;
		//This is probably wrong.
		return E_NOTIMPL;
	}
}
STDMETHODIMP BasicSeekable::GetRate(double* outRate) {
	if (mSeekDelegate != NULL) {
		seekDebug<<"Getrate : Passed on..."<<endl;
		return mSeekDelegate->GetRate(outRate);
	} else {
		seekDebug<<"Getrate : NULL Delegate"<<endl;
		//This is probably wrong.
		return E_NOTIMPL;
	}
}
STDMETHODIMP BasicSeekable::GetPreroll(LONGLONG* outPreroll) {
	if (mSeekDelegate != NULL) {
		seekDebug<<"Getpreroll : Passed on..."<<endl;
		return mSeekDelegate->GetPreroll(outPreroll);
	} else {
		seekDebug<<"Getpreroll : NULL Delegate"<<endl;
		//This is probably wrong.
		return E_NOTIMPL;
	}
}
STDMETHODIMP BasicSeekable::IsUsingTimeFormat(const GUID* inFormat) {
if (mSeekDelegate != NULL) {
		seekDebug<<"Isusingtimeformat : Passed on..."<<endl;
		return mSeekDelegate->IsUsingTimeFormat(inFormat);
	} else {
		seekDebug<<"Isusingtimeformat : NULL Delegate"<<endl;
		//This is probably wrong.
		return E_NOTIMPL;
	}
}
//
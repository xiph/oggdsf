#pragma once


#ifndef __IFLACENCODESETTINGS__
#define __IFLACENCODESETTINGS__

#ifdef __cplusplus
extern "C" {
#endif


DECLARE_INTERFACE_(IFLACEncodeSettings, IUnknown) {

    virtual STDMETHODIMP_(bool) canModifySettings() PURE;

    virtual STDMETHODIMP_(bool) setEncodingLevel(unsigned long inLevel) PURE;
    virtual STDMETHODIMP_(bool) setLPCOrder(unsigned long inLPCOrder) PURE;
    virtual STDMETHODIMP_(bool) setBlockSize(unsigned long inBlockSize) PURE;
    virtual STDMETHODIMP_(bool) useMidSideCoding(bool inUseMidSideCoding) PURE; //Only for 2 channels
    virtual STDMETHODIMP_(bool) useAdaptiveMidSideCoding(bool inUseAdaptiveMidSideCoding) PURE; //Only for 2 channels, overrides midside, is faster
    virtual STDMETHODIMP_(bool) useExhaustiveModelSearch(bool inUseExhaustiveModelSearch) PURE;
    virtual STDMETHODIMP_(bool) setRicePartitionOrder(unsigned long inMin, unsigned long inMax) PURE;

    //const vector<unsigned long>& getValidBlockSizes();

    //virtual STDMETHODIMP_(unsigned long) numChannels() PURE;
    //virtual STDMETHODIMP_(unsigned long) sampleRate() PURE;
    //virtual STDMETHODIMP_(unsigned long) bitsPerSample() PURE;

    virtual STDMETHODIMP_(long) encoderLevel() PURE;
    virtual STDMETHODIMP_(unsigned long) LPCOrder() PURE;
    virtual STDMETHODIMP_(unsigned long) blockSize() PURE;
    virtual STDMETHODIMP_(unsigned long) riceMin() PURE;
    virtual STDMETHODIMP_(unsigned long) riceMax() PURE;
    virtual STDMETHODIMP_(bool) isUsingMidSideCoding() PURE;
    virtual STDMETHODIMP_(bool) isUsingAdaptiveMidSideCoding() PURE;
    virtual STDMETHODIMP_(bool) isUsingExhaustiveModel() PURE;

};

#ifdef __cplusplus
}
#endif

#endif
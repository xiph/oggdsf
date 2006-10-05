#pragma once

#include <vector>

using namespace std;

class FLACEncoderSettings
{
public:
    FLACEncoderSettings(void);
    ~FLACEncoderSettings(void);
    
    bool setAudioParameters(unsigned long inNumChannels, unsigned long inSampleRate, unsigned long inBitsPerSample);
    bool setEncodingLevel(unsigned long inLevel);
    bool setLPCOrder(unsigned long inLPCOrder);
    bool setBlockSize(unsigned long inBlockSize);
    bool useMidSideCoding(bool inUseMidSideCoding); //Only for 2 channels
    bool useAdaptiveMidSideCoding(bool inUseAdaptiveMidSideCoding); //Only for 2 channels, overrides midside, is faster
    bool useExhaustiveModelSearch(bool inUseExhaustiveModelSearch);
    bool setRicePartitionOrder(unsigned long inMin, unsigned long inMax);

    const vector<unsigned long>& getValidBlockSizes();

    unsigned long numChannels() { return mNumChannels; }
    unsigned long sampleRate() { return mSampleRate; }
    unsigned long bitsPerSample() { return mBitsPerSample; }
    unsigned long LPCOrder() { return mLPCOrder; }
    unsigned long blockSize() { return mBlockSize; }
    unsigned long riceMin() { return mRiceMin; }
    unsigned long riceMax() { return mRiceMax; }
    bool isUsingMidSideCoding() { return mUsingMidSide; }
    bool isUsingAdaptiveMidSideCoding() { return mUsingAdaptiveMidSide; }
    bool isUsingExhaustiveModel() { return mUsingExhaustiveModelSearch; }

private:
    bool isValidBlockSize(unsigned long inBlockSize);
    void makeValidBlockSizeList();
    vector<unsigned long> mValidBlockSizes;

	unsigned long mNumChannels;
	unsigned long mSampleRate;
    unsigned long mBitsPerSample;

    unsigned long mLPCOrder;
    unsigned long mBlockSize;
    unsigned long mRiceMin;
    unsigned long mRiceMax;

    bool mUsingMidSide;
    bool mUsingAdaptiveMidSide;
    bool mUsingExhaustiveModelSearch;
};

#include "StdAfx.h"
#include "FLACEncoderSettings.h"

FLACEncoderSettings::FLACEncoderSettings(void)
    :   mNumChannels(0)
	,   mSampleRate(0)
    ,   mBitsPerSample(0)
    ,   mEncoderLevel(5)
    ,   mLPCOrder(8)
    ,   mBlockSize(4608)
    ,   mRiceMin(3)
    ,   mRiceMax(3)
    ,   mUsingMidSide(true)
    ,   mUsingAdaptiveMidSide(false)
    ,   mUsingExhaustiveModelSearch(false)
{
    makeValidBlockSizeList();

}

FLACEncoderSettings::~FLACEncoderSettings(void)
{
}

bool FLACEncoderSettings::setAudioParameters(unsigned long inNumChannels, unsigned long inSampleRate, unsigned long inBitsPerSample)
{
    //TODO::: Validate?
    mNumChannels = inNumChannels;
    mSampleRate = inSampleRate;
    mBitsPerSample = inBitsPerSample;
    return true;
}
bool FLACEncoderSettings::setEncodingLevel(unsigned long inLevel)
{
    struct sFLACDefaultSettings {
        unsigned long LPCOrder;
        unsigned long blockSize;
        unsigned long riceMin;
        unsigned long riceMax;
        bool useExhaustive;
        bool useMidSide;
        bool useAdaptiveMidside;
    };

    const sFLACDefaultSettings locDefaults[] = {
        { 0, 1152, 2, 2, false, false, false},      //0
        { 0, 1152, 2, 2, false, true, true},       //1
        { 0, 1152, 0, 3, false, true, false},       //2
        { 6, 4608, 3, 3, false, false, false},      //3
        { 8, 4608, 3, 3, false, true, true},       //4
        { 8, 4608, 3, 3, false, true, false},       //5
        { 8, 4608, 0, 4, false, true, false},       //6
        { 8, 4608, 0, 6, true, true, false},        //7
        { 12, 4608, 0, 6, true, true, false},       //8
    };
    bool locISOK = true;
    if (inLevel <= 8) {
        

        locISOK = locISOK && setLPCOrder(locDefaults[inLevel].LPCOrder);
        locISOK = locISOK && setBlockSize(locDefaults[inLevel].blockSize);
        locISOK = locISOK && setRicePartitionOrder(locDefaults[inLevel].riceMin, locDefaults[inLevel].riceMax);
        if (mNumChannels == 2) {
            locISOK = locISOK && useAdaptiveMidSideCoding(locDefaults[inLevel].useAdaptiveMidside);
            locISOK = locISOK && useMidSideCoding(locDefaults[inLevel].useMidSide);
        } else {
            useAdaptiveMidSideCoding(false);
            useMidSideCoding(false);
        }
        locISOK = locISOK && useExhaustiveModelSearch(locDefaults[inLevel].useExhaustive);
        mEncoderLevel = inLevel;
        return locISOK;

    }
    return false;
}
bool FLACEncoderSettings::setLPCOrder(unsigned long inLPCOrder)
{
    if (inLPCOrder <= 32) {
        mEncoderLevel = -1;
        mLPCOrder = inLPCOrder;
        return true;
    }
    return false;
}
bool FLACEncoderSettings::setBlockSize(unsigned long inBlockSize)
{
    if (isValidBlockSize(inBlockSize)) {
        mEncoderLevel = -1;
        mBlockSize = inBlockSize;
        return true;
    }
    return false;
}
bool FLACEncoderSettings::useMidSideCoding(bool inUseMidSideCoding)
{
    if (mNumChannels == 2) {
        mEncoderLevel = -1;
        mUsingMidSide = inUseMidSideCoding;

        if (!inUseMidSideCoding) {
            mUsingAdaptiveMidSide = false;
        }
        return true;
    } else {
        mUsingMidSide = false;
        return false;
    }
}
bool FLACEncoderSettings::useAdaptiveMidSideCoding(bool inUseAdaptiveMidSideCoding)
{
    if (mNumChannels == 2) {
        mEncoderLevel = -1;
        mUsingAdaptiveMidSide = inUseAdaptiveMidSideCoding;

        if (inUseAdaptiveMidSideCoding) {
            mUsingMidSide = true;
        }
        return true;
    } else {
        mUsingAdaptiveMidSide = false;
        return false;
    }
}
bool FLACEncoderSettings::useExhaustiveModelSearch(bool inUseExhaustiveModelSearch)
{
    mEncoderLevel = -1;
    mUsingExhaustiveModelSearch = inUseExhaustiveModelSearch;
    return true;
}
bool FLACEncoderSettings::setRicePartitionOrder(unsigned long inMin, unsigned long inMax)
{
    if (        (inMin <= 16)
            &&  (inMax <= 16)
            &&  (inMin <= inMax)) {
        mEncoderLevel = -1;
        mRiceMin = inMin;
        mRiceMax = inMax;
        return true;
    }
    return false;
}


const vector<unsigned long>& FLACEncoderSettings::getValidBlockSizes()
{
    return mValidBlockSizes;
}

bool FLACEncoderSettings::isValidBlockSize(unsigned long inBlockSize)
{
    for (size_t i = 0; i < mValidBlockSizes.size(); i++) {
        if (mValidBlockSizes[i] == inBlockSize) {
            return true;
        }
    }
    return false;
}
void FLACEncoderSettings::makeValidBlockSizeList()
{
    mValidBlockSizes.clear();
    mValidBlockSizes.push_back(192);
    mValidBlockSizes.push_back(576);
    mValidBlockSizes.push_back(1152);
    mValidBlockSizes.push_back(2304);
    mValidBlockSizes.push_back(4608);
    mValidBlockSizes.push_back(256);
    mValidBlockSizes.push_back(512);
    mValidBlockSizes.push_back(1024);
    mValidBlockSizes.push_back(2048);
    mValidBlockSizes.push_back(4096);
    mValidBlockSizes.push_back(8192);
    mValidBlockSizes.push_back(16384);
    mValidBlockSizes.push_back(32768);
}

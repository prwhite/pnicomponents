////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////

#include "pnifft.h"

////////////////////////////////////////////////////////////////////

namespace pni {
    
////////////////////////////////////////////////////////////////////

// #define    mSize                   (6)
// const int8_t mCoefficients[mSize] = {10, 4, -5, -9, -4, 5};
// int16_t mSamples[mSize] = {0};
// int16_t mReturn[mSize];

void Fft::init(size_t sampleNum, size_t bucketNum) {
    mSampleNum = sampleNum;
    mBucketNum = bucketNum;
    mCoefficients = {10, 4, -5, -9, -4, 5}; // should have number of elements that match with mReturn?
    mSamples.resize(mSampleNum, 0);
    mReturn.resize(mBucketNum, 0);
}

void Fft::deinit() {
    // Nothing for now
}

// Inspired by: http://blog.podkalicki.com/attiny13-dance-lights-with-fft/

void Fft::doFft(void)
{
    uint8_t a, k, n;

        // zero out return array
    mReturn.assign(mBucketNum, 0);

    for (k = 0; k <= (mBucketNum >> 1); ++k) {
        a = 0;
        for (n = 0; n < mSampleNum; ++n) {
            mReturn[k] += mCoefficients[a % mBucketNum] * mSamples[n];
            a += k;
        }
        if (mReturn[k] < 0)
            mReturn[k] = -(mReturn[k] + 1);
    }
}

////////////////////////////////////////////////////////////////////

} // end namespace pni


////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////

#ifndef pnifft_h
#define pnifft_h

#include <cstdint>
#include <cstddef>
#include <vector>

////////////////////////////////////////////////////////////////////

namespace pni {

////////////////////////////////////////////////////////////////////

class Fft {
    public:

        using BDatum = int8_t;
        using SDatum = int16_t;
        using BData = std::vector< BDatum >;
        using SData = std::vector< SDatum >;

        void init(size_t sampleNum = 6, size_t bucketNum = 6);
        void deinit();

        void update(SData const& samples) { mSamples = samples; }
        void updateSwap(SData& samples) { mSamples.swap(samples); }

        void doFft();
        
    protected:

        size_t mSampleNum;
        size_t mBucketNum;
        BData mCoefficients;    // Cos coefficients, scaled [-10,10]
        SData mSamples;         // Input samples
        SData mReturn;          // Return values

    private:

};

////////////////////////////////////////////////////////////////////

} // end namespace pni

#endif // pnifft_h


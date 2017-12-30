////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////

#ifndef pnifilters_h
#define pnifilters_h

#include <cstdint>
#include <cstddef>
#include <cmath>
#include <climits>
#include <cassert>
#include <vector>

#include "pnifixedpoint.h"
#include "pnifft.h"

////////////////////////////////////////////////////////////////////

namespace pni {

////////////////////////////////////////////////////////////////////

class Filter {
    public:
        using SDatum = FftSDatum;
        using SData = FftSData;

        // using Datum = FixedPoint< int32_t, 0, 15 >; // [0, 1)

            // src and dst not aliased
        virtual void apply(FftSData& dst, FftSData const& src) = 0;

    protected:

};

    // TODO: Switch floats to fixed point
class LowPassFilter : public Filter {

        float mAlpha = 0.9f;
        float mAlphaMinus = 0.1f;
        SDatum mRunningAvg = 0;
        bool uninit = true;
    public:
        
        LowPassFilter(float const& alpha = 0.9f) {
            setAlpha(alpha);
        }

        void setAlpha(float const& alpha) {
            mAlpha = alpha;
            mAlphaMinus = 1.0f - alpha;
        }

        virtual void apply(FftSData& dst, FftSData const& src) {
            assert(dst.size() == src.size());
            assert(src.size() > 0);

                // Seed the running average with a value.
            if(uninit) {
                mRunningAvg = src[ 0 ];
                uninit = false;
            }

            for(size_t num = 0; num < src.size(); ++num) {
                dst[ num ] = mRunningAvg = src[ num ] * mAlpha + mRunningAvg * mAlphaMinus;
            }
        }

    protected:

};

// TODO: Switch floats to fixed point
class HighPassFilter : public Filter {
    
        float mAlpha = 0.9f;
        float mAlphaMinus = 0.1f;
        SDatum mRunningAvg = 0;
        bool uninit = true;
    public:
        
        HighPassFilter(float const& alpha = 0.9f) {
            setAlpha(alpha);
        }

        void setAlpha(float const& alpha) {
            mAlpha = alpha;
            mAlphaMinus = 1.0f - alpha;
        }

        virtual void apply(FftSData& dst, FftSData const& src) {
            assert(dst.size() == src.size());
            assert(src.size() > 0);

                // Seed the running average with a value.
            if(uninit) {
                mRunningAvg = src[ 0 ];
                uninit = false;
            }

            for(size_t num = 0; num < src.size(); ++num) {
                mRunningAvg = src[ num ] * mAlpha + mRunningAvg * mAlphaMinus;
                dst[ num ] = src[ num ] - mRunningAvg;  // This is the only real diff between high pass and low pass
            }
        }

    protected:

};

class BiasCalcFilter : public Filter {

        SDatum mBias = 0;

    public:

        virtual void apply(FftSData& dst, FftSData const& src) {
            int32_t tBias = 0;
            for(auto val : src) {
                tBias += val;
            }
            mBias = tBias / src.size();
        }

        SDatum getBias() const { return mBias; }

    protected:

};

class BiasApplyFilter : public Filter {
    
            SDatum mBias = 0;
    
        public:
    
            virtual void apply(FftSData& dst, FftSData const& src) {
                assert(dst.size() == src.size());
                assert(src.size() > 0);

                auto end = src.size();
                for(size_t num = 0; num < end; ++num) {
                    dst[ num ] = src[ num ] - mBias;
                }
            }
    
            void setBias(SDatum bias) { mBias = bias; }
    
        protected:
    
    };

////////////////////////////////////////////////////////////////////

} // end namespace pni

#endif // pnifilters_h


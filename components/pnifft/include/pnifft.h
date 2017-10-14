////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////

#ifndef pnifft_h
#define pnifft_h

#include <cstdint>
#include <cstddef>
#include <cmath>
#include <climits>
#include <vector>

#include "pffft.h"

////////////////////////////////////////////////////////////////////

namespace pni {

////////////////////////////////////////////////////////////////////

/**
 * \brief    Fast Square root algorithm, with rounding
 *
 * This does arithmetic rounding of the result. That is, if the real answer
 * would have a fractional part of 0.5 or greater, the result is rounded up to
 * the next integer.
 *      - SquareRootRounded(2) --> 1
 *      - SquareRootRounded(3) --> 2
 *      - SquareRootRounded(4) --> 2
 *      - SquareRootRounded(6) --> 2
 *      - SquareRootRounded(7) --> 3
 *      - SquareRootRounded(8) --> 3
 *      - SquareRootRounded(9) --> 3
 *
 * \param[in] a_nInput - unsigned integer for which to find the square root
 *
 * \return Integer square root of the input value.
 */
 uint32_t SquareRootRounded(uint32_t a_nInput)
 {
     uint32_t op  = a_nInput;
     uint32_t res = 0;
     uint32_t one = 1uL << 30; // The second-to-top bit is set: use 1u << 14 for uint16_t type; use 1uL<<30 for uint32_t type
 
 
     // "one" starts at the highest power of four <= than the argument.
     while (one > op)
     {
         one >>= 2;
     }
 
     while (one != 0)
     {
         if (op >= res + one)
         {
             op = op - (res + one);
             res = res +  2 * one;
         }
         res >>= 1;
         one >>= 2;
     }
 
     /* Do arithmetic rounding to nearest integer */
     if (op > res)
     {
         res++;
     }
 
     return res;
 }

////////////////////////////////////////////////////////////////////

template< size_t Pow >
class Fft {
    public:

        static const size_t Num = 1 << Pow;
        static const size_t NumOut = Num >> 1;
        static const size_t Num2 = Num << 1;

        using SDatum = int16_t;
        using SData = std::vector< SDatum >;

        SData mReal;                // Input samples.   PUBLIC DATA!!!  Don't resize!!!
        
    protected:
            // Utility method.
        template< class Src, class Dst >
        void doCopy(Src const& src, Dst& dst, size_t srcStride =1, size_t dstStride = 1) {
            for(auto num = 0; num < Num; ++num) {
                dst[ num * dstStride ] = src[ num * srcStride ];
            }
        }
    
        constexpr float getHanningMult() const {
            return 1.0f / (float)(Num - 1);
        }

        float getPi() const {
            static float ret = 4.0f * atanf(1.0f); // http://www.cplusplus.com/forum/beginner/83485/
            return ret;
        }
        
        float calcHanningMod(size_t num) const {
            const float mult = getHanningMult();
            const float PiVal = getPi();
            return 0.5f * (1.0f - cosf(2.0f * PiVal * num * mult));
        }
        
        SData mHco; // Will hold Hanning coefficients if needed.

        void genHanningCoefficients() {
            if(mHco.empty()) {
                mHco.resize(Num);

                for(auto num = 0; num < Num; ++num) {
                    float mod = calcHanningMod(num);        // [0,1]
                    mHco[ num ] = (float) 0x8000 * mod;                       // [0,0x7fff]
                }
            }
        }

    public:

        virtual ~Fft() {}
        virtual void doFft() = 0;

            // From: https://www.edn.com/electronics-news/4383713/Windowing-Functions-Improve-FFT-Results-Part-I
            //  and: https://stackoverflow.com/questions/3555318/implement-hann-window
            // w(n)Hanning = 0.5 – 0.5cos(2pn/N)
            // Applies Hanning window to values currently in mReal.
            // Hanning window and FFT only work on data that is centered on zero,
            // so apply `bias` to any data that is not centered on zero.
            // The `bias` value will be subracted from all source values.
        void doHanningWindow(SDatum bias = 0) {
            genHanningCoefficients();

            for(auto num = 0; num < Num; ++num) {
                int32_t mod = mHco[ num ];
                int32_t val = mReal[ num ];
                int32_t bias32 = bias;

                mReal[ num ] = (((val - bias) * mod) >> 15);
            }
        }

        void doHanningWindowFloat(float bias) {
            for(auto num = 0; num < Num; ++num) {
                float mod = calcHanningMod(num);
                float val = mReal[ num ];

                mReal[ num ] = ((val - bias) * mod);
            }
        }

            // Find average (some kind of middle value) of
            // input data to use as bias in Hanning window calcs.
        SDatum calcBias() const {
            int32_t accum = 0;
            for(auto val : mReal) {
                accum += val;
            }
            accum /= mReal.size();
            return accum;
        }

            // After `doFtt`, convert real and imaginary bits to real-only.
            //  Only first half of array ( < index Num / 2) contain real data,
            //  the rest is set to zero.
        template< bool doSqrRoot = true >
        void convToReal() {
            for(size_t num = 0; num < Num; num += 2) {
                // do math using 32 bits so we don't overflow
                int32_t rval = (int32_t) mReal[ num ];
                int32_t ival = (int32_t) mReal[ num + 1 ];
                int32_t out = rval * rval + ival * ival;
                if (doSqrRoot) {
                    out = SquareRootRounded(out);
                }
                    // out is [-0x7ffe, 0x7ffe]
                    // mReal is [-0x7fff, 0x7fff]
                mReal[ num / 2 ] = out; // Can overrun... hmmm.
            }
            for(size_t num = NumOut; num < Num; ++num) {
                mReal[ num ] = 0;
            }
        }
};

////////////////////////////////////////////////////////////////////

template< size_t Pow >
class FftPffft : public Fft< Pow > {
        using Base = Fft< Pow >;
    public:
            // Need to promote the things we get from the template base class.
        using Base::Num;
        using Base::NumOut;
        using Base::Num2;

        using typename Base::SDatum;
        using typename Base::SData;

        using Base::mReal;
        
    private:
        using FDatum = float;
        using FData = std::vector< FDatum >;

        PFFFT_Setup* mSetup = 0;

        FData mIn;
        FData mOut;

    public:

        FftPffft() {
            mSetup = pffft_new_setup(Num, PFFFT_REAL);

            mReal.resize(Num, 0);
            mIn.resize(Num, 0.0f);
            mOut.resize(Num, 0.0f);
        }

        ~FftPffft() {
            // Currently no-op
        }


            // Does fwd fft.
            // mReal contains the input and will contain the output.
            // Output will be interleaved real and imaginary components.
        virtual void doFft() {
            this->doCopy(mReal, mIn);
            pffft_transform_ordered(mSetup, &mIn[ 0 ], &mOut[ 0 ], 0, PFFFT_FORWARD);
            this->doCopy(mOut, mReal);
        }


};

////////////////////////////////////////////////////////////////////

} // end namespace pni

#endif // pnifft_h

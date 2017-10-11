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
        using FDatum = float;
        using FData = std::vector< FDatum >;

        PFFFT_Setup* mSetup = 0;

        FData mIn;
        FData mOut;

        template< class Src, class Dst >
        void doCopy(Src const& src, Dst& dst, size_t srcStride =1, size_t dstStride = 1) {
            for(auto num = 0; num < Num; ++num) {
                dst[ num * dstStride ] = src[ num * srcStride ];
            }
        }

    public:

        static const size_t Num = 1 << Pow;
        static const size_t NumOut = Num >> 1;
        static const size_t Num2 = Num << 2;

        using SDatum = int16_t;
        using SData = std::vector< SDatum >;

            // Input and output arrays.  
            // Range of arrays is (-2^16, 2^16) represent [-1,1]
        SData mReal;                // Input samples.   PUBLIC DATA!!!  Don't resize!!!

        Fft() {
            mSetup = pffft_new_setup(Num, PFFFT_REAL);

            mReal.resize(Num, 0);
            mIn.resize(Num, 0.0f);
            mOut.resize(Num, 0.0f);
        }

        ~Fft() {
            // Currently no-op
        }


            // Does fwd fft
            // mReal and mImaginary should contain the input samples.
            // mReal and mImaginary will contain the output.
            //  Only first half of arrays ( < index Num / 2) contain valid data.
        void doFft() {
            doCopy(mReal, mIn);
            pffft_transform_ordered(mSetup, &mIn[ 0 ], &mOut[ 0 ], 0, PFFFT_FORWARD);
            doCopy(mOut, mReal);
        }

            // From: https://www.edn.com/electronics-news/4383713/Windowing-Functions-Improve-FFT-Results-Part-I
            //  and: https://stackoverflow.com/questions/3555318/implement-hann-window
            // w(n)Hanning = 0.5 – 0.5cos(2pn/N)
            // Applies Hanning window to values currently in mReal.
            // TODO: Make static table for this with int6_t.
            // TODO: Values are [0,SHRT_MAX], but mod should be aplied to values centered on zero.
        void doHanningWindow(SDatum bias = 0) {
            static SData hco = genHanningCoefficients();

            for(auto num = 0; num < Num; ++num) {
                int32_t mod = hco[ num ];
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

    private:
        constexpr float getHanningMult() const {
            return 1.0f / (float)(Num - 1);
        }

        float getPi() const {
            return 4.0f * atanf(1.0f); // http://www.cplusplus.com/forum/beginner/83485/
        }
        
        float calcHanningMod(size_t num) const {
            const float mult = getHanningMult();
            const float PiVal = getPi();
            return 0.5f * (1.0f - cosf(2.0f * PiVal * num * mult));
        }

        SData genHanningCoefficients() const {
            SData ret;
            ret.resize(Num);

            for(auto num = 0; num < Num; ++num) {
                float mod = calcHanningMod(num);        // [0,1]
                ret[ num ] = (float) 0x8000 * mod;                       // [0,0x7fff]
            }
            return ret;
        }

    public:

            // After `doFtt`, convert real and imaginary bits to real-only.
            //  Only first half of array ( < index Num / 2) contain valid data.
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

    protected:


    private:

};

////////////////////////////////////////////////////////////////////

} // end namespace pni

#endif // pnifft_h

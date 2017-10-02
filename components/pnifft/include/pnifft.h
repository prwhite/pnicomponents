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

#include "fix_fft.h"

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

        using SDatum = int16_t;
        using SData = std::vector< SDatum >;

        Fft() {
            mReal.resize(Num, 0);
            mImaginary.resize(Num, 0);        
        }

        ~Fft() {
            // Currently no-op
        }

            // Input and output arrays.  
            // Range of arrays is (-2^16, 2^16) represent [-1,1]
        SData mReal;                // Input samples.   PUBLIC DATA!!!  Don't resize!!!
        SData mImaginary;           // Imaginary values PUBLIC DATA!!!  Don't resize!!!
        
            // Does fwd fft
            // mReal and mImaginary should contain the input samples.
            // mReal and mImaginary will contain the output.
            //  Only first half of arrays ( < index Num / 2) contain valid data.
        void doFft() {
                // zero out imaginary component
            mImaginary.assign(Num, 0);
        
                // int fix_fft(int fr[], int fi[], int m, int inverse);
            fix_fft(&mReal[ 0 ], &mImaginary[ 0 ], Num, 0);
        }

            // After `doFtt`, convert real and imaginary bits to real-only.
            //  Only first half of array ( < index Num / 2) contain valid data.
        template< bool doSqrRoot = true >
        void convToReal() {
            for(size_t num = 0; num < NumOut; ++num) {
                    // do math using 32 bits so we don't overflow
                int32_t rval = (int32_t) mReal[ num ];
                int32_t ival = (int32_t) mImaginary[ num ];
                int32_t out = rval * rval + ival * ival;
                if (doSqrRoot) {
                    out = SquareRootRounded(out);
                }
                    // out is [-0x7ffe, 0x7ffe]
                    // mReal is [-0x7fff, 0x7fff]
                mReal[ num ] = out; // Can overrun... hmmm.
            }
        }

    protected:


    private:

};

////////////////////////////////////////////////////////////////////

} // end namespace pni

#endif // pnifft_h


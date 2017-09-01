////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////

#include "pnifixedpoint.h"

////////////////////////////////////////////////////////////////////

namespace pni {

////////////////////////////////////////////////////////////////////

#define DOFIXEDPOINTTEMPLATECOMPLIATIONTEST
#ifdef DOFIXEDPOINTTEMPLATECOMPLIATIONTEST

using Fpu3288 = FixedPoint< uint32_t, 8, 8>;
using Fp3278 = FixedPoint<  int32_t, 7, 8>;
using Fpu1644 = FixedPoint< uint16_t, 4, 4>;
using Fp1634 = FixedPoint<  int16_t, 3, 4>;

// For early development to get the template to instantiate
Fpu3288 fpu3288;
Fp3278 fp3278;
Fpu1644 fpu1644;
Fp1634 fp1634;
// static FixedPoint<  int16_t, 14, 4> fp1634;  // Error datatype not big enough for bits

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

void testTemplateExpansion() {
    fpu3288 += fpu3288;
    fpu3288 += 17;

    Fpu3288::ValueType out = fpu3288.get();
    float fout (fpu3288);
    double dout (fpu3288);

    // Test conversion constructors
    Fp3278 tmps(1);
    Fp3278 tmpf(1.0f);
    Fp3278 tmpd(1.0);

    // Test conversion operators
    // uint32_t tmpii(tmps);
    float tmpff(tmps);
    double tmpdd(tmps);

    Fpu3288 tmp;
    tmp = fpu3288;
    bool val00 = tmp == fpu3288;
    bool val01 = tmp != fpu3288;
    bool val02 = tmp == 33.77;
    
    bool val10 = tmp < fpu3288;
    auto val11 = tmp.get(0xff);
    auto val12 = tmp.get(0,255);
}
#pragma GCC diagnostic pop

#endif // DOFIXEDPOINTTEMPLATECOMPLIATIONTEST

////////////////////////////////////////////////////////////////////

} // end namespace pni

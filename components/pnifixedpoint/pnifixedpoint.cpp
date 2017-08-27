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
using Fp3288 = FixedPoint<  int32_t, 7, 8>;
using Fpu1644 = FixedPoint< uint16_t, 4, 4>;
using Fp1644 = FixedPoint<  int16_t, 3, 4>;

// For early development to get the template to instantiate
Fpu3288 fpu3288;
Fp3288 fp3288;
Fpu1644 fpu1644;
Fp1644 fp1644;
// static FixedPoint<  int16_t, 14, 4> fp1644;  // Error datatype not big enough for bits

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

void testTemplateExpansion() {
    fpu3288 += fpu3288;
    fpu3288 += 17;

    Fpu3288::ValueType out = fpu3288;
    float fout = fpu3288;
    double dout = fpu3288;

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

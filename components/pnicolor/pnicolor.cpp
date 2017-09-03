////////////////////////////////////////////////////////////////////
//
//
//
// Reference for some integer conversion math: 
//  https://stackoverflow.com/a/14733008/5305104
// 
////////////////////////////////////////////////////////////////////

#include "pnicolor.h"

#include <vector>

////////////////////////////////////////////////////////////////////

namespace pni {

    // local utility functions

template< typename Type >
inline static Type max(Type lhs, Type rhs) {
    return lhs > rhs ? lhs : rhs;
}

template< typename Type >
inline static Type min(Type lhs, Type rhs) {
    return lhs < rhs ? lhs : rhs;
}

inline static Color::Component clamp(Color::Component src, Color::Component lower, Color::Component upper) {
    return(max(upper, min(lower, src)));
}

////////////////////////////////////////////////////////////////////

Color::Rgb Color::Rgb::lerp(Color::Rgb const& lhs, Color::Rgb const& rhs, Component const& tval) {
    return Color::Rgb { 
        lhs.r + (rhs.r - lhs.r) * tval,
        lhs.g + (rhs.g - lhs.g) * tval,
        lhs.b + (rhs.b - lhs.b) * tval };
}

Color::Hsv Color::Hsv::lerp(Color::Hsv const& lhs, Color::Hsv const& rhs, Component const& tval) {
    return Color::Hsv { 
        lhs.h + (rhs.h - lhs.h) * tval,
        lhs.s + (rhs.s - lhs.s) * tval,
        lhs.v + (rhs.v - lhs.v) * tval };
}

Color::~Color() {

}

////////////////////////////////////////////////////////////////////

Color::Rgb Color::lerp(ColorRgb const& lhsColor, Color const& rhsColor, Component const& tval) {
    auto lhs = lhsColor.toRgb();    // no conversion, free
    auto rhs = rhsColor.toRgb();
    
    return Rgb::lerp(lhs, rhs, tval);
}

Color::Hsv Color::lerp(ColorHsv const& lhsColor, Color const& rhsColor, Component const& tval) {
    auto lhs = lhsColor.toHsv();    // no conversion, free
    auto rhs = rhsColor.toHsv();
    
    return Hsv::lerp(lhs, rhs, tval);
}

////////////////////////////////////////////////////////////////////

ColorRgb::ColorRgb(Color const& color) :
    mRgb(color.toRgb()) {

}

ColorRgb::ColorRgb(Rgb const& rgb) :
    mRgb(rgb) {
}
    
Color::Rgb ColorRgb::toRgb() const {
    return mRgb;
}

Color::Hsv ColorRgb::toHsv() const {
    Hsv hsv;

    Component maxVal = max(mRgb.r, max(mRgb.g, mRgb.b));    // [0,1]
    Component minVal = min(mRgb.r, min(mRgb.g, mRgb.b));    // [0,1]
    Component vd = maxVal - minVal;                         // [0,1]

    hsv.v = maxVal;                                         // [0,1]
    hsv.s = maxVal == 0 ? maxVal : vd / maxVal;             // [0,1]

    if (maxVal == minVal) {
        hsv.h = 0;
    } else {
        static const Component One(1);
        static const Component Two(2);
        static const Component Three(3);
        static const Component Six (6);
        static const Component OneThird(1.0f / 3.0f);
        static const Component TwoThird(2.0f / 3.0f);

        if (maxVal == mRgb.r) {
            hsv.h = ((mRgb.g - mRgb.b) / vd) / Six + One;           // [-1/6, 1/6] + 1
            hsv.h %= One;                                           // [0, 1) handle wrap around 1 -> 0
        } else if (maxVal == mRgb.g) {
            hsv.h = ((mRgb.b - mRgb.r) / vd) / Six + OneThird;      // [-1/6, 1/6] + 1/3
        } else if (maxVal == mRgb.b) {
            hsv.h = ((mRgb.r - mRgb.g) / vd) / Six + TwoThird;      // [-1/6, 1/6] + 2/3
        } else {
            hsv.h = 0;
        }
    }
    
    return hsv;
}

ColorRgb& ColorRgb::clamp(Rgb const& beg, Rgb const& end) {
    mRgb.r.clamp(beg.r, end.r);
    mRgb.g.clamp(beg.g, end.g);
    mRgb.b.clamp(beg.b, end.b);
    return *this;
}

////////////////////////////////////////////////////////////////////

ColorHsv::ColorHsv(Color const& color) :
    mHsv(color.toHsv()) {

}

ColorHsv::ColorHsv(Hsv const& hsv) :
    mHsv(hsv) {
    
}
    
Color::Rgb ColorHsv::toRgb() const {
    static const auto table = std::vector< Rgb > { { 1, 0, 0 }, { 1, 1, 0 }, { 0, 1, 0 }, { 0, 1, 1 }, { 0, 0, 1 }, { 1, 0, 1 } };

    Component hn6 = mHsv.h % 1;     // [0,1), can do that because hue wraps around anyway
    hn6 *= 6;                       // [0,6)
    Component hd = hn6 % 1;         // [0,1), delta between lhi and rhi
    size_t lhi = hn6.get();         // [0,5] implicit truncation of fractional part gives index into table
    size_t rhi = (lhi + 1) % 6;     // [0,5], handles wrap-around

    auto rgbFull = Color::Rgb::lerp(Rgb(table[lhi]), Rgb(table[rhi]), hd);
    auto rgbSat = Color::Rgb::lerp(rgbFull, Rgb( {1, 1, 1} ), Component(1) - mHsv.s);
    auto rgbVal = Color::Rgb::lerp(rgbSat, Rgb( {0, 0, 0} ), Component(1) - mHsv.v);
    
    return rgbVal;
}

Color::Hsv ColorHsv::toHsv() const {
    return mHsv;
}

ColorHsv& ColorHsv::clamp(Hsv const& beg, Hsv const& end) {
    mHsv.h.clamp(beg.h, end.h);
    mHsv.s.clamp(beg.s, end.s);
    mHsv.v.clamp(beg.v, end.v);
    return *this;
}

////////////////////////////////////////////////////////////////////

} // end namespace pni

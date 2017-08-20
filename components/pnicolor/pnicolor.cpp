////////////////////////////////////////////////////////////////////

#include "pnicolor.h"

////////////////////////////////////////////////////////////////////

namespace pni {

////////////////////////////////////////////////////////////////////

Color::~Color() {

}

Color::Rgb lerp(ColorRgb const& lhs, Color const&rhs) {
    // TODO
}

Color::Hsv lerp(ColorHsv const& lhs, Color const&rhs) {
    // TODO
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
    Hsv ret;
    // TODO
    return ret;
}

////////////////////////////////////////////////////////////////////

ColorHsv::ColorHsv(Color const& color) :
    mHsv(color.toHsv()) {

}

ColorHsv::ColorHsv(Hsv const& hsv) :
    mHsv(hsv) {
    
}
    
Color::Rgb ColorHsv::toRgb() const {
    Rgb ret;
    // TODO
    return ret;
}

Color::Hsv ColorHsv::toHsv() const {
    return mHsv;
}


////////////////////////////////////////////////////////////////////

} // end namespace pni

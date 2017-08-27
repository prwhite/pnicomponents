////////////////////////////////////////////////////////////////////
//
//  Working really hard to have polymorphic types, which are convenient
//  in some situations, while preserving good value semantics so
//  instances of Color and derived classes generally don't need
//  to be on the heap.
//
////////////////////////////////////////////////////////////////////

#include <cstdint>
#include <cstddef>
#include "pnifixedpoint.h"

////////////////////////////////////////////////////////////////////

namespace pni {

////////////////////////////////////////////////////////////////////

class Color;
class ColorRgb;
class ColorHsv;

class Color {
    public:
        virtual ~Color() = 0;   // Really don't need this, derived classes do no allocation.

        using Component = FixedPoint< int32_t, 5, 10 >;

        struct Rgb {
            Component g;
            Component r;
            Component b;

            static Rgb lerp(Rgb const& lhs, Rgb const& rhs, Component const& tval);
        };

        struct Hsv {
            Component h;
            Component s;
            Component v;

            static Hsv lerp(Hsv const& lhs, Hsv const& rhs, Component const& tval);
        };

            // Conversion operators, simply dispatches to conversion functions below.
        operator Rgb () const { return toRgb(); }
        operator Hsv () const { return toHsv(); }

            // Linear interpolation.
            // TRICKY: keyed off of type of first argument.
        static Rgb lerp(ColorRgb const& lhs, Color const&rhs, Component const& tval);
        static Hsv lerp(ColorHsv const& lhs, Color const&rhs, Component const& tval);

            // Conversion functions.
        virtual Rgb toRgb() const = 0;
        virtual Hsv toHsv() const = 0;

    protected:


    private:
};

class ColorRgb final : public Color {
    public:
        ColorRgb(Color const& color);
        ColorRgb(Rgb const& rgb);

        virtual Rgb toRgb() const final;
        virtual Hsv toHsv() const final;

    private:
        Rgb mRgb;
};

class ColorHsv final : public Color {
    public:
        ColorHsv(Color const& color);
        ColorHsv(Hsv const& hsv);

        virtual Rgb toRgb() const final;
        virtual Hsv toHsv() const final;
        
    private:
        Hsv mHsv;
};

////////////////////////////////////////////////////////////////////

} // end namespace pni

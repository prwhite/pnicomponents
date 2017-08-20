////////////////////////////////////////////////////////////////////
//
//  Working really hard to have polymorphic types, which are convenient
//  in some situations, while preserving good value semantics so
//  instances of Color and derived classes generally don't need
//  to be on the heap.
//
////////////////////////////////////////////////////////////////////

#include <cstdint>

////////////////////////////////////////////////////////////////////

namespace pni {

////////////////////////////////////////////////////////////////////

class Color;
class ColorRgb;
class ColorHsv;

class Color {
    public:
        virtual ~Color() = 0;

        using Component = uint16_t; // Works with any bit size <= 16.  Internal math is 32 bit and protects against overflows.

        struct Rgb {
            Component r = 0;
            Component g = 0;
            Component b = 0;

            Rgb& operator += (int amount) { r += amount; g += amount; b += amount; return *this; }
            Rgb& operator -= (int amount) { r -= amount; g -= amount; b -= amount; return *this; }
            
            Rgb& operator >>=(int count) { r >>= count; g >>= count; b >>= count;  return *this; }
            Rgb& operator <<=(int count) { r <<= count; g <<= count; b <<= count;  return *this; }
        };

        struct Hsv {
            Component h = 0;
            Component s = 0;
            Component v = 0;

            Hsv& operator += (int amount) { h += amount; s += amount; v += amount; return *this; }
            Hsv& operator -= (int amount) { h -= amount; s -= amount; v -= amount; return *this; }

            Hsv& operator >>=(int count) { h >>= count; s >>= count; v >>= count;  return *this; }
            Hsv& operator <<=(int count) { h <<= count; s <<= count; v <<= count;  return *this; }
        };

            // Conversion operators, simply dispatches to conversion functions below.
        operator Rgb () const { return toRgb(); }
        operator Hsv () const { return toHsv(); }

            // Linear interpolation.
            // TRICKY: keyed off of type of first argument.
        static Rgb lerp(ColorRgb const& lhs, Color const&rhs);
        static Hsv lerp(ColorHsv const& lhs, Color const&rhs);

            // Conversion functions.
        virtual Rgb toRgb() const = 0;
        virtual Hsv toHsv() const = 0;

    protected:


    private:
};

class ColorRgb : public Color {
    public:
        ColorRgb(Color const& color);
        ColorRgb(Rgb const& rgb);

        virtual Rgb toRgb() const;
        virtual Hsv toHsv() const;

    private:
        Rgb mRgb;
};

class ColorHsv : public Color {
    public:
        ColorHsv(Color const& color);
        ColorHsv(Hsv const& hsv);

        virtual Rgb toRgb() const;
        virtual Hsv toHsv() const;
    private:
        Hsv mHsv;
};

////////////////////////////////////////////////////////////////////

} // end namespace pni

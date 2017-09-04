////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////

#ifndef pnifixedpoint_h
#define pnifixedpoint_h

#include <cstdint>
#include <cstddef>
#include <type_traits>

////////////////////////////////////////////////////////////////////

namespace pni {

////////////////////////////////////////////////////////////////////

template< typename Type, size_t IntBits, size_t FracBits, typename _SrcType = Type >
class FixedPoint {
    private:
        static const Type   Scale = 1 << FracBits;
        static const size_t SignBits = std::is_unsigned<Type>::value ? 0 : 1;
        static const size_t TypeBits = sizeof(Type) * 8;

        static_assert((IntBits + FracBits + SignBits) <= TypeBits / 2, "Sizes too big for data type");

    public:
        using ValueType = Type;
        using SrcType = _SrcType;

        // Convenient values and generators
        static const Type   RawOneVal = Scale;
        static FixedPoint const& oneVal() { static FixedPoint val(1); return val; }
        static FixedPoint const& epsVal() { static FixedPoint val(1, Raw); return val;  }
        
        FixedPoint() : mVal ( 0 ) {}
        FixedPoint(FixedPoint const& rhs) : mVal( rhs.mVal ) {}

    private:
        // Internal constructor for raw values.
        // Uses dumb enum to make sure right constructor is invoked.
        enum ConstructorType { Raw };
        FixedPoint(SrcType const& val, ConstructorType dummy) : mVal ( val ) {}   // Pre-scaled SrcType
    
    public:        
        // Conversion constructors, _currently_ with no range checking.
        FixedPoint(SrcType const& val) : mVal ( val * Scale ) {}                // Unscaled SrcType
        explicit FixedPoint(float const& val) : mVal ( val * Scale ) {}         // Unscaled float
        explicit FixedPoint(double const& val) : mVal ( val * Scale ) {}        // Unscaled double

        // Assignment operators
        FixedPoint& operator = (SrcType const& rhs) { mVal = rhs * Scale; return *this; }
        FixedPoint& operator = (FixedPoint const& rhs) { mVal = rhs.mVal; return *this; }
        FixedPoint& operator = (float const& rhs) { mVal = rhs * Scale; return *this; }
        FixedPoint& operator = (double const& rhs) { mVal = rhs * Scale; return *this; }
        
        // Arithmetic operations
        FixedPoint operator + (FixedPoint const& rhs) const { return FixedPoint(mVal + rhs.mVal, Raw); }
        FixedPoint& operator += (FixedPoint const& rhs) { mVal += rhs.mVal; return *this; }
        
        FixedPoint operator - (FixedPoint const& rhs) const { return FixedPoint(mVal - rhs.mVal, Raw); }
        FixedPoint& operator -= (FixedPoint const& rhs) { mVal -= rhs.mVal; return *this; }

        FixedPoint operator * (FixedPoint const& rhs) const { return FixedPoint(mVal * rhs.mVal / Scale, Raw); }
        FixedPoint& operator *= (FixedPoint const& rhs) { mVal *= rhs.mVal; mVal /= Scale; return *this; }

        FixedPoint operator / (FixedPoint const& rhs) const { return FixedPoint((mVal * Scale) / rhs.mVal, Raw); }
        FixedPoint& operator /= (FixedPoint const& rhs) { mVal *= Scale; mVal /= rhs.mVal; return *this; }

        FixedPoint operator % (FixedPoint const& rhs) const { return FixedPoint(mVal % rhs.mVal, Raw); }
        FixedPoint& operator %= (FixedPoint const& rhs) { mVal %= rhs.mVal; return *this; }
        
        // Conversion operators
        explicit operator SrcType () const { return mVal / Scale; }
        explicit operator float () const { return mVal / (float) Scale; }
        explicit operator double () const { return mVal / (double) Scale; }

        // Comparison operators
        bool operator == (FixedPoint const& rhs) const { return mVal == rhs.mVal; }
        bool operator != (FixedPoint const& rhs) const { return ! (*this == rhs); }
        bool operator == (SrcType const& rhs) const { return mVal == rhs * Scale; }
        bool operator == (float const& rhs) const { return mVal == rhs * Scale; }
        bool operator == (double const& rhs) const { return mVal == rhs * Scale; }

        bool eq (float const& compare, float const& eps = 0.01) const {
            float val = this->getFloat();
            return ((val + eps) > compare) && ((val - eps) < compare);
        }

        bool eq (double const& compare, double const& eps = 0.01) const {
            double val = this->getDouble();
            return ((val + eps) > compare) && ((val - eps) < compare);
        }

        bool operator < (FixedPoint const& rhs) const { return mVal < rhs.mVal; }
        bool operator <= (FixedPoint const& rhs) const { return mVal <= rhs.mVal; }
        bool operator > (FixedPoint const& rhs) const { return mVal > rhs.mVal; }
        bool operator >= (FixedPoint const& rhs) const { return mVal >= rhs.mVal; }
        
        // Other operators
        explicit operator bool () { return mVal ? true : false; }
        FixedPoint operator - () const { return FixedPoint(-mVal, Raw); }

        // Get data masked/clamped
        SrcType get () const { return mVal / Scale; }
        float getFloat() const { return mVal / (float) Scale; }
        double getDouble() const { return mVal / (double) Scale; }

        SrcType get(ValueType const& mask) const { return ( mVal / Scale ) & mask; }
        SrcType get(SrcType minVal, SrcType maxVal) const {
            FixedPoint tmp(*this);
            tmp.clamp(minVal, maxVal);
            return tmp.get();
        }
        FixedPoint& clamp(SrcType const& minVal, SrcType const& maxVal) {
            ValueType smin = minVal * Scale;
            ValueType smax = maxVal * Scale;
            mVal = mVal > smin ? mVal : smin;
            mVal = mVal < smax ? mVal : smax;
            return *this;
        }

        FixedPoint& clamp(FixedPoint const& minVal, FixedPoint const& maxVal) {
            mVal = mVal > minVal.mVal ? mVal : minVal.mVal;
            mVal = mVal < maxVal.mVal ? mVal : maxVal.mVal;
            return *this;
        }

        FixedPoint& clampRaw(ValueType const& minVal, ValueType const& maxVal) {
            mVal = mVal > minVal ? mVal : minVal;
            mVal = mVal < maxVal ? mVal : maxVal;
            return *this;
        }

        // Concept: Truncate number by zero-ing out last numBits bits
        // It's a template to provide performance and static range-checking.
        template< size_t numBits >
        FixedPoint& truncate() {
            static_assert(numBits <= (IntBits + FracBits), "trying to truncate more bits than available");

            size_t mask = 1 << numBits;     // E.g., 0b00100000
            mask -= 1;                      // E.g., 0b00011111
            mask = ~mask;                   // E.g., 0b11100000

            // This won't alter the sign bit for signed types
            mVal = mVal &= mask;           

            return *this;
        }

        // Set/get raw bytes without being unscaled first
        FixedPoint& setRaw(ValueType const& val) { mVal = val; return *this; }
        SrcType getRaw() const { return mVal; }
        SrcType getRaw(ValueType const& mask) const { return ( mVal ) & mask; }

    protected:

    private:
        Type mVal;
};

// Some predefined versions of the template.
using FixedPointU3288 = FixedPoint< uint32_t, 8, 8 >;
using FixedPoint3278 = FixedPoint< int32_t, 7, 8 >;
using FixedPointU1644 = FixedPoint< uint16_t, 4, 4 >;
using FixedPoint1634 = FixedPoint< uint16_t, 3, 4 >;

////////////////////////////////////////////////////////////////////

} // end namespace pni

#endif // pnifixedpoint_h

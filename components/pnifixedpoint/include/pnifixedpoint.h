////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////

#include <cstdint>
#include <cstddef>
#include <type_traits>

////////////////////////////////////////////////////////////////////

namespace pni {

////////////////////////////////////////////////////////////////////

template< typename Type, size_t IntBits, size_t FracBits, typename _SrcType = Type >
class FixedPoint {
    private:
        static const size_t Scale = 1 << FracBits;
        static const size_t SignBits = std::is_unsigned<Type>::value ? 0 : 1;
        static const size_t TypeBits = sizeof(Type) * 8;

        static_assert((IntBits + FracBits + SignBits) <= TypeBits / 2, "Sizes too big for data type");

    public:
        using ValueType = Type;
        using SrcType = _SrcType;

        FixedPoint() : mVal ( 0 ) {}
        FixedPoint(FixedPoint const& rhs) : mVal( rhs.mVal ) {}

    private:
        // Internal constructor for raw values.
        // Uses dumb enum to make sure right constructor is invoked.
        enum ConstructorType { Scaled };
        FixedPoint(SrcType const& val, ConstructorType dummy) : mVal ( val ) {}   // Scaled SrcType
    
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
        FixedPoint operator + (FixedPoint const& rhs) const { return FixedPoint(mVal + rhs.mVal, Scaled); }
        FixedPoint& operator += (FixedPoint const& rhs) { mVal += rhs.mVal; return *this; }
        
        FixedPoint operator - (FixedPoint const& rhs) const { return FixedPoint(mVal - rhs.mVal, Scaled); }
        FixedPoint& operator -= (FixedPoint const& rhs) { mVal -= rhs.mVal; return *this; }

        FixedPoint operator * (FixedPoint const& rhs) const { return FixedPoint(mVal * rhs.mVal / Scale, Scaled); }
        FixedPoint& operator *= (FixedPoint const& rhs) { mVal *= rhs.mVal; mVal /= Scale; return *this; }

        FixedPoint operator / (FixedPoint const& rhs) const { return FixedPoint((mVal * Scale) / rhs.mVal, Scaled); }
        FixedPoint& operator /= (FixedPoint const& rhs) { mVal *= Scale; mVal /= rhs.mVal; return *this; }

        FixedPoint operator % (FixedPoint const& rhs) const { return FixedPoint(mVal % rhs.mVal, Scaled); }
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

        bool operator < (FixedPoint const& rhs) const { return mVal < rhs.mVal; }
        bool operator <= (FixedPoint const& rhs) const { return mVal <= rhs.mVal; }
        bool operator > (FixedPoint const& rhs) const { return mVal > rhs.mVal; }
        bool operator >= (FixedPoint const& rhs) const { return mVal >= rhs.mVal; }
        
        // Other operators
        explicit operator bool () { return mVal ? true : false; }
        FixedPoint operator - () const { return FixedPoint(-mVal, Scaled); }

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
        FixedPoint& clamp(SrcType minVal, SrcType maxVal) {
            ValueType smin = minVal * Scale;
            ValueType smax = maxVal * Scale;
            mVal = mVal > smin ? mVal : smin;
            mVal = mVal < smax ? mVal : smax;
            return *this;
        }

        // Set/get raw bytes without being unscaled first
        FixedPoint& setUnscaled(ValueType const& val) { mVal = val; return *this; }
        SrcType getUnscaled() const { return mVal; }
        SrcType getUnscaled(ValueType const& mask) const { return ( mVal ) & mask; }

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

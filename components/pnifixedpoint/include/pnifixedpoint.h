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

        // Counting on RVO to make this not be a big performance problem.
        static FixedPoint make(Type val) { return FixedPoint(static_cast< Type >(val * Scale)); }

    public:
        using ValueType = Type;
        using SrcType = _SrcType;

        FixedPoint() : mVal ( 0 ) {}
        FixedPoint (FixedPoint const& rhs) : mVal( rhs.mVal ) {}
        
        // Conversion constructors, _currently_ with no range checking.
        FixedPoint(SrcType const& val) : mVal ( val * Scale ) {}        // Unscaled SrcType
        FixedPoint(float const& val) : mVal ( val * Scale ) {}          // Unscaled float
        FixedPoint(double const& val) : mVal ( val * Scale ) {}         // Unscaled double

        // Assignment operators
        FixedPoint& operator =  (SrcType const& val) { mVal = val * Scale; return *this; }
        FixedPoint& operator = (FixedPoint const& rhs) { mVal = rhs.mVal; return *this; }

        // Arithmetic operations
        FixedPoint operator + (FixedPoint const& rhs) const { return make(mVal + rhs.mVal); }
        FixedPoint operator + (SrcType const& rhs) const { return make(mVal + rhs * Scale); }
        FixedPoint& operator += (FixedPoint const& rhs) { mVal += rhs.mVal; return *this; }
        FixedPoint& operator += (SrcType const& rhs) { mVal += rhs * Scale; return *this; }
        
        FixedPoint operator - (FixedPoint const& rhs) const { return make(mVal - rhs.mVal); }
        FixedPoint operator - (SrcType const& rhs) const { return make(mVal - rhs * Scale); }
        FixedPoint& operator -= (FixedPoint const& rhs) { mVal -= rhs.mVal; return *this; }
        FixedPoint& operator -= (SrcType const& rhs) { mVal -= rhs * Scale; return *this; }

        FixedPoint operator * (FixedPoint const& rhs) const { return make(mVal * rhs.mVal / Scale); }
        FixedPoint operator * (SrcType const& rhs) const { return make(mVal * rhs); }
        FixedPoint& operator *= (FixedPoint const& rhs) { mVal *= rhs.mVal; mVal /= Scale; return *this; }
        FixedPoint& operator *= (SrcType const& rhs) { mVal *= rhs; return *this; }

        FixedPoint operator / (FixedPoint const& rhs) const { return make(mVal * Scale / rhs.mVal); }
        FixedPoint operator / (SrcType const& rhs) const { return make(mVal / rhs); }
        FixedPoint& operator /= (FixedPoint const& rhs) { mVal *= Scale; mVal /= rhs.mVal; return *this; }
        FixedPoint& operator /= (SrcType const& rhs) { mVal /= rhs; return *this; }

        SrcType operator % (SrcType const& rhs) const { return mVal % ((ValueType)rhs * Scale); }
        FixedPoint& operator %= (SrcType const& rhs) { mVal %= (rhs * Scale); return *this; }
        SrcType operator % (FixedPoint const& rhs) const { return mVal % rhs.mVal; }
        FixedPoint& operator %= (FixedPoint const& rhs) { mVal %= rhs; return *this; }

        // Conversion operators
        operator SrcType () const { return mVal / Scale; }
        operator float () const { return mVal / (float) Scale; }
        operator double () const { return mVal / (double) Scale; }

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
        operator bool () { return mVal ? true : false; }
        FixedPoint operator - () const { return make(-mVal); }

        // Get data masked/clamped
        SrcType get(ValueType const& mask) const { return ( mVal / Scale ) & mask; }
        SrcType get(SrcType minVal, SrcType maxVal) const {
            FixedPoint tmp(*this);
            tmp.clamp(minVal, maxVal);
            return tmp;
        }
        FixedPoint& clamp(SrcType minVal, SrcType maxVal) {
            ValueType smin = minVal * Scale;
            ValueType smax = maxVal * Scale;
            mVal = mVal > smin ? mVal : smin;
            mVal = mVal < smax ? mVal : smax;
            return *this;
        }


        // Get raw bytes without being unscaled first
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


#include <iostream>

#include "microtest/microtest.h"

#include "pnifixedpoint.h"

using namespace std;
using namespace pni;

using u88_t = FixedPoint< uint32_t, 8, 8 >;
using s78_t = FixedPoint< int32_t, 7, 8 >;

TEST(basicValueTest) {
    u88_t u880(0u);
    ASSERT_EQ(u880.get(), 0);

    u880 = 1u;
    ASSERT_EQ(u880.get(), 1);

    u880.setRaw(0xff);         // should truncate to 0
    ASSERT_EQ(u880.get(), 0);

    u880 = 1.0f;
    ASSERT_EQ(u880.get(), 1);

    u880 = 1.5f;
    ASSERT_EQ(u880.getRaw(), 0x180);
}

TEST(basicArithmetic) {
    u88_t u880(0u);

    // +
    u880 = u880 + 3u;
    ASSERT_EQ(u880.get(), 3);

    // -
    u880 = u880 - 1u;
    ASSERT_EQ(u880.get(), 2);

    // *
    u880 = u880 * 2u;
    ASSERT_EQ(u880.get(), 4);
    
    // /
    u880 = u880 / 4u;
    ASSERT_EQ(u880.get(), 1);

    s78_t pos(1);
    s78_t neg(-pos);
    ASSERT_EQ(neg.get(), -1);

    s78_t val(30);
    val -= s78_t(-10);
    ASSERT_EQ(val.get(), 40);
}

TEST(basicArithmeticAssigment) {
    u88_t u880(0u);

    // +
    u880 += 3u;
    ASSERT_EQ(u880.get(), 3);

    // -
    u880 -= 1u;
    ASSERT_EQ(u880.get(), 2);

    // *
    u880 *= 2u;
    ASSERT_EQ(u880.get(), 4);

    // /
    u880 /= 4u;
    ASSERT_EQ(u880.get(), 1);
}

TEST(comparisons) {
    u88_t lhs(2.7);
    u88_t rhs(7.3);

    ASSERT_TRUE(lhs < rhs);
    ASSERT_TRUE(rhs > lhs);

    ASSERT_TRUE(lhs <= rhs);
    ASSERT_TRUE(rhs >= lhs);

    ASSERT_TRUE(lhs != rhs);

    // Operator bool
    ASSERT_TRUE((bool)lhs);

    u88_t fffi(0u);
    ASSERT_FALSE((bool)fffi);

    u88_t ffff(0.0);
    ASSERT_FALSE((bool)ffff);

    ASSERT_TRUE(lhs.eq(2.7f));
    ASSERT_TRUE(rhs.eq(7.3f));
}

TEST(gets) {
    u88_t flt(19.3f);
    u88_t dbl(17.7);

    // float fltOut(flt);
    // double dblOut(dbl);

    // TODO: There's no epsilon eq for fixed point yet
    ASSERT_TRUE(flt.eq(19.3f));
    ASSERT_TRUE(dbl.eq(17.7, 0.1));

    ASSERT_TRUE(u88_t(0.0f).eq(0.0));

    u88_t clampIn(1.3);
    ASSERT_EQ(clampIn.get(0,1),1);
    ASSERT_EQ(clampIn.clamp(0,1).get(), 1);

    u88_t clampDown1(1u);
    u88_t clampDown0(0u);
    clampDown1.clampDown();
    clampDown0.clampDown();
    ASSERT_EQ(clampDown1.getRaw(), 0xff);
    ASSERT_EQ(clampDown0.getRaw(), 0x00);
    
    u88_t roundNum(1.77777777);
    roundNum.truncate<7>();
    u88_t::ValueType raw = roundNum.getRaw();
    ASSERT_EQ(raw, 0x180);  // = 0b01 1000 0000
    ASSERT_EQ(roundNum.getFloat(), 1.5f);
}

TEST(constants) {
    u88_t u880(1u);
    ASSERT_EQ(u880.getRaw(), 0x100);

    cout << "u88_t::Mask = " << hex << u88_t::maskVal().getRaw() << endl;
    cout << "s78_t::Mask = " << hex << s78_t::maskVal().getRaw() << endl;
    
    u88_t one88 = u88_t::oneVal();
    u88_t min88 = u88_t::minVal();
    u88_t max88 = u88_t::maxVal();

    cout << "max88 min max = " << min88.getFloat() << ", " << max88.getFloat() << endl;

    ASSERT_EQ(one88.getRaw(), 0x100);
    ASSERT_EQ(min88.getRaw(), 0x00);
    ASSERT_EQ(max88.getRaw(), 0xffff);

    s78_t one78 = s78_t::oneVal();
    s78_t min78 = s78_t::minVal();
    s78_t max78 = s78_t::maxVal();

    cout << "max78 min max = " << min78.getFloat() << ", " << max78.getFloat() << endl;
    
    ASSERT_EQ(one78.getRaw(), 0x100);
    ASSERT_EQ(min78.getRaw(), -1 * 0x7fff);
    ASSERT_EQ(max78.getRaw(), 0x7fff);
}

TEST_MAIN();

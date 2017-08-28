
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

    u880.setUnscaled(0xff);         // should truncate to 0
    ASSERT_EQ(u880.get(), 0);

    u880 = 1.0f;
    ASSERT_EQ(u880.get(), 1);

    u880 = 1.5f;
    ASSERT_EQ(u880.getUnscaled(), 0x180);
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


TEST(unscaledValues) {
    u88_t u880(1u);

    ASSERT_EQ(u880.getUnscaled(), 0x100);
}

TEST_MAIN();

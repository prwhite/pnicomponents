
#include <iostream>

#include "microtest/microtest.h"

#include "pnifixedpoint.h"
#include "pnicolor.h"

using namespace std;
using namespace pni;

static const Color::Component OneVal(1);

TEST(basicValueTest) {
    ColorRgb crgb0 = { { 0, 0, 0 } };
    ColorRgb crgb1 = { { 1, 1, 1 } };
    ColorHsv chsv0 = { { 0, 1, 1 } };
    ColorHsv chsv1 = { { 1, 1, 1 } };

    ASSERT_EQ(crgb0.toRgb().r.getRaw(), 0x00);
    ASSERT_EQ(crgb1.toRgb().r.getRaw(), OneVal.getRaw());
    ASSERT_EQ(chsv0.toHsv().h.getRaw(), 0x00);
    ASSERT_EQ(chsv1.toHsv().h.getRaw(), OneVal.getRaw());
}

TEST(lerpTest1) {
    ColorRgb crgb0 = { { 0, 0, 0 } };
    ColorRgb crgb1 = { { 1, 1, 1 } };

    auto rgbOut = Color::lerp(crgb0, crgb1, Color::Component(0.5f));
    auto rOut = rgbOut.r.getFloat();
    ASSERT_EQ(rOut, 0.5f);
}

TEST(lerpTest2) {
    ColorHsv chsv0 = { { 0, 1, 1 } };
    ColorHsv chsv1 = { { 1, 1, 1 } };

    auto hsvOut = Color::lerp(chsv0, chsv1, Color::Component(1.0f / 3.0f));
    auto rOut = hsvOut.h;
    ASSERT_TRUE(rOut.eq(0.3333));   // just testing
}

TEST(rgbToHsv) {
    ColorRgb crgb0 = { { 0, 0, 0 } };
    ColorHsv chsv0 = { { 0, 1, 1 } };

    for(size_t num = 1; num <= OneVal.getRaw(); num += 0x20) {
        crgb0->r = num / (float) OneVal.getRaw();
        chsv0 = crgb0.toHsv();
        ASSERT_TRUE(chsv0->h.eq(0.0f, 0.1f)); // All red
        ASSERT_EQ(chsv0->s.getRaw(), OneVal.getRaw());
        ASSERT_EQ(chsv0->v.getRaw(), num);
    }

    Color::Hsv hsv;

    crgb0 = { { 1, 0, 0 } };
    hsv = crgb0.toHsv();
    // cout << hsv.h.getFloat() << endl;
    ASSERT_EQ(hsv.h.getFloat(), 0.0f);

    crgb0 = { { 1, 1, 0 } };
    hsv = crgb0.toHsv();
    // cout << hsv.h.getFloat() << endl;
    ASSERT_TRUE(hsv.h.eq(1.0f / 6.0f));

    crgb0 = { { 0, 1, 0 } };
    hsv = crgb0.toHsv();
    // cout << hsv.h.getFloat() << endl;
    ASSERT_TRUE(hsv.h.eq(2.0f / 6.0f));

    crgb0 = { { 0, 1, 1 } };
    hsv = crgb0.toHsv();
    // cout << hsv.h.getFloat() << endl;
    ASSERT_TRUE(hsv.h.eq(3.0f / 6.0f));

    crgb0 = { { 0, 0, 1 } };
    hsv = crgb0.toHsv();
    // cout << hsv.h.getFloat() << endl;
    ASSERT_TRUE(hsv.h.eq(4.0f / 6.0f));
    
    crgb0 = { { 1, 0, 1 } };
    hsv = crgb0.toHsv();
    // cout << hsv.h.getFloat() << endl;
    ASSERT_TRUE(hsv.h.eq(5.0f / 6.0f));
}

TEST(hsvToRgbHue) {
    ColorHsv chsv;
    Color::Rgb rgb;

    chsv = { { Color::Component(0), 1, 1 } };
    rgb = chsv.toRgb();
    // cout << hex << rgb.r.getRaw() << std::endl;
    ASSERT_TRUE(rgb.r.eq(1.0f));

    chsv = { { Color::Component(1.0f / 6.0f), 1, 1 } };
    rgb = chsv.toRgb();
    // cout << hex << rgb.r.getRaw() << std::endl;
    ASSERT_TRUE(rgb.r.eq(1.0f));

    chsv = { { Color::Component(2.0f / 6.0f), 1, 1 } };
    rgb = chsv.toRgb();
    // cout << hex << rgb.g.getRaw() << std::endl;
    ASSERT_TRUE(rgb.g.eq(1.0f));

    chsv = { { Color::Component(3.0f / 6.0f), 1, 1 } };
    rgb = chsv.toRgb();
    // cout << hex << rgb.g.getRaw() << std::endl;
    ASSERT_TRUE(rgb.g.eq(1.0f));

    chsv = { { Color::Component(4.0f / 6.0f), 1, 1 } };
    rgb = chsv.toRgb();
    // cout << hex << rgb.b.getRaw() << std::endl;
    ASSERT_TRUE(rgb.b.eq(1.0f));

    chsv = { { Color::Component(5.0f / 6.0f), 1, 1 } };
    rgb = chsv.toRgb();
    // cout << hex << rgb.b.getRaw() << std::endl;
    ASSERT_TRUE(rgb.b.eq(1.0f));
}

TEST(hsvToRgbSatVal) {
    ColorHsv chsv;
    Color::Rgb rgb;

    chsv = { { 0, 0, 1 } };
    rgb = chsv.toRgb();
    // cout << hex << rgb.r.getRaw() << std::endl;
    ASSERT_TRUE(rgb.r.eq(1.0f));
    ASSERT_TRUE(rgb.g.eq(1.0f));
    ASSERT_TRUE(rgb.b.eq(1.0f));

    chsv = { { 0, Color::Component(0.5f), 1 } };
    rgb = chsv.toRgb();
    // cout << hex << rgb.r.getRaw() << std::endl;
    ASSERT_TRUE(rgb.r.eq(1.0f));
    ASSERT_TRUE(rgb.g.eq(0.5f));
    ASSERT_TRUE(rgb.b.eq(0.5f));

    chsv = { { 0, 1, 0 } };
    rgb = chsv.toRgb();
    // cout << hex << rgb.r.getRaw() << std::endl;
    ASSERT_TRUE(rgb.r.eq(0.0f));
    ASSERT_TRUE(rgb.g.eq(0.0f));
    ASSERT_TRUE(rgb.b.eq(0.0f));

    chsv = { { 0, 1, Color::Component(0.5f) } };
    rgb = chsv.toRgb();
    // cout << hex << rgb.r.getRaw() << std::endl;
    ASSERT_TRUE(rgb.r.eq(0.5f));
    ASSERT_TRUE(rgb.g.eq(0.0f));
    ASSERT_TRUE(rgb.b.eq(0.0f));
}

TEST(clamp) {
    static const Color::Component zp25(0.25f);
    static const Color::Component zp50(0.5f);
    static const Color::Component zp75(0.75f);

    ColorRgb crgb = { { 0, 1, zp50 } };
    ColorHsv chsv = { { 0, 0, 1 } };

    crgb.clamp({ zp25, zp25, zp25 }, { zp75, zp75, zp75 });
    ASSERT_EQ(crgb.toRgb().r.getRaw(), zp25.getRaw());
    ASSERT_EQ(crgb.toRgb().g.getRaw(), zp75.getRaw());
    ASSERT_EQ(crgb.toRgb().b.getRaw(), zp50.getRaw());
    
    chsv.clamp({ 0, zp25, zp25 }, { 1, zp75, zp75 });
    ASSERT_EQ(chsv.toHsv().h.getRaw(), Color::Component(0).getRaw());
    ASSERT_EQ(chsv.toHsv().s.getRaw(), zp25.getRaw());
    ASSERT_EQ(chsv.toHsv().v.getRaw(), zp75.getRaw());
}


TEST_MAIN();

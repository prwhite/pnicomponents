////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////

// Using some info from: https://cpldcpu.com/2014/08/27/apa102/

#include "pniapa102.h"

namespace pni {

////////////////////////////////////////////////////////////////////


void Apa102::init(InitArgs const& args) {
    mInitArgs = args;
    initHook();
}

void Apa102::deinit() {
    deinitHook();
}

////////////////////////////////////////////////////////////////////

void Apa102Software::writeColor(Color const& color) {
    writeBeg();
    writeSingleColor(color);
    writeEnd();
}
    
void Apa102Software::writeColors(Colors const& colors) {
    writeBeg();
    for (auto cur : colors.mColor) {
        writeSingleColor(cur);
    }   
    writeEnd();
}

void Apa102Software::initHook() {
	uint64_t bitmask = 0;
	bitmask = bitmask | (1<<mInitArgs.mClockPin);
	bitmask = bitmask | (1<<mInitArgs.mDataPin);

	gpio_config_t gpioConfig;
	gpioConfig.pin_bit_mask = bitmask;
	gpioConfig.mode         = GPIO_MODE_OUTPUT;
	gpioConfig.pull_up_en   = GPIO_PULLUP_DISABLE;
	gpioConfig.pull_down_en = GPIO_PULLDOWN_ENABLE;
	gpioConfig.intr_type    = GPIO_INTR_DISABLE;
	gpio_config(&gpioConfig);

	gpio_set_level(mInitArgs.mClockPin, 0x00);
	gpio_set_level(mInitArgs.mDataPin, 0x00);
}

void Apa102Software::deinitHook() {
    // n/a for s/w SPI
}

void Apa102Software::writeByte(uint8_t val) {
    for (uint8_t num = 0; num < 8; ++num) {
        gpio_set_level(mInitArgs.mDataPin, (val >> (7 - num)) & 0x01);
        gpio_set_level(mInitArgs.mClockPin, 0x01);
        gpio_set_level(mInitArgs.mClockPin, 0x00);
    }
}

void Apa102Software::writeBeg() {
    // bit bang out the begining
    writeByte(0x00);
    writeByte(0x00);
    writeByte(0x00);
    writeByte(0x00);
}

void Apa102Software::writeEnd() {
    // bit bang out the ending
    writeByte(0x01);
    writeByte(0x01);
    writeByte(0x01);
    writeByte(0x01);
}

void Apa102Software::writeSingleColor(Color const& color) {
    writeByte(0xe0 | color.v);  // top 3 bits on, bottom 5 bytes for brightness
    writeByte(color.b);
    writeByte(color.g);
    writeByte(color.r);
}

////////////////////////////////////////////////////////////////////

} // end namespace pni

////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////

#ifndef pniapa102_h
#define pniapa102_h

#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include <vector>

////////////////////////////////////////////////////////////////////

namespace pni {

////////////////////////////////////////////////////////////////////

class Apa102 {
    public:
        struct InitArgs {
            gpio_num_t mDataPin;
            gpio_num_t mClockPin;

        };

        using Component = uint8_t;
        
        struct Color {
            Component r;
            Component g;
            Component b;
            Component v;    // apa102 brightness is 'v'alue
        };

            // This representation should work with h/w API w DMA
        using ColorVec = std::vector< Color >;
        
        struct Colors {
            ColorVec mColor;
        };

        void init(InitArgs const& args);
        void deinit();

        virtual void writeColor(Color const& color) = 0;
        virtual void writeColors(Colors const& colors) = 0;

    protected:
        InitArgs mInitArgs;

        virtual void initHook() = 0;
        virtual void deinitHook() = 0;
};

////////////////////////////////////////////////////////////////////

class Apa102Software : public Apa102 {
    public:

        virtual void writeColor(Color const& color);
        virtual void writeColors(Colors const& colors);

    protected:
        virtual void initHook();
        virtual void deinitHook();

        void writeByte(uint8_t val);
        void writeBeg();
        void writeEnd();
        void writeSingleColor(Color const& color);
};

// TODO class Apa102Hardware to use esp32 h/w SPI

////////////////////////////////////////////////////////////////////

} // end namespace pni

#endif // pniapa102_h

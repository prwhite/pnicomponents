////////////////////////////////////////////////////////////////////
//
//
//
////////////////////////////////////////////////////////////////////

#ifndef pnimicsph0645_h
#define pnimicsph0645_h

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/portmacro.h"
#include "driver/i2s.h"
#include "esp_log.h"

#include <cstdint>
#include <cstddef>
#include <vector>

////////////////////////////////////////////////////////////////////

namespace pni {

////////////////////////////////////////////////////////////////////

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-fpermissive"

    // With some help from here: https://www.esp32.com/viewtopic.php?t=1756
class MicSph0645 {
        constexpr static const char* TAG = "MicSph0645";

    public:

        struct Config {
            // TODO: Sample Rate, Channels, Etc.
            size_t mI2sNum;         // 0 or 1 typically
            size_t mSampleRate;         // 44100, etc.
            size_t mChannels;           // TODO: For i2s_channel_t or i2s_channel_fmt_t?
            size_t mBitsPerSample;      // 8, 16, 24, 32
            size_t mBufCount;
            size_t mBufLen;

            int mBckPin;
            int mWsPin;
            int mDataOut;
            int mDataIn;
        };

        MicSph0645() {

        }


        bool init(Config const& config) {
            ESP_LOGI(TAG, "starting to configure i2s input for mic");

            mConfig = config;
            
            static const int i2s_num = config.mI2sNum; // i2s port number
            
            static const i2s_config_t i2s_config = {
                 .mode = (i2s_mode_t) (I2S_MODE_MASTER | I2S_MODE_RX),
                 .sample_rate = (int)config.mSampleRate,
                 .bits_per_sample = (i2s_bits_per_sample_t)config.mBitsPerSample, // I2S_BITS_PER_SAMPLE_16BIT,
                 .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
                 .communication_format = (i2s_comm_format_t) (I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
                 .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // high interrupt priority
                 .dma_buf_count = (int)config.mBufCount,
                 .dma_buf_len = (int)config.mBufLen
            };
            
            static const i2s_pin_config_t pin_config = {
                .bck_io_num = config.mBckPin, // 26,
                .ws_io_num = config.mWsPin, //  25,
                .data_out_num = config.mDataOut, // 22,
                .data_in_num = config.mDataIn // I2S_PIN_NO_CHANGE
            };
            
            if(i2s_driver_install(getPort(), &i2s_config, 0, 0) != ESP_OK) {   //install and start i2s driver
                ESP_LOGE(TAG, "failed to install driver with given config");
                return false;
            }

            if(i2s_set_pin(getPort(), &pin_config) != ESP_OK) {
                ESP_LOGE(TAG, "failed to set pin config");
                return false;
            }
        
            if(i2s_set_clk(getPort(), mConfig.mSampleRate, (i2s_bits_per_sample_t)config.mBitsPerSample, I2S_CHANNEL_MONO) != ESP_OK) {
                ESP_LOGE(TAG, "failed to set clk values");
                return false;
            }

            ESP_LOGI(TAG, "successfully configured i2s input for mic");
            return true;
        }

        void uninit() {
            i2s_driver_uninstall(getPort()); //stop & destroy i2s driver
        }

        Config const& getConfig() const {
            return mConfig;
        }

        i2s_port_t getPort() const {
            return (i2s_port_t)mConfig.mI2sNum;
        }

        size_t getSampleByteSize() const {
            return mConfig.mChannels * mConfig.mBitsPerSample / 8;
        }

        size_t getDmaByteSize() const {
            return mConfig.mBufCount * mConfig.mBufLen;
        }

        size_t getSamplesPerDmaByteSize() const {
            return getDmaByteSize() / getSampleByteSize();
        }

        using Bytes = std::vector< char >;
        int readBytes(Bytes& bytes) {
            bytes.resize(getDmaByteSize());
            auto ret = i2s_read_bytes(getPort(), &bytes[ 0 ], bytes.size(), (portTickType)portMAX_DELAY);
            bytes.resize(ret);

            return ret;
        }

    private:
        Config mConfig;
};

#pragma GCC diagnostic pop

////////////////////////////////////////////////////////////////////

} // end namespace pni

#endif // pnimicsph0645_h

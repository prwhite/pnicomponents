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
            size_t mBufLen;             // if mono, number of samples; if stereo, number of sample pairs

            int mBckPin;
            int mWsPin;
            int mDataOut;
            int mDataIn;
        };

        MicSph0645() {

        }


            // Example runtime output from esp32 I2S library.
            // Note that specified dma_buf_len was 64, but resulted in 128.
            // Assuming that it's 64 samples = 128 bytes.  
            // That's unexpected from the sparse documentation.
            // For 16, 64, short, 1 chan
            //  I (1785) I2S: DMA Malloc info, datalen=blocksize=128, dma_buf_count=16
            //  I (1792) I2S: Req RATE: 22050, real rate: 22321.000, BITS: 16, CLKM: 28, BCK: 8, MCLK: 5644983.462, SCLK: 714272.000000, diva: 64, divb: 22
            //  I (1805) I2S: Req RATE: 22050, real rate: 22321.000, BITS: 16, CLKM: 28, BCK: 8, MCLK: 5644983.462, SCLK: 714272.000000, diva: 64, divb: 22
            // For 8, 64, int, 2 chan:
            //  I (693) I2S: DMA Malloc info, datalen=blocksize=512, dma_buf_count=8
            // So it looks like dma size is dma_buf_count * dma_buf_len * Bps * channels.
        bool init(Config const& config) {
            ESP_LOGI(TAG, "starting to configure i2s input for mic");

            mConfig = config;
            
            static const i2s_config_t i2s_config = {
                 .mode = (i2s_mode_t) (I2S_MODE_MASTER | I2S_MODE_RX),
                 .sample_rate = (int)config.mSampleRate,
                 .bits_per_sample = (i2s_bits_per_sample_t)config.mBitsPerSample, // I2S_BITS_PER_SAMPLE_16BIT,
                 .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
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
        
            // if(i2s_set_clk(getPort(), mConfig.mSampleRate, (i2s_bits_per_sample_t)config.mBitsPerSample, I2S_CHANNEL_MONO) != ESP_OK) {
            //     ESP_LOGE(TAG, "failed to set clk values");
            //     return false;
            // }

            ESP_LOGI(TAG, "successfully configured i2s input for mic");
            return true;
        }

        void uninit() {
            i2s_driver_uninstall(getPort()); //stop & destroy i2s driver
        }

        Config const& getConfig() const {
            return mConfig;
        }

    private:
            // Helper methods
        i2s_port_t getPort() const {
            return (i2s_port_t)mConfig.mI2sNum;
        }

        size_t getSampleByteSize() const {
            return mConfig.mBitsPerSample / 8;
        }

        size_t getDmaByteSize() const {
            return mConfig.mBufCount * mConfig.mBufLen * getSampleByteSize() * mConfig.mChannels;
        }

        size_t getSamplesPerDmaByteSize() const {
            return getDmaByteSize() / getSampleByteSize();
        }

    public:
            // When doing stereo, samples will be [rlrlrl...]
            // but, with only one mic, the lefts are all zero.
            // So, this packs the samples [rrr] and compacts the
            // storage to half the size.
        template< class VectorType >        // must be a vector
        void reorderSamples(VectorType& vec) {
            size_t end = vec.size() / 2;  // Only half the data is good
            for(size_t num = 0; num < end; ++num) {
                vec[ num ] = vec[ num * 2];
            }
            vec.resize(end);
        }

            // TODO: Must document this complicated method!
        template< class VectorType >    // must be a vector
        int readSamples(VectorType& vec) {
            const size_t ValSize = getSampleByteSize();
            const size_t SamplesInBuffer = getSamplesPerDmaByteSize();
            const size_t DmaBufferSize = getDmaByteSize();

            static_assert(sizeof(typename VectorType::value_type)>0, "VectoryType doesn't seem to be a vector");

            vec.resize(SamplesInBuffer);

            auto ret = i2s_read_bytes(getPort(), (char*) &vec[ 0 ], DmaBufferSize, (portTickType)portMAX_DELAY);

            vec.resize(ret / ValSize);

            for(auto& val : vec) {
                val <<= 5;              // TODO: Only works for 32 bps
            }

            return ret;
        }

    protected:
        Config mConfig;
};

////////////////////////////////////////////////////////////////////

} // end namespace pni

#endif // pnimicsph0645_h

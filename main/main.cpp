#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include <string>
#include <cstring>

extern "C" esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}

wifi_config_t createWifiStaConfig(std::string const& ssid, std::string const& password) {
    wifi_config_t sta_config;
    strncpy((char*)(sta_config.sta.ssid), ssid.c_str(), ssid.size());
    strncpy((char*)(sta_config.sta.password), password.c_str(), password.size());
    sta_config.sta.bssid_set = false;
    return sta_config;
}

void init_wifi()
{
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    wifi_config_t sta_config = createWifiStaConfig("prehiti-wireless", "gwokmle!");

    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_ERROR_CHECK( esp_wifi_connect() );
}

extern "C" void app_main(void)
{
    nvs_flash_init();
    tcpip_adapter_init();
    init_wifi();

    gpio_set_direction(GPIO_NUM_5, GPIO_MODE_OUTPUT);
    int level = 0;
    while (true) {
        gpio_set_level(GPIO_NUM_5, level);
        level = !level;
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}

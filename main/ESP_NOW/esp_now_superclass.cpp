//
// Created by maxwc on 16/04/2025.
//

#include "esp_now_superclass.h"

#include <cstring>
#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_now.h>
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include <esp_mac.h>



wifi_init_config_t Esp_now_superclass::cfg;
TelemetryPacket Esp_now_superclass::telemetry_packet;


Esp_now_superclass::Esp_now_superclass() {
    cfg = WIFI_INIT_CONFIG_DEFAULT();
    telemetry_packet.counter = 0;
    telemetry_packet = {};
    Esp_now_superclass::init_wifi();
}


esp_err_t Esp_now_superclass::readMacAddress(uint8_t baseMac[6]){
    esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
    if (ret == ESP_OK) {
        ESP_LOGI("MAC ADDR", "%02x:%02x:%02x:%02x:%02x:%02x",
                      baseMac[0], baseMac[1], baseMac[2],
                      baseMac[3], baseMac[4], baseMac[5]);
        return ESP_OK;
    } else {
        ESP_LOGI("MAC ADDR", "Failed to read MAC address");
        return ESP_FAIL;
    }
}

esp_err_t Esp_now_superclass::init_wifi(){
    if(nvs_flash_init() != ESP_OK) return ESP_FAIL;
    if(esp_netif_init() != ESP_OK) return ESP_FAIL;
    if(esp_event_loop_create_default() != ESP_OK) return ESP_FAIL;

    // WIFI
    //wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT(); -> init in constructor
    if(esp_wifi_init(&cfg) != ESP_OK) return ESP_FAIL;
    if(esp_wifi_set_mode(WIFI_MODE_STA) != ESP_OK) return ESP_FAIL;
    if(esp_wifi_start() != ESP_OK) return ESP_FAIL;
    if(esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE) != ESP_OK) return ESP_FAIL;

    esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_LR); //added for LR
    if(esp_now_init() != ESP_OK) return ESP_FAIL;
    return ESP_OK;
}

void Esp_now_superclass::print_telemetry(const TelemetryPacket& packet) {
    ESP_LOGI("TELEMETRY", "-----------------------------");
    ESP_LOGI("TELEMETRY", "ID: %d", packet.id);
    ESP_LOGI("TELEMETRY", "Counter: %d", packet.counter);
    ESP_LOGI("TELEMETRY", "Pitch: %.2f", packet.pitch);
    ESP_LOGI("TELEMETRY", "Yaw: %.2f", packet.yaw);
    ESP_LOGI("TELEMETRY", "Roll: %.2f", packet.roll);
    ESP_LOGI("TELEMETRY", "AX: %.2f", packet.ax);
    ESP_LOGI("TELEMETRY", "AY: %.2f", packet.ay);
    ESP_LOGI("TELEMETRY", "AZ: %.2f", packet.az);
    ESP_LOGI("TELEMETRY", "Temperature: %.2f °C", packet.temperature);
    ESP_LOGI("TELEMETRY", "Pressure: %.2f Pa", packet.pressure);
    ESP_LOGI("TELEMETRY", "Altitude: %.2f m", packet.altitude);
    ESP_LOGI("TELEMETRY", "-----------------------------");
}







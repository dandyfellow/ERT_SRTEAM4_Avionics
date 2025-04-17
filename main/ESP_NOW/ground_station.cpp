//
// Created by maxwc on 16/04/2025.
//

#include "ground_station.h"
#include "esp_now_configuration.h"

#include <cstring>
#include <esp_log.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"

//SLAVE CODE
//include libraries in esp_now_configuration

static const char* TAG = "ESP-NOW-SLAVE";


Ground_station::Ground_station() {
    if(init_esp_now() != ESP_OK) ESP_LOGI(TAG, "Esp_now_init failed!");
}

// Init ESP-NOW
esp_err_t Ground_station::init_esp_now() {
    if(esp_now_init() != ESP_OK) return ESP_FAIL;
    // Register receive callback
    if (esp_now_register_recv_cb(on_receive_cb) != ESP_OK) return ESP_FAIL;
    ESP_LOGI(TAG, "ESP-NOW Slave ready and listening...");
    return ESP_OK;
}




void Ground_station::on_receive_cb(const esp_now_recv_info_t* info, const uint8_t* data, int len) {
    if (info == nullptr || data == nullptr || len != sizeof(TelemetryPacket)) {
        ESP_LOGW("ESP-NOW-RECV", "Invalid packet size or null input");
        return;
    }

    const uint8_t* mac_addr = info->src_addr;
    char mac_str[18];
    snprintf(mac_str, sizeof(mac_str), "%02X:%02X:%02X:%02X:%02X:%02X",
             mac_addr[0], mac_addr[1], mac_addr[2],
             mac_addr[3], mac_addr[4], mac_addr[5]);

    memcpy(&telemetry_packet, data, sizeof(TelemetryPacket));

    ESP_LOGI("ESP-NOW-RECV", "Received Telemetry from %s", mac_str);
    //print_telemetry(telemetry_packet);
}





/*
 Callback to handle incoming ESP-NOW messages
void on_receive_cb(const esp_now_recv_info_t* info, const uint8_t* data, int len) {
    if (info == nullptr || data == nullptr || len <= 0) return;

    const uint8_t* mac_addr = info->src_addr;

    char mac_str[18];
    snprintf(mac_str, sizeof(mac_str), "%02X:%02X:%02X:%02X:%02X:%02X",
             mac_addr[0], mac_addr[1], mac_addr[2],
             mac_addr[3], mac_addr[4], mac_addr[5]);

    char msg[256];
    int copy_len = len < sizeof(msg) - 1 ? len : sizeof(msg) - 1;
    memcpy(msg, data, copy_len);
    msg[copy_len] = '\0';

    ESP_LOGI("ESP-NOW-RECV", "From %s: %s", mac_str, msg);
}


extern "C" void app_main() {

    // Init ESP-NOW
    ESP_ERROR_CHECK(esp_now_init());
    // Register receive callback
    ESP_ERROR_CHECK(esp_now_register_recv_cb(on_receive_cb));

    ESP_LOGI(TAG, "ESP-NOW Slave ready and listening...");
}
*/
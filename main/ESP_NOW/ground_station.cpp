//
// Created by maxwc on 16/04/2025.
//

#include "ground_station.h"

#include <cstring>
#include <esp_log.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"

//SLAVE CODE / GROUND STATION
//include libraries in esp_now_configuration

static const char* TAG = "GROUND STATION";

int Ground_station::packet_number = 1;

Ground_station::Ground_station() {
    ground_to_avionics_data.start = false;
    init_peer_info(ESP_SLAVE_GROUND_STATION);
    init_esp_now_callback();
}


void Ground_station::on_receive_cb(const esp_now_recv_info_t* info, const uint8_t* data, int len) {
    if (info == nullptr || data == nullptr || len != sizeof(TelemetryPacket)) {
        ESP_LOGW("ESP-NOW-RECV", "Invalid packet size or null input");
        return;
    }
    memcpy(&telemetry_packet, data, sizeof(TelemetryPacket));
    //ESP_LOGI(TAG, "Received telemetry_packet");
    //print_telemetry(telemetry_packet);
}

void Ground_station::my_data_populate(bool start) {
    ground_to_avionics_data.start = start;
    ground_to_avionics_data.packet_num = packet_number++;
}

esp_err_t Ground_station::send_packet() {
    //printf("Packet ground_to_avionics_data #%d \n",  ground_to_avionics_data.counter++);
    return esp_now_send(mac_addrMASTER, reinterpret_cast<uint8_t *>(&ground_to_avionics_data), sizeof(ground_to_avionics_data));
}

esp_err_t Ground_station::init_esp_now_callback() {
    if (esp_now_register_recv_cb(on_receive_cb) != ESP_OK) {
        ESP_LOGI(TAG, "Esp_now_init failed!");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "ESP-NOW ready and listening...");
    return ESP_OK;
}

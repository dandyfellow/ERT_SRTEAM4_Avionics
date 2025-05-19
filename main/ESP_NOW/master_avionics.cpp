//
// Created by maxwc on 16/04/2025.
//

#include "master_avionics.h"
#include "esp_now_superclass.h"

// MASTER CODE / AVIONICS


static const char* TAG = "AVIONICS";


unsigned int Master_avionics::packet_number;

Master_avionics::Master_avionics() {
    packet_number = 1;
    init_peer_info(ESP_SLAVE_GROUND_STATION);
    init_esp_now_callback();
}

esp_err_t Master_avionics::send_packet() {
    //printf("Packet #%d \n",  telemetry_packet.counter++);
    esp_err_t result = esp_now_send(mac_addrMASTER, reinterpret_cast<uint8_t *>(&telemetry_packet), sizeof(telemetry_packet));
    if (result != ESP_OK) {
        ESP_LOGE("AVIONICS", "Send failed: %d", result);
    } else {
        ESP_LOGI("AVIONICS", "Packet sent successfully");
    }
    return result;
}

void Master_avionics::my_data_populate(const float& pitch, const float& yaw, const float& roll,
    const float& ax, const float& ay, const float& az,
    const float& temperature, const float& pressure, const float& altitude,
    const float& max_altitude, const bool& max_altitude_reached,
    const bool& deploy_main_para_parachute, const float& starting_altitude,
    const float& time) {

    //ESP_LOGI(TAG, "Populating TelemetryPacket");
    Master_avionics::packet_number++;

    telemetry_packet.id = MSG_TELEMETRY;
    telemetry_packet.packet_num = packet_number;
    telemetry_packet.pitch = pitch;
    telemetry_packet.yaw = yaw;
    telemetry_packet.roll = roll;
    telemetry_packet.ax = ax;
    telemetry_packet.ay = ay;
    telemetry_packet.az = az;
    telemetry_packet.temperature = temperature;
    telemetry_packet.pressure = pressure;
    telemetry_packet.altitude = altitude;
    telemetry_packet.max_altitude = max_altitude;
    telemetry_packet.max_altitude_reached = max_altitude_reached;
    telemetry_packet.deploy_main_para_parachute = deploy_main_para_parachute;
    telemetry_packet.starting_altitude = starting_altitude;
    telemetry_packet.time = time;
}

void Master_avionics::on_receive_cb(const esp_now_recv_info_t* info, const uint8_t* data, int len) {
    if (info == nullptr || data == nullptr || len != sizeof(Ground_to_avionics_data)) {
        ESP_LOGW("ESP-NOW-RECV", "Invalid packet size or null input");
        return;
    }
    memcpy(&ground_to_avionics_data, data, sizeof(Ground_to_avionics_data));
    ESP_LOGI(TAG, "Received ground_to_avionics_data");
    //print_telemetry(ground_to_avionics_data);

}


esp_err_t Master_avionics::init_esp_now_callback() {
    if (esp_now_register_recv_cb(on_receive_cb) != ESP_OK) {
        ESP_LOGI(TAG, "Esp_now_init failed!");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "ESP-NOW ready and listening...");
    return ESP_OK;
}

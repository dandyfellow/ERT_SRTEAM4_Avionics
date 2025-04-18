//
// Created by maxwc on 16/04/2025.
//

#include "master_avionics.h"
#include "esp_now_configuration.h"
#include "esp_now_superclass.h"

// MASTER CODE


static const char* TAG = "ESP-NOW-MASTER";

esp_now_peer_info_t Master_avionics::peerInfo;
unsigned int Master_avionics::packet_number;

Master_avionics::Master_avionics() {
    init_peer_info();
    packet_number = 1;
}

esp_err_t Master_avionics::init_peer_info() {
    memcpy(peerInfo.peer_addr, mac_addrSLAVE, ESP_NOW_ETH_ALEN);
    peerInfo.channel = WIFI_CHANNEL;
    peerInfo.ifidx = WIFI_IF_STA;
    peerInfo.encrypt = false;
    if(esp_now_add_peer(&peerInfo) != ESP_OK) {return ESP_FAIL; ESP_LOGE(TAG, "Peer init failed");}
    ESP_LOGI(TAG, "Peer init success");
    esp_now_register_send_cb(Master_avionics::on_send_cb);
    return ESP_OK;
}

esp_err_t Master_avionics::send_packet() {
    //printf("Packet #%d \n",  telemetry_packet.counter++);
    return esp_now_send(mac_addrSLAVE, (uint8_t*)&telemetry_packet, sizeof(telemetry_packet));
}




void Master_avionics::on_send_cb(const uint8_t* mac_addr, esp_now_send_status_t status) {
    //ESP_LOGI("SEND", "Send to " MACSTR " %s", MAC2STR(mac_addr), status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}


void Master_avionics::my_data_populate(const float& pitch, const float& yaw, const float& roll,
    const float& ax, const float& ay, const float& az,
    const float& temperature, const float& pressure, const float& altitude,
    const float& max_altitude, const bool& max_altitude_reached,
    const bool& deploy_main_para_parachute) {

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
}

void Master_avionics::display_data_for_python() {
    printf("*123*--For_Python--*456* "
           "ID:%d,PACKET:%u,PITCH:%.2f,YAW:%.2f,ROLL:%.2f,"
           "AX:%.2f,AY:%.2f,AZ:%.2f,"
           "TEMP:%.2f,PRESS:%.2f,ALT:%.2f,"
           "MAX_ALT:%.2f,MAX_ALT_REACHED:%d,DEPLOYED:%d\n",
           telemetry_packet.id, telemetry_packet.packet_num,
           telemetry_packet.pitch, telemetry_packet.yaw, telemetry_packet.roll,
           telemetry_packet.ax, telemetry_packet.ay, telemetry_packet.az,
           telemetry_packet.temperature, telemetry_packet.pressure, telemetry_packet.altitude,
           telemetry_packet.max_altitude,
           static_cast<int>(telemetry_packet.max_altitude_reached),
           static_cast<int>(telemetry_packet.deploy_main_para_parachute));
}


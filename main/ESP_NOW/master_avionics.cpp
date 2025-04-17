//
// Created by maxwc on 16/04/2025.
//

#include "master_avionics.h"
#include "esp_now_configuration.h"
#include "esp_now_superclass.h"

// MASTER CODE


static const char* TAG = "ESP-NOW-MASTER";
/*
void on_send_cb(const uint8_t* mac_addr, esp_now_send_status_t status) {
    ESP_LOGI("SEND", "Send to " MACSTR " %s", MAC2STR(mac_addr),
             status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}
*/
/*
extern "C" void app_main() {

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // WIFI
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE));



    //esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_LR); added for LR
    ESP_ERROR_CHECK(esp_now_init());


    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, mac_addrSLAVE, ESP_NOW_ETH_ALEN);
    peerInfo.channel = WIFI_CHANNEL;
    peerInfo.ifidx = WIFI_IF_STA;
    peerInfo.encrypt = false;
    ESP_ERROR_CHECK(esp_now_add_peer(&peerInfo));



    while (true) {
        char msg[32];
        snprintf(msg, sizeof(msg), "Msg #%d", message_counter++);
        esp_now_send(mac_addrSLAVE, (uint8_t*)msg, strlen(msg));
        esp_now_register_send_cb(on_send_cb);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }

}
*/

esp_now_peer_info_t Master_avionics::peerInfo;

Master_avionics::Master_avionics() {
    init_peer_info();
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
    printf("Packet #%d",  telemetry_packet.counter++);
    return esp_now_send(mac_addrSLAVE, (uint8_t*)&telemetry_packet, sizeof(telemetry_packet));
}




void Master_avionics::on_send_cb(const uint8_t* mac_addr, esp_now_send_status_t status) {
    ESP_LOGI("SEND", "Send to " MACSTR " %s", MAC2STR(mac_addr),
             status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}


void Master_avionics::my_data_populate(float pitch, float yaw, float roll,
                                       float ax, float ay, float az,
                                       float temperature, float pressure, float altitude) {
    //ESP_LOGI(TAG, "Populating TelemetryPacket");
    telemetry_packet.id = MSG_TELEMETRY;
    telemetry_packet.pitch = pitch;
    telemetry_packet.yaw = yaw;
    telemetry_packet.roll = roll;
    telemetry_packet.ax = ax;
    telemetry_packet.ay = ay;
    telemetry_packet.az = az;
    telemetry_packet.temperature = temperature;
    telemetry_packet.pressure = pressure;
    telemetry_packet.altitude = altitude;
}

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

static const char* TAG = "ESP-NOW-Superclass";


wifi_init_config_t Esp_now_superclass::cfg;
TelemetryPacket Esp_now_superclass::telemetry_packet;
esp_now_peer_info_t Esp_now_superclass::peerInfo;
Ground_to_avionics_data Esp_now_superclass::ground_to_avionics_data = {};


Esp_now_superclass::Esp_now_superclass() {
    cfg = WIFI_INIT_CONFIG_DEFAULT();
    telemetry_packet = {};
    telemetry_packet.packet_num = 1;
    if(init_wifi() != ESP_OK) ESP_LOGE(TAG, "ESP-NOW Init Failed");
    else ESP_LOGI(TAG, "ESP-NOW Init sucess");
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
    ESP_LOGI("TELEMETRY", "Counter: %d", packet.packet_num);
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

esp_err_t Esp_now_superclass::init_peer_info(ESP_NOW_DEVICE device) {
    if(device == ESP_MASTER_AVIONICS) memcpy(peerInfo.peer_addr, mac_addrSLAVE, ESP_NOW_ETH_ALEN);
    else if(device == ESP_SLAVE_GROUND_STATION) memcpy(peerInfo.peer_addr, mac_addrMASTER, ESP_NOW_ETH_ALEN);
    //device A talks to B, so it A inits to B and B inits to A
    peerInfo.channel = WIFI_CHANNEL;
    peerInfo.ifidx = WIFI_IF_STA;
    peerInfo.encrypt = false;
    if(esp_now_add_peer(&peerInfo) != ESP_OK) {
        ESP_LOGE(TAG, "Peer init failed");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Peer init success");
    //esp_now_register_send_cb(on_send_cb);
    return ESP_OK;
}

void Esp_now_superclass::on_send_cb(const uint8_t* mac_addr, esp_now_send_status_t status) {
    ESP_LOGI(TAG, "Send to " MACSTR " %s", MAC2STR(mac_addr), status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}


void Esp_now_superclass::display_data_for_python() {
    printf("*123*--For_Python--*456* "
           "ID:%d,PACKET:%u,PITCH:%.2f,YAW:%.2f,ROLL:%.2f,"
           "AX:%.2f,AY:%.2f,AZ:%.2f,"
           "TEMP:%.2f,PRESS:%.2f,ALT:%.2f,"
           "MAX_ALT:%.2f,MAX_ALT_REACHED:%d,DEPLOYED:%d,START_ALT:%.2f\n",
           telemetry_packet.id, telemetry_packet.packet_num,
           telemetry_packet.pitch, telemetry_packet.yaw, telemetry_packet.roll,
           telemetry_packet.ax, telemetry_packet.ay, telemetry_packet.az,
           telemetry_packet.temperature, telemetry_packet.pressure, telemetry_packet.altitude,
           telemetry_packet.max_altitude,
           static_cast<int>(telemetry_packet.max_altitude_reached),
           static_cast<int>(telemetry_packet.deploy_main_para_parachute),
           telemetry_packet.starting_altitude);
}


// Init ESP-NOW


//
// Created by maxwc on 16/04/2025.
//

#ifndef ESP_NOW_H
#define ESP_NOW_H

#include <cstring>
#include <esp_log.h>
#include "freertos/FreeRTOS.h"
//#include "freertos/task.h"
#include "esp_now.h"
#include "esp_wifi.h"
//#include "esp_event.h"
//#include "nvs_flash.h"
//#include <esp_mac.h>



//small breadboard esp32 (ground station) f8:b3:b7:2f:56:c4
constexpr uint8_t mac_addrMASTER[6] = {0xF8, 0xB3, 0xB7, 0x2F, 0x56, 0xC4};

//large breadboard esp32 (telemetry / avionics)  ec:e3:34:ce:34:90
constexpr uint8_t mac_addrSLAVE[6] = {0xEC, 0xE3, 0x34, 0xCE, 0x34, 0x90};



#define WIFI_CHANNEL 6

enum MessageType {
    MSG_TELEMETRY = 1,
    MSG_COMMAND   = 2, //not used
    MSG_ACK       = 3  //not used
};

enum ESP_NOW_DEVICE {
    ESP_MASTER_AVIONICS = 0,
    ESP_SLAVE_GROUND_STATION = 1
};

typedef struct __attribute__((packed)) { // __attribute__((packed)) no padding between fields
    uint8_t id; //1 byte
    unsigned int packet_num;  //4 byte
    float pitch;  //4 byte
    float yaw;  //4 byte
    float roll;  //4 byte
    float ax;  //4 byte
    float ay;  //4 byte
    float az;  //4 byte
    float temperature;  //4 byte
    float pressure;  //4 byte
    float altitude;  //4 byte
    float max_altitude; //4 byte
    bool max_altitude_reached; //1 byte
    bool deploy_main_para_parachute; //1 byte
    float starting_altitude; //4bytes
    float time; //4bytes
} TelemetryPacket; //total bytes = 57 bytes


typedef struct __attribute__((packed)) {
    bool start;
    int packet_num;
} Ground_to_avionics_data; //total size: 5byte


class Esp_now_superclass {
public:
    Esp_now_superclass();

    static esp_err_t readMacAddress(uint8_t baseMac[6]);
    static void print_telemetry(const TelemetryPacket& packet); //just for testing

    static unsigned int get_packet_number() {return  telemetry_packet.packet_num;}
    static TelemetryPacket get_telemetry() {return telemetry_packet;}
    static Ground_to_avionics_data get_ground_data() {return ground_to_avionics_data;}

    static void display_data_for_python();

protected:
    static TelemetryPacket telemetry_packet;
    static wifi_init_config_t cfg;
    static esp_now_peer_info_t peerInfo;
    static Ground_to_avionics_data ground_to_avionics_data;

    static esp_err_t init_peer_info(ESP_NOW_DEVICE device);
    static esp_err_t init_wifi();

    static void on_send_cb(const uint8_t* mac_addr, esp_now_send_status_t status);

};



#endif //ESP_NOW_H

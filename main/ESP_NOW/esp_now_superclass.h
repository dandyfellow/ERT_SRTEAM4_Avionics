//
// Created by maxwc on 16/04/2025.
//

#ifndef ESP_NOW_H
#define ESP_NOW_H

#include "esp_now_configuration.h"

#include <cstring>
#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include <esp_mac.h>


//small breadboard esp32 (ground station? for now) 14:2b:2f:ec:04:5c
constexpr uint8_t mac_addrSLAVE[6] = {0x14, 0x2B, 0x2F, 0xEC, 0x04, 0x5C};
//large breadboard esp32 (telemetry / avionics)  f8:b3:b7:2f:56:c4
constexpr uint8_t mac_addrMASTER[6] = {0xF8, 0xB3, 0xB7, 0x2F, 0x56, 0xC4};

#define WIFI_CHANNEL 6

enum MessageType {
    MSG_TELEMETRY = 1,
    MSG_COMMAND   = 2, //not used
    MSG_ACK       = 3  //not used
};


typedef struct __attribute__((packed)) { // __attribute__((packed)) no padding between fields
    uint8_t id; //1 byte
    int8_t counter;  //1 byte
    float pitch;  //4 byte
    float yaw;  //4 byte
    float roll;  //4 byte
    float ax;  //4 byte
    float ay;  //4 byte
    float az;  //4 byte
    float temperature;  //4 byte
    float pressure;  //4 byte
    float altitude;  //4 byte

} TelemetryPacket; //total bytes = 40 bytes

class Esp_now_superclass {
public:
    Esp_now_superclass();
    static esp_err_t readMacAddress(uint8_t baseMac[6]);
    static esp_err_t init_wifi();
    static void print_telemetry(const TelemetryPacket& packet);
    static unsigned int get_message_counter() {return  telemetry_packet.counter;}

protected:
    static wifi_init_config_t cfg;
    static TelemetryPacket telemetry_packet;
};



#endif //ESP_NOW_H

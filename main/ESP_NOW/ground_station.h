//
// Created by maxwc on 16/04/2025.
//

#ifndef ESP_NOW_SLAVE_GROUND_STATION_H
#define ESP_NOW_SLAVE_GROUND_STATION_H

#include "esp_now_superclass.h"


class Ground_station : public Esp_now_superclass {
public:
    Ground_station(); //calls init_esp_now
    static TelemetryPacket get_telemetry() {return telemetry_packet;}

private:
    static void on_receive_cb(const esp_now_recv_info_t* info, const uint8_t* data, int len);
    static esp_err_t init_esp_now();
};




#endif //ESP_NOW_SLAVE_GROUND_STATION_H

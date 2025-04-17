//
// Created by maxwc on 16/04/2025.
//

#ifndef MASTER_AVIONICS_H
#define MASTER_AVIONICS_H

#include "esp_now_superclass.h"


class Master_avionics : public Esp_now_superclass {
public:
    Master_avionics();

    static esp_err_t send_packet();
    static void my_data_populate(float pitch, float yaw, float roll,
                          float ax, float ay, float az,
                          float temperature, float pressure, float altitude);

private:
    static esp_now_peer_info_t peerInfo;

    static esp_err_t init_peer_info();
    static void on_send_cb(const uint8_t* mac_addr, esp_now_send_status_t status);
};



#endif //MASTER_AVIONICS_H

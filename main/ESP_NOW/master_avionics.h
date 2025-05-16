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
    static void my_data_populate(const float& pitch, const float& yaw, const float& roll,
        const float& ax, const float& ay, const float& az,
        const float& temperature, const float& pressure, const float& altitude,
        const float& max_altitude, const bool& max_altitude_reached,
        const bool& deploy_main_para_parachute, const float& starting_altitude);

    static void set_packet_number(const unsigned int n) {packet_number = n;}

private:
    static unsigned int packet_number;

    static esp_err_t init_esp_now_callback();
    static void on_receive_cb(const esp_now_recv_info_t* info, const uint8_t* data, int len);

};



#endif //MASTER_AVIONICS_H

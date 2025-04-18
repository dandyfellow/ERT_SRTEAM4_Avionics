//
// Created by maxwc on 07/04/2025.
//

#ifndef TOOLS_H
#define TOOLS_H
#include <cstdio>
#include "esp_now_superclass.h"

struct IMUData10Axis {
    float ax = 0, ay = 0, az = 0; // Accelerometer data [g]
    float gx = 0, gy = 0, gz = 0; // [deg/s]
    float mx = 0, my = 0, mz = 0; // Magnetometer data
    float pressure = 0; // [Pa]
    float temperature = 0; // [C]
    float altitude = 0; // [meters]
};



class Tools {
public:
    static void Blinking(void* param);
    static void displayIMUData(const IMUData10Axis& data);

    static void deploy_main_parachute(void* param);

private:

};



#endif //TOOLS_H

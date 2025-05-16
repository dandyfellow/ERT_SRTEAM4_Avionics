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
    float time = 0;
    float pitch = 0, yaw = 0, roll = 0;
    bool max_altitude_reached = false;
    bool deploy_main_para_parachute = false;
};

class Tools {
public:
    static void Blinking(void* param);
    static void BlinkOnce(void* param);
    static void displayIMUData(const IMUData10Axis& data); //purely for testing reason -> to display in the serial monitor
    static void switchAxis(float& x, float& y, float& z);
    static void delay(double ms);
private:
};






#endif //TOOLS_H

//
// Created by maxwc on 07/04/2025.
//

#ifndef TOOLS_H
#define TOOLS_H
#include <cstdio>

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
    static void display_data_for_python(
        const double& pitch, const double& yaw, const double& roll,
        const double& ax,const double& ay, const double& az,
        const double& temp, const double& pressure, const double& height);


private:

};



#endif //TOOLS_H

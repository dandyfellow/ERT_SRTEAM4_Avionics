//
// Created by maxwc on 07/04/2025.
//

#ifndef TOOLS_H
#define TOOLS_H
#include <cstdio>

struct IMUData10Axis {
    float ax, ay, az; // Accelerometer data
    float gx, gy, gz; // Gyroscope data
    float mx, my, mz; // Magnetometer data
    float pressure; // Pressure data
    float temperature; // Temperature data
};

void displayIMUData(const IMUData10Axis& data) {
    printf("Accelerometer: ax=%.2f, ay=%.2f, az=%.2f\n", data.ax, data.ay, data.az);
    printf("Gyroscope: gx=%.2f, gy=%.2f, gz=%.2f\n", data.gx, data.gy, data.gz);
    printf("Magnetometer: mx=%.2f, my=%.2f, mz=%.2f\n", data.mx, data.my, data.mz);
    printf("Pressure: %.2f hPa\n", data.pressure);
    printf("Temperature: %.2f °C\n", data.temperature);
}

class Tools {
public:
    static void Blinking(void* param);


private:

};



#endif //TOOLS_H

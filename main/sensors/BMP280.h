//
// Created by maxwc on 02/04/2025.
//

#ifndef BMP280_H
#define BMP280_H

extern "C" {
    #include "../../components/bmp280/bmp280.h"
}


#include <Sensor.h>


class BMP280 : public Sensor {
public:
    BMP280();
    ~BMP280() override;

    bool init() override;
    void read() override;
    void display() override;

    static void testing();

    float getTemperature() const;
    float getPressure() const;

private:
    bmp280_t dev;
    float temperature;
    float pressure;
};





#endif //BMP280_H

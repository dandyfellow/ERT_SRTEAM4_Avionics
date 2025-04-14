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
    //For all the overriden functions, check the SuperClass
    //Sensor. -> go to Sensor.h

    BMP280();
    ~BMP280() override;
    bool init() override;
    bool read() override;
    void display() override;

    static void testing();

    float get_temperature() const {return temperature;}
    float get_pressure() const {return pressure;}
    float get_altitude() const {return altitude;}

private:
    bmp280_t dev; // the bmp280 parameters (I think, not sure :/ )
    float temperature; //BMP280 has two readings, pressure and temp, BME280 has humidity on top of that
    float pressure;
    double altitude;
};





#endif //BMP280_H

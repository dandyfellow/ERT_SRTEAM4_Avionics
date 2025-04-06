//
// Created by maxwc on 02/04/2025.
//

#ifndef SENSOR_H
#define SENSOR_H



class Sensor {
public:
    virtual bool init() = 0;
    virtual void read() = 0;
    virtual ~Sensor() = default;
    virtual void display() = 0;
    static void testing();
};

#endif //SENSOR_H

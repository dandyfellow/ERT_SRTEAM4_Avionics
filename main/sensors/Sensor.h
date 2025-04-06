//
// Created by maxwc on 02/04/2025.
//

#ifndef SENSOR_H
#define SENSOR_H



class Sensor {
public:
       // To be finished
    virtual ~Sensor() = default;

    // virtual bool init() return true if the initiation has passed,
    // returns false if connection error
    virtual bool init() = 0;

    // To be finished
    virtual void display() = 0;

    /* Originally, I wanted virtual bool read() to return true
         * if the reading has passed, returns false if reading
         * has failed. However to create Tasks, you need to pass
         * a pointer to a void -> void* param. So there goes the logic :/
         */
    virtual bool read() = 0;

    // just tests the skeleton of the project (outputs "testing
    // bla bla bla" in the serial monitor)
    static void testing();
};

#endif //SENSOR_H

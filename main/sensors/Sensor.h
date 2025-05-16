//
// Created by maxwc on 02/04/2025.
//

#ifndef SENSOR_H
#define SENSOR_H
#include <soc/gpio_num.h>


class Sensor {
public:

    virtual ~Sensor() = default; //I only use object, so no need for a complex destructor for pointers

    // virtual bool init() return true if the initiation has passed,
    // returns false if connection error
    virtual bool init() = 0;

    // To be finished // purely for testing to the serial monitor
    virtual void display() = 0;

    virtual bool read() = 0;

    // averages the first 100 readings and stores it as bias
    virtual void calibrate() = 0;

    // just tests the skeleton of the project (outputs "testing
    // bla bla bla" in the serial monitor)
    static void testing();


protected:
    static constexpr gpio_num_t I2C_MASTER_SDA = GPIO_NUM_21;
    static constexpr gpio_num_t I2C_MASTER_SCL = GPIO_NUM_22;
};

#endif //SENSOR_H

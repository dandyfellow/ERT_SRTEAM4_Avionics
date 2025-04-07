//
// Created by maxwc on 02/04/2025.
//

#ifndef MPU6050_H
#define MPU6050_H

#include "Sensor.h"

extern "C" {
    #include "../../components/mpu6050/mpu6050.h"
    //#include "mpu6050.h" //doesn't fucking work fuck you cmakefile
}



class MPU6050 : public Sensor {
public:
    MPU6050();
    ~MPU6050() override;
    bool init() override; //tries to init at most 10 times
    bool read() override;
    void display() override;

    static void testing();
private:
    mpu6050_dev_t dev;
    mpu6050_acceleration_t accel;
    mpu6050_rotation_t rotation;
};



#endif //MPU6050_H

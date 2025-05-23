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
    void calibrate() override; //substacts the mean of the first 100 readings as bias

    mpu6050_acceleration_t get_accel() const {return accel;}
    mpu6050_rotation_t get_rotation() const {return rotation;}

    float get_accel_x() const {return accel.x;}
    float get_accel_y() const {return accel.y;}
    float get_accel_z() const {return accel.z;}

    float get_gyro_x() const {return rotation.x;}
    float get_gyro_y() const {return rotation.y;}
    float get_gyro_z() const {return rotation.z;}

    float get_bias_accel_x() const {return bias_accel.x;}
    float get_bias_accel_y() const {return bias_accel.y;}
    float get_bias_accel_z() const {return bias_accel.z;}

    float get_bias_rotation_x() const {return bias_rotation.x;}
    float get_bias_rotation_y() const {return bias_rotation.y;}
    float get_bias_rotation_z() const {return bias_rotation.z;}


    static void testing();
private:
    mpu6050_dev_t dev;
    mpu6050_acceleration_t accel;
    mpu6050_rotation_t rotation;

    mpu6050_acceleration_t bias_accel;
    mpu6050_rotation_t bias_rotation;
};



#endif //MPU6050_H

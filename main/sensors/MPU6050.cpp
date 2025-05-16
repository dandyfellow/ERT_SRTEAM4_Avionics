//
// Created by maxwc on 02/04/2025.
//


#include "MPU6050.h"
#include <cstdio>
#include <Tools.h>

extern "C" {
    #include <esp_log.h>
}

#define ADDR MPU6050_I2C_ADDRESS_LOW //0x68


static const char *TAG = "MPU6500)";


void MPU6050::testing() {
    ESP_LOGI("MPU6050", "Testing MPU6050");
}

MPU6050::MPU6050() {
    dev = {};
    accel.x = 0; accel.y = 0; accel.z = 0;
    rotation.x = 0; rotation.y = 0; rotation.z = 0;
    bias_accel.x = 0; bias_accel.y = 0; bias_accel.z = 0;
    bias_rotation.x = 0; bias_rotation.y = 0; bias_rotation.z = 0;
}
MPU6050::~MPU6050() {
    // destructor doesn't need to do much
    esp_err_t err = mpu6050_free_desc(&dev);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to free descriptor: %d", err);
    }
}

bool MPU6050::init() {
    ESP_ERROR_CHECK(mpu6050_init_desc(&dev, ADDR, static_cast<i2c_port_t>(0), I2C_MASTER_SDA, I2C_MASTER_SCL));

    unsigned int init_count(0);
    while (init_count < 2)
    {
        esp_err_t res = i2c_dev_probe(&dev.i2c_dev, I2C_DEV_WRITE);
        if (res == ESP_OK)
        {
            ESP_LOGI(TAG, "Found MPU60x0 device");
            break;
        }
        ESP_LOGE(TAG, "MPU60x0 not found");
        vTaskDelay(pdMS_TO_TICKS(500));
        init_count++;
    }

    ESP_ERROR_CHECK(mpu6050_init(&dev));
    //mpu6050_set_full_scale_gyro_range(&dev, MPU6050_GYRO_RANGE_250); //!< ± 250 °/s
    //mpu6050_set_full_scale_accel_range(&dev, MPU6050_ACCEL_RANGE_2); //!< ± 2g

    mpu6050_set_full_scale_gyro_range(&dev, MPU6050_GYRO_RANGE_2000); //!< ± 2000 °/s
    mpu6050_set_full_scale_accel_range(&dev, MPU6050_ACCEL_RANGE_16); //!< ± 16g
    /*
    MPU6050_GYRO_RANGE_250 = 0,  //!< ± 250 °/s
MPU6050_GYRO_RANGE_500,      //!< ± 500 °/s
    MPU6050_GYRO_RANGE_1000,     //!< ± 1000 °/s
    MPU6050_GYRO_RANGE_2000,     //!< ± 2000 °/s

    MPU6050_ACCEL_RANGE_2 = 0, //!< ± 2g
    MPU6050_ACCEL_RANGE_4,     //!< ± 4g
    MPU6050_ACCEL_RANGE_8,     //!< ± 8g
    MPU6050_ACCEL_RANGE_16,    //!< ± 16g
    */
    ESP_LOGI(TAG, "Initialisation succes");
    ESP_LOGI(TAG, "Accel range: %d", dev.ranges.accel);
    ESP_LOGI(TAG, "Gyro range:  %d", dev.ranges.gyro);
    return true;
}
bool MPU6050::read() {
    if(mpu6050_get_motion(&dev, &accel, &rotation) == ESP_OK) {
        //adjust for bias
        accel.x -= bias_accel.x; accel.y -= bias_accel.y; accel.z -= bias_accel.z;
        rotation.x -= bias_rotation.x; rotation.y -= bias_rotation.y; rotation.z -= bias_rotation.z;


        return true;
    }
    //accel.x = 0; accel.y = 0; accel.z = 0;
    //rotation.x = 0; rotation.y = 0; rotation.z = 0;
    ESP_LOGE(TAG, "Could not read data");
    return false;
}

void MPU6050::calibrate() {
    ESP_LOGI(TAG, "Calibrating MPU6500...Reading 100 samples");
    unsigned int samples = 100;
    mpu6050_acceleration_t sum_accel = {0, 0, 0};
    mpu6050_rotation_t sum_rotation = {0, 0, 0};
    for (unsigned int i = 0; i < samples; ++i) {
        if (mpu6050_get_motion(&dev, &accel, &rotation) == ESP_OK) {
            sum_accel.x += accel.x;
            sum_accel.y += accel.y;
            sum_accel.z += accel.z;

            sum_rotation.x += rotation.x;
            sum_rotation.y += rotation.y;
            sum_rotation.z += rotation.z;
        } else {
            ESP_LOGE(TAG, "Could not read data for calibration");
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    bias_accel.x = sum_accel.x / samples  - 1; // THE Z AXIS IS NOW THE X AXIS (FOR ROCKET MOUNT)
    bias_accel.y = sum_accel.y / samples;
    bias_accel.z = sum_accel.z / samples;// -1 to account for gravity (1g)

    bias_rotation.x = sum_rotation.x / samples;
    bias_rotation.y = sum_rotation.y / samples;
    bias_rotation.z = sum_rotation.z / samples;

    ESP_LOGI(TAG, "MPU6050 calibration done: bias_accel: x=%.4f y=%.4f z=%.4f | bias_rotation: x=%.4f y=%.4f z=%.4f",
             bias_accel.x, bias_accel.y, bias_accel.z,
             bias_rotation.x, bias_rotation.y, bias_rotation.z);
}

void MPU6050::display() {
    ESP_LOGI(TAG, "Acceleration: x=%.4f   y=%.4f   z=%.4f", accel.x, accel.y, accel.z);
    ESP_LOGI(TAG, "Gyroscope:    x=%.4f   y=%.4f   z=%.4f", rotation.x, rotation.y, rotation.z);
}
//
// Created by maxwc on 02/04/2025.
//


#include "MPU6050.h"
#include <cstdio>

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
    while (init_count < 10)
    {
        esp_err_t res = i2c_dev_probe(&dev.i2c_dev, I2C_DEV_WRITE);
        if (res == ESP_OK)
        {
            ESP_LOGI(TAG, "Found MPU60x0 device");
            break;
        }
        ESP_LOGE(TAG, "MPU60x0 not found");
        vTaskDelay(pdMS_TO_TICKS(1000));
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
    ESP_LOGI(TAG, "Accel range: %d", dev.ranges.accel);
    ESP_LOGI(TAG, "Gyro range:  %d", dev.ranges.gyro);

    return true;
}
bool MPU6050::read() {

    ESP_ERROR_CHECK(mpu6050_get_motion(&dev, &accel, &rotation));
    return true;

}
void MPU6050::display() {
    ESP_LOGI(TAG, "Acceleration: x=%.4f   y=%.4f   z=%.4f", accel.x, accel.y, accel.z);
    ESP_LOGI(TAG, "Rotation:     x=%.4f   y=%.4f   z=%.4f", rotation.x, rotation.y, rotation.z);
}
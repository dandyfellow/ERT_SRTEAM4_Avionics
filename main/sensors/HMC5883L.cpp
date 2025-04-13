//
// Created by maxwc on 07/04/2025.
//

#include "HMC5883L.h"

#include <cstring>

extern "C" {
#include <esp_log.h>
}

static const char *TAG = "QMC5883L";



HMC5883L::HMC5883L() {
    memset(&dev, 0, sizeof(qmc5883l_t));
    data.x = 0, data.y = 0, data.z = 0;
}
HMC5883L::~HMC5883L() {
    esp_err_t err = qmc5883l_free_desc(&dev);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to free descriptor: %d", err);
    }
}

bool HMC5883L::init() {
    ESP_ERROR_CHECK(qmc5883l_init_desc(&dev, QMC5883L_I2C_ADDR_DEF, static_cast<i2c_port_t>(0), I2C_MASTER_SDA, I2C_MASTER_SCL));

    // 50Hz data rate, 128 samples, -2G..+2G range
    ESP_ERROR_CHECK(qmc5883l_set_config(&dev, QMC5883L_DR_50, QMC5883L_OSR_128, QMC5883L_RNG_2));
    return true;
}


bool HMC5883L::read() {
    if (qmc5883l_get_data(&dev, &data) == ESP_OK){return true;}
    ESP_LOGE(TAG, "Could not read QMC5883L data");
    return false;
}
void HMC5883L::display() {
    ESP_LOGI(TAG, "X:%.2f mG, Y:%.2f mG, Z:%.2f mG", data.x, data.y, data.z);
}


void HMC5883L::testing() {
    ESP_LOGI("MPU6050", "Testing MPU6050");
}
// qmc5883l_espidf_arduino.cpp
// Arduino-style QMC5883L support for ESP-IDF

#include "QMC5883LCompass.h"
#include "driver/i2c.h"
#include "esp_log.h"

#define QMC5883L_I2C_ADDRESS 0x0D

#define REG_CTRL1  0x09
#define REG_CTRL2  0x0A
#define REG_SET_RESET 0x0B
#define REG_DATA 0x00
#define REG_STATUS 0x06

static const char *TAG = "QMC5883L";

bool QMC5883LCompass::begin(i2c_port_t port, gpio_num_t sda, gpio_num_t scl, uint32_t freq) {
    _port = port;
    _addr = QMC5883L_I2C_ADDRESS;

    i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = sda;
    conf.scl_io_num = scl;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = freq;
    conf.clk_flags = 0;

    ESP_ERROR_CHECK(i2c_param_config(_port, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(_port, conf.mode, 0, 0, 0));

    vTaskDelay(pdMS_TO_TICKS(100));

    // Unlock / set reset
    uint8_t reset = 0x01;
    ESP_ERROR_CHECK(writeRegister(REG_SET_RESET, &reset, 1));
    vTaskDelay(pdMS_TO_TICKS(10));

    // Config: Continuous mode, 50Hz, 2G range, OSR 512
    uint8_t ctrl1 = 0x1D;
    ESP_ERROR_CHECK(writeRegister(REG_CTRL1, &ctrl1, 1));

    // Optional status/interrupt config
    uint8_t ctrl2 = 0x00;
    writeRegister(REG_CTRL2, &ctrl2, 1);

    return true;
}

bool QMC5883LCompass::readRaw(int16_t &x, int16_t &y, int16_t &z) {
    uint8_t raw[6];
    esp_err_t err = readRegister(REG_DATA, raw, 6);
    if (err != ESP_OK) return false;

    x = (int16_t)((raw[1] << 8) | raw[0]);
    y = (int16_t)((raw[3] << 8) | raw[2]);
    z = (int16_t)((raw[5] << 8) | raw[4]);
    return true;
}

esp_err_t QMC5883LCompass::writeRegister(uint8_t reg, const uint8_t *data, size_t len) {
    uint8_t buffer[2] = {reg, data[0]};
    return i2c_master_write_to_device(_port, _addr, buffer, 2, pdMS_TO_TICKS(100));
}

esp_err_t QMC5883LCompass::readRegister(uint8_t reg, uint8_t *data, size_t len) {
    return i2c_master_write_read_device(_port, _addr, &reg, 1, data, len, pdMS_TO_TICKS(100));
}

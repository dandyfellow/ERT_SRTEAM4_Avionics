//
// Created by maxwc on 02/04/2025.
//

#include "BMP280.h"
#include <esp_log.h>


void BMP280::testing() {
    ESP_LOGI("BMP280", "Testing BMP280");
}


static const char *TAG = "BMP280_Class";

BMP280::BMP280()
{
    dev = {};  // zero-initialize all fields (C++)
    temperature = 0.0;
    pressure = 0.0;
}

BMP280::~BMP280()
{
    // destructor doesn't need to do much
    esp_err_t err = bmp280_free_desc(&dev);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to free descriptor: %d", err);
    }
}

bool BMP280::init()
{

    bmp280_params_t params;
    bmp280_init_default_params(&params);

    esp_err_t err = bmp280_init_desc(&dev, BMP280_I2C_ADDRESS_0, I2C_NUM_0, GPIO_NUM_21, GPIO_NUM_22);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init descriptor: %d", err);
        return false;
    }

    err = bmp280_init(&dev, &params);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to init BMP280 sensor: %d", err);
        return false;
    }

    ESP_LOGI(TAG, "BMP280 initialized successfully.");
    return true;
}

void BMP280::read()
{
    float temp, press;
    if (bmp280_read_float(&dev, &temp, &press, NULL) == ESP_OK) {
        temperature = temp;
        pressure = press;
        ESP_LOGI(TAG, "Temperature: %.2f °C, Pressure: %.2f hPa", temperature, pressure / 100.0f);
    } else {
        ESP_LOGW(TAG, "Failed to read sensor data.");
    }
}

float BMP280::getTemperature() const
{
    return temperature;
}

float BMP280::getPressure() const
{
    return pressure;
}

void BMP280::display()
{
    printf("BMP280 Reading → Temperature: %.2f °C | Pressure: %.2f hPa\n",
           temperature, pressure / 100.0f);
}

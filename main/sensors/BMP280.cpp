//
// Created by maxwc on 02/04/2025.
//

#include "BMP280.h"

#include <cstring>
#include <esp_log.h>
#include <string>

#define CONFIG_EXAMPLE_I2C_MASTER_SDA GPIO_NUM_21
#define CONFIG_EXAMPLE_I2C_MASTER_SCL GPIO_NUM_22

void BMP280::testing() {
    ESP_LOGI("BMP280", "Testing BMP280");
}

static const char *TAG = "BMP280_Class"; //bruh idk what this is chatgpt is a menace

BMP280::BMP280()
{
    dev = {};  // zero-initialize all fields (C++)
    temperature = 0.0;
    pressure = 0.0;
}

BMP280::~BMP280() // chatGPT behaviour
{
    // destructor doesn't need to do much
    esp_err_t err = bmp280_free_desc(&dev);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to free descriptor: %d", err);
    }
}

bool BMP280::init() // a rework of UncleRus' example
{
    /*
     * Small problem with the logic, init will always return true...
     * I can't figure out (didn't give put in the type ( I don't give a rats ass))
     * Oh well, if the init didn't work, you will know about it in the compiler :)
     * But actually, if the init is weird, the "read()" method should
     * catch it.
     */
    bmp280_params_t params;
    bmp280_init_default_params(&params);
    memset(&dev, 0, sizeof(bmp280_t));

    ESP_ERROR_CHECK(bmp280_init_desc(&dev, BMP280_I2C_ADDRESS_0, static_cast<i2c_port_t>(0), CONFIG_EXAMPLE_I2C_MASTER_SDA, CONFIG_EXAMPLE_I2C_MASTER_SCL));
    ESP_ERROR_CHECK(bmp280_init(&dev, &params));

    //technically useless, but i don't wanna touch it
    bool bme280p = dev.id == BME280_CHIP_ID;
    printf("BMP280: found %s\n", bme280p ? "BME280" : "BMP280");
    return true;
}

bool BMP280::read() // a rework of UncleRus' example
{
    vTaskDelay(pdMS_TO_TICKS(500));
    if (bmp280_read_float(&dev, &temperature, &pressure, NULL) != ESP_OK) {
        printf("Temperature/pressure reading failed\n");
        temperature = 0;
        pressure = 0; //since I will display the results regarding of if the readings failed, set to 0 to see the fail.
        return false;
    }
    return true;
}

float BMP280::getTemperature() const {return temperature;}

float BMP280::getPressure() const {return pressure;}

void BMP280::display() {printf("BMP280 Reading → Temperature: %.2f °C | Pressure: %.2f Pa\n",temperature, pressure);}

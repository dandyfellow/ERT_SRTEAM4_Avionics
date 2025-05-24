//
// Created by maxwc on 02/04/2025.
//

#include "BMP280.h"

#include <cmath>
#include <cstring>
#include <esp_log.h>
#include <string>
#include <cmath>
#include <Tools.h>
#include <constantes.h>

#define I2C_MASTER_SDA GPIO_NUM_21
#define I2C_MASTER_SCL GPIO_NUM_22


//===============================================PROTOYPES===================================================================================0

//===============================================END_OF_PROTOYPES===================================================================================0
void BMP280::testing() {
    ESP_LOGI("BMP280", "Testing BMP280");
}

static const char *TAG = "BMP280";

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

    ESP_ERROR_CHECK(bmp280_init_desc(&dev, BMP280_I2C_ADDRESS_1, static_cast<i2c_port_t>(0), I2C_MASTER_SDA, I2C_MASTER_SCL));
    ESP_ERROR_CHECK(bmp280_init(&dev, &params));

    //technically useless, but i don't wanna touch it
    bool bme280p = dev.id == BME280_CHIP_ID;
    ESP_LOGI(TAG,"BMP280: found %s\n", bme280p ? "BME280" : "BMP280");
    ESP_LOGI(TAG, "Initialisation succes");
    return true;
}

bool BMP280::read() // a rework of UncleRus' example
{
    if (bmp280_read_float(&dev, &temperature, &pressure, NULL) != ESP_OK) {
        ESP_LOGE(TAG, "BMP280 reading failed");
        //temperature = 0; pressure = 0; //since I will display the results regarding of if the readings failed, set to 0 to see the fail.
        temperature = previous_temperature; pressure = previous_pressure;
        return false;
    }

    previous_temperature = temperature; previous_pressure = pressure; previous_altitude = altitude;
    // Convert pressure to altitude
    altitude = calculateHeightFromPressure(pressure);
    //to avoid when the baro gives completely wrong values:
    if(altitude < starting_altitude - 500) { //500m below starting altitude
        pressure = previous_pressure;
        temperature = previous_temperature;
        altitude = previous_altitude;
    }

    calculate_max_altitude();
    if(max_altitude_reached == false) calculate_max_altitude_reached();
    if(deploy_main_para_parachute == false) calculate_main_para_deploy();

    if (deploy_main_para_parachute && !deployed) {
        deployed = true;
        xTaskCreate(deploy_parachute_task, "deploy_parachute", 2048, NULL, 5, NULL);
    }
    //ESP_LOGI(TAG, "Altitude: %f", altitude);

    return true;
}

void BMP280::calibrate()
{
    // This function is not really needed for BMP280, as it doesn't have a bias like MPU6050
    // This function will get the first 100 readings and calculate the average.
    // Then set the starting_temperature and starting_altitude to those values.
    ESP_LOGI(TAG, "Calibrating BMP280...Reading 100 samples");
    unsigned int samples = 100;
    float sum_temperature = 0.0f;
    float sum_pressure = 0.0f;
    for (unsigned int i = 0; i < samples; i++) {
        if (bmp280_read_float(&dev, &temperature, &pressure, NULL) != ESP_OK) {
            ESP_LOGE(TAG, "BMP280 reading failed");
        }
        sum_temperature += temperature;
        sum_pressure += pressure;
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    starting_temperature = sum_temperature / samples;
    starting_pressure = sum_pressure / samples;
    starting_altitude = calculateHeightFromPressure(starting_pressure);

    ESP_LOGI(TAG, "BMP280 calibration done: Temperature: %.2f °C, Pressure: %.2f Pa, Altitude: %.2f m", starting_temperature, starting_pressure, starting_altitude);
}


void BMP280::display() {ESP_LOGI(TAG, "Temperature: %.2f °C | Pressure: %.2f Pa | Altitude: %.2f m",temperature, pressure, altitude);}

float BMP280::calculateHeightFromPressure(float p) {
    return 44330.0f * (1.0f - std::pow((p/100) / 1013.25, 0.1903f)); //1013.25 = sea level pressure
}

void BMP280::calculate_max_altitude() {
    if(altitude > max_altitude) {
        max_altitude = altitude;
    }
}

void BMP280::calculate_max_altitude_reached() {
    static unsigned int count = 0;
    static unsigned int baro_c = 0;
    static double altitude_sum = 0;
    static double altitude_sum_previous = starting_altitude;
    baro_c++;
    altitude_sum += altitude;
    if(baro_c >= 4) {
        altitude_sum = altitude_sum/4;
        baro_c = 0;
        double vertical_velocity = (altitude_sum - altitude_sum_previous)/((1./SAMPLE_FREQ) * 4); //to get time in seconds and not ms
        ESP_LOGI(TAG, "Velocity: %f | Altitude: %f  | Altitude_sum: %f",vertical_velocity, altitude,altitude_sum ); //REMOVE LATER
        altitude_sum_previous = altitude_sum;
        altitude_sum = 0;

        if(max_altitude - altitude > MAIN_PARA_DEPLOY_HEIGHT_TOLERANCE and vertical_velocity < MAIN_PARA_DEPLOY_VELOCITY) {
            count++;
        } else {
            count = 0;
        }
        if(count > 5) {
            max_altitude_reached = true;
            ESP_LOGI(TAG, "MAX ALTITUDE REACHED!!!: %.2f m", altitude);
        }
    }
}

void BMP280::calculate_main_para_deploy() {
    static unsigned int count = 0;
    if(max_altitude_reached) {
        if(max_altitude < (MAIN_PARA_DEPLOY_HEIGHT + starting_altitude)) {
            Tools::delay(1*1000); // CHANGE TO VTASKDELAY(oiasdoiash) DELAY IS SHIT
            ESP_LOGI(TAG, "Max altitude reached under MAIN_PARA_DEPLOY_HEIGHT | max_altitude: %.2f m, MAIN_PARA_DEPLOY_HEIGHT: %.2f m", max_altitude, MAIN_PARA_DEPLOY_HEIGHT);
            deploy_main_para_parachute = true;
            ESP_LOGI(TAG, "Main parachute deploy variable set to true: %.2f m", altitude);
        } else {
            if(altitude < MAIN_PARA_DEPLOY_HEIGHT + starting_altitude) { //150m
                count++;
                if(count > 3) {
                    deploy_main_para_parachute = true;
                    ESP_LOGI(TAG, "Normal parachute sequence | max_altitude: %.2f m, MAIN_PARA_DEPLOY_HEIGHT: %.2f m, altitude: %.2f m", max_altitude, MAIN_PARA_DEPLOY_HEIGHT, altitude);
                }
            }
        }
    }
}

void deploy_parachute_task(void* param) {
    gpio_set_level(GPIO_NUM_4, 1);
    vTaskDelay(pdMS_TO_TICKS(5000));
    gpio_set_level(GPIO_NUM_4, 0);
    vTaskDelete(nullptr);  // Task cleans itself up
}


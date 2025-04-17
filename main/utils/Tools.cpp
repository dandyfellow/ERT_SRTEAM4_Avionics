//
// Created by maxwc on 07/04/2025.
//

#include "Tools.h"

extern "C" {
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <driver/i2c.h>
}

void Tools::Blinking(void* param) { //for testing purpouses
#define LED_PIN GPIO_NUM_2

    bool led_on = false;
    unsigned int count = 0;
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    while(true) {
        led_on = !led_on;
        gpio_set_level(LED_PIN, led_on);
        vTaskDelay(pdMS_TO_TICKS(50));
        //ESP_LOGI("Blinking counter", "%d", count);
        count++;
    }
}

//;
void Tools::display_data_for_python(
        const double& pitch, const double& yaw, const double& roll,
        const double& ax,const double& ay, const double& az,
        const double& temp, const double& pressure, const double& height) {
    printf("*123*--For_Python--*456* PITCH:%.2f,YAW:%.2f,ROLL:%.2f,AX:%.2f,AY:%.2f,AZ:%.2f,TEMP:%.2f,PRESS:%.2f,HEIGHT:%.2f\n",
       pitch, yaw, roll,
       ax, ay, az,
       temp, pressure, height);
}

void Tools::displayIMUData(const IMUData10Axis& data) {
    printf("Accelerometer: ax=%.2f, ay=%.2f, az=%.2f\n", data.ax, data.ay, data.az);
    printf("Gyroscope: gx=%.2f, gy=%.2f, gz=%.2f\n", data.gx, data.gy, data.gz);
    printf("Magnetometer: mx=%.2f, my=%.2f, mz=%.2f\n", data.mx, data.my, data.mz);
    printf("Pressure: %.2f hPa\n", data.pressure);
    printf("Temperature: %.2f °C\n", data.temperature);
}


void Tools::deploy_main_parachute(void* param) {
    #define MAIN_PARACHUTE_PIN GPIO_NUM_10 // set here

    gpio_set_direction(MAIN_PARACHUTE_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(MAIN_PARACHUTE_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(5000)); // wait 5 seconds
    gpio_set_level(MAIN_PARACHUTE_PIN, 0);

}
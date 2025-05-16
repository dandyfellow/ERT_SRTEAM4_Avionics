//
// Created by maxwc on 07/04/2025.
//

#include "Tools.h"

#include <esp_timer.h>

#include "esp_now_superclass.h"

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

    while(count < 100) {
        led_on = !led_on;
        gpio_set_level(LED_PIN, led_on);
        vTaskDelay(pdMS_TO_TICKS(500));
        //ESP_LOGI("Blinking counter", "%d", count);
        count++;
    }
}

void Tools::BlinkOnce(void* param) {
#define LED_PIN GPIO_NUM_2
    bool led_on = false;
    unsigned int count = 0;
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    while(count < 50) {
        led_on = !led_on;
        gpio_set_level(LED_PIN, led_on);
        vTaskDelay(pdMS_TO_TICKS(10));
        //ESP_LOGI("Blinking counter", "%d", count);
        count++;
    }
    gpio_set_level(LED_PIN, false);
}

//;



void Tools::displayIMUData(const IMUData10Axis& data) {
    printf("Accelerometer: ax=%.2f, ay=%.2f, az=%.2f\n", data.ax, data.ay, data.az);
    printf("Gyroscope: gx=%.2f, gy=%.2f, gz=%.2f\n", data.gx, data.gy, data.gz);
    printf("Magnetometer: mx=%.2f, my=%.2f, mz=%.2f\n", data.mx, data.my, data.mz);
    printf("Pressure: %.2f hPa\n", data.pressure);
    printf("Temperature: %.2f °C\n", data.temperature);
}


void Tools::switchAxis(float& x, float& y, float& z) {
    const float temp = z;
    z = x;
    x = -y;
    y = -temp;
}


void Tools::delay(double ms) {
    int64_t start_time = esp_timer_get_time(); // microseconds

    while ((esp_timer_get_time() - start_time) < (int64_t)(ms * 1000)) {
        taskYIELD();  // Allows other FreeRTOS tasks to run
    }
}




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

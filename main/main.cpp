#include "Madgwick.h"
#include "BMP280.h"
#include "MPU6050.h"
#include "I2CManager.h"


extern "C" {
    #include "freertos/FreeRTOS.h"
    #include "esp_log.h"
    #include "freertos/task.h"
    #include "driver/gpio.h"
    #include <driver/i2c.h>
    #include <stdio.h> //equivalent of iostream but for c
    #include <string.h>
}

#include <vector>

void Blinking(void* param) { //for testing purpouses

#define LED_PIN GPIO_NUM_2


    bool led_on = false;
    unsigned int count = 0;
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    while(true) {
        led_on = !led_on;
        gpio_set_level(LED_PIN, led_on);
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI("Counter", "%d", count);
        count++;
    }
}


extern "C" void app_main(void)
{
    ESP_LOGI("MAIN", "Hello World");
    Madgwick::testing();
    BMP280::testing();
    MPU6050::testing();
    I2CManager::testing();

    ESP_ERROR_CHECK(i2cdev_init());

    std::vector<Sensor*> sensors;

    sensors.push_back(new BMP280());

    for(Sensor* s : sensors) {
        s->init();
    }

    while(true) {
        for(Sensor* s : sensors){
            s->read();
            s->display();
        }
    }



    //xTaskCreatePinnedToCore(bmp280_test, "bmp280_test", configMINIMAL_STACK_SIZE * 8, NULL, 5, NULL, APP_CPU_NUM);

    //xTaskCreate(Blinking, "Blinking", 4096, NULL, 5, NULL);
    /*
    BMP280 bmp;

    if (!bmp.init()) {
        ESP_LOGI("BMP init","Failed to initialize BMP280!");
        return;
    }
    while (true) {
        bmp.read();
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
    */
}

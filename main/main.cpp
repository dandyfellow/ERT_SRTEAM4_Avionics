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

//====================================BMP=START=============================================

#define CONFIG_EXAMPLE_I2C_MASTER_SDA GPIO_NUM_21
#define CONFIG_EXAMPLE_I2C_MASTER_SCL GPIO_NUM_22

void bmp280_test(void *pvParameters)
{
    bmp280_params_t params;
    bmp280_init_default_params(&params);
    bmp280_t dev;
    memset(&dev, 0, sizeof(bmp280_t));

    ESP_ERROR_CHECK(bmp280_init_desc(&dev, BMP280_I2C_ADDRESS_0, static_cast<i2c_port_t>(0), CONFIG_EXAMPLE_I2C_MASTER_SDA, CONFIG_EXAMPLE_I2C_MASTER_SCL));
    ESP_ERROR_CHECK(bmp280_init(&dev, &params));

    bool bme280p = dev.id == BME280_CHIP_ID;
    printf("BMP280: found %s\n", bme280p ? "BME280" : "BMP280");

    float pressure, temperature, humidity;

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(500));
        if (bmp280_read_float(&dev, &temperature, &pressure, &humidity) != ESP_OK)
        {
            printf("Temperature/pressure reading failed\n");
            continue;
        }

        /* float is used in printf(). you need non-default configuration in
         * sdkconfig for ESP8266, which is enabled by default for this
         * example. see sdkconfig.defaults.esp8266
         */
        printf("Pressure: %.2f Pa, Temperature: %.2f C", pressure, temperature);
        if (bme280p)
            printf(", Humidity: %.2f\n", humidity);
        else
            printf("\n");
    }
}

//====================================BMP=END==============================================

extern "C" void app_main(void)
{
    ESP_LOGI("MAIN", "Hello World");
    Madgwick::testing();
    BMP280::testing();
    MPU6050::testing();
    I2CManager::testing();

    ESP_ERROR_CHECK(i2cdev_init());
    xTaskCreatePinnedToCore(bmp280_test, "bmp280_test", configMINIMAL_STACK_SIZE * 8, NULL, 5, NULL, APP_CPU_NUM);

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

#include "Madgwick.h"
#include "BMP280.h"
#include "MPU6050.h"
#include "HMC5883L.h"
#include "I2CManager.h"
#include "Tools.h"

extern "C" { // C includes
    #include "freertos/FreeRTOS.h"
    #include "esp_log.h"
    #include "freertos/task.h"
    #include "driver/gpio.h"
    #include <driver/i2c.h>
    #include <stdio.h> //equivalent of iostream but for c
    #include <string.h>
}
// C++ includes
#include <vector>



extern "C" void app_main(void)
{

    ESP_LOGI("MAIN", "Hello World");
    //this is just to test the structure of the code / skeleton. See Sensor.h for what testing does
    Madgwick::testing();
    BMP280::testing();
    MPU6050::testing();
    I2CManager::testing();


    ESP_ERROR_CHECK(i2cdev_init());
    /*i2cdev_init() is a wrapper by the UncleRes library with these settings for the i2c protocol
    * #define I2CDEV_DEFAULT_SDA   21 -> added my own defaults as static const constexpr in Sensor
    * #define I2CDEV_DEFAULT_SCL   22
    * #define I2CDEV_DEFAULT_PORT  I2C_NUM_0
    * #define I2CDEV_DEFAULT_FREQ  100000
    */


    std::vector<Sensor*> sensors;

    sensors.push_back(new BMP280());
    sensors.push_back(new MPU6050());
    sensors.push_back(new HMC5883L());

    for(Sensor* s : sensors) {
        s->init();
    }
    //xTaskCreate(Tools::Blinking, "Blinking", configMINIMAL_STACK_SIZE , NULL, 5, NULL);

    while(true) {
        for(Sensor* s : sensors){
            // do not init here!! I2C problems
            s->read();
            s->display();
        }
        printf("==================================================================================|\n");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }





    //xTaskCreate(Blinking, "Blinking", 4096, NULL, 5, NULL);

}

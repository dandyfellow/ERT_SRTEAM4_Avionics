#include "Madgwick.h"
#include "BMP280.h"
#include "MPU6050.h"
#include "I2CManager.h"


extern "C" {
    #include "freertos/FreeRTOS.h"
    #include <esp_log.h>
}

extern "C" void app_main(void)
{
    ESP_LOGI("MAIN", "Hello World");
    Madgwick::testing();
    BMP280::testing();
    MPU6050::testing();
    I2CManager::testing();
}

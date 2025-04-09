//Created by maxwc on 02/04/2025.
#include "BMP280.h"
#include "MPU6050.h"
#include "HMC5883L.h"
#include "I2CManager.h"
#include "Tools.h"
#include "MadgwickAHRS.h"

extern "C" { // C includes
    #include "freertos/FreeRTOS.h"
    #include "esp_log.h"
    #include "freertos/task.h"
    #include "driver/gpio.h"
    #include <driver/i2c.h>
    #include <stdio.h> //equivalent of iostream but for c
    #include <string.h>
    #include "esp_timer.h"
}
// C++ includes
#include <vector>


static const char *TAG = "Main"; // tag for logging
#define SAMPLE_FREQ 5 // Hz | 1Hz = 10ms

extern "C" void app_main(void)
{
    ESP_LOGI("MAIN", "Hello World");
    //this is just to test the structure of the code / skeleton. See Sensor.h for what testing does
    BMP280::testing();MPU6050::testing();I2CManager::testing();

    ESP_ERROR_CHECK(i2cdev_init()); //-> check read me for default values for the i2c bus

    // Initiliazing Sensors
    BMP280 bmp;
    MPU6050 mpu;
    HMC5883L hmc;

    // Initializing Madgwick filter
    MadgwickAHRS madgwick;
    madgwick.begin(SAMPLE_FREQ); // 100Hz | 1Hz = 10ms
    IMUData10Axis data = {0};
    // initialize sensors
    bmp.init();
    mpu.init();
    hmc.init();

    //xTaskCreate(Tools::Blinking, "Blinking", configMINIMAL_STACK_SIZE , NULL, 5, NULL);

    // dynamically measure the sample frequency -> dt
    int64_t last_time = esp_timer_get_time();
    float dt = 0.0f;

    while(true) {
        //timer
        if(SAMPLE_FREQ == 0) {
            int64_t now = esp_timer_get_time();
            dt = (now - last_time) / 1000000.0f; // convert to seconds
            last_time = now;
            madgwick.begin(dt); // update the sample frequency
            ESP_LOGI(TAG, "Sample frequency: %.2f Hz", dt);
        }
        //get data from sensors
        bmp.read();
        mpu.read();
        hmc.read();

        bmp.display();
        mpu.display();
        hmc.display();

        // process data from with Madgwick filter
        data.pressure = bmp.get_pressure();
        data.temperature = bmp.get_temperature();
        data.ax = mpu.get_accel_x();
        data.ay = mpu.get_accel_y();
        data.az = mpu.get_accel_z();
        data.gx = mpu.get_gyro_x();
        data.gy = mpu.get_gyro_y();
        data.gz = mpu.get_gyro_z();
        data.mx = hmc.get_mag_x();
        data.my = hmc.get_mag_y();
        data.mz = hmc.get_mag_z();

        madgwick.update(
            data.gx, data.gy, data.gz,
            data.ax, data.ay, data.az,
            data.mx, data.my, data.mz
        );

        displayIMUData(data); // Display the data
        // Print the results
        ESP_LOGI(TAG, "Orientation: %.2f %.2f %.2f", madgwick.getYaw(), madgwick.getPitch(), madgwick.getRoll());
        ESP_LOGI(TAG, "Orientation (rad): %.2f %.2f %.2f", madgwick.getYawRadians(), madgwick.getPitchRadians(), madgwick.getRollRadians());

        printf("==================================================================================|\n");
        if(SAMPLE_FREQ != 0) vTaskDelay(pdMS_TO_TICKS(SAMPLE_FREQ*10));
        if(SAMPLE_FREQ == 0) vTaskDelay(pdMS_TO_TICKS(1)); // Keep cpu breathing

    }
}

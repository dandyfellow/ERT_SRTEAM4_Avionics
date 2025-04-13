//Created by maxwc on 02/04/2025.
#include "BMP280.h"
#include "MPU6050.h"
#include "HMC5883L.h"
#include "I2CManager.h"
#include "Tools.h"
#include "MadgwickAHRS.h"
#include "../../components/QMC5883LArduino/QMC5883LCompass.h"

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
#define SAMPLE_FREQ 50 // Hz | t = 1/SAMPLE_FREQ
//=================================================================================================================================0

//=================================================================================================================================0

#ifndef APP_CPU_NUM
#define APP_CPU_NUM PRO_CPU_NUM
#endif
#define CONFIG_EXAMPLE_I2C_MASTER_SDA GPIO_NUM_21
#define CONFIG_EXAMPLE_I2C_MASTER_SCL GPIO_NUM_22

void qmc5883l_test(void *pvParameters)
{
    qmc5883l_t dev;

    memset(&dev, 0, sizeof(qmc5883l_t));

    ESP_ERROR_CHECK(i2cdev_init());
    ESP_ERROR_CHECK(qmc5883l_init_desc(&dev, QMC5883L_I2C_ADDR_DEF, static_cast<i2c_port_t>(0), CONFIG_EXAMPLE_I2C_MASTER_SDA, CONFIG_EXAMPLE_I2C_MASTER_SCL));

    // 50Hz data rate, 128 samples, -2G..+2G range
    ESP_ERROR_CHECK(qmc5883l_set_config(&dev, QMC5883L_DR_50, QMC5883L_OSR_128, QMC5883L_RNG_2));

    while (1)
    {
        qmc5883l_data_t data;
        if (qmc5883l_get_data(&dev, &data) == ESP_OK)
            /* float is used in printf(). you need non-default configuration in
             * sdkconfig for ESP8266, which is enabled by default for this
             * example. see sdkconfig.defaults.esp8266
             */
                printf("Magnetic data: X:%.2f mG, Y:%.2f mG, Z:%.2f mG\n", data.x, data.y, data.z);
        else
            printf("Could not read QMC5883L data\n");

        vTaskDelay(pdMS_TO_TICKS(250));
    }
}
//=================================================================================================================================0

//=================================================================================================================================0

extern "C" void app_main(void)
{

    /*
    QMC5883LCompass compass;

    compass.begin(I2C_NUM_0, GPIO_NUM_21, GPIO_NUM_22);

    while (true) {
        int16_t x, y, z;
        if (compass.readRaw(x, y, z)) {
            ESP_LOGI("MAG", "X: %d Y: %d Z: %d", x, y, z);
        }
        vTaskDelay(pdMS_TO_TICKS(250));
    }
*/




    //xTaskCreatePinnedToCore(qmc5883l_test, "qmc5883l_test", configMINIMAL_STACK_SIZE * 4, NULL, 5, NULL, APP_CPU_NUM);


    ESP_LOGI("MAIN", "Hello World");
    //this is just to test the structure of the code / skeleton. See Sensor.h for what testing does
    BMP280::testing();MPU6050::testing();I2CManager::testing();

    ESP_ERROR_CHECK(i2cdev_init()); //-> check read me for default values for the i2c bus



    // Initializing Madgwick filter
    MadgwickAHRS madgwick;
    madgwick.begin(1000./SAMPLE_FREQ); // t = 1/SAMPLE_FREQ
    IMUData10Axis data = {0};
    // Initiliazing Sensors
    BMP280 bmp;
    bmp.init();

    MPU6050 mpu;
    mpu.init();

    HMC5883L hmc;
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
        if(!bmp.read()) bmp.init();
        if(!mpu.read()) mpu.init();
        if(!hmc.read()) hmc.init();
    /*
        bmp.display();
        mpu.display();
        hmc.display();
    */
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

        //displayIMUData(data); // Display the data
        // Print the results
        //ESP_LOGI(TAG, "Orientation: %.2f %.2f %.2f", madgwick.getYaw(), madgwick.getPitch(), madgwick.getRoll());
        ESP_LOGI(TAG, "Orientation (rad): %.2f %.2f %.2f", madgwick.getYawRadians(), madgwick.getPitchRadians(), madgwick.getRollRadians());

        //printf("==================================================================================|\n");
        if(SAMPLE_FREQ != 0) vTaskDelay(pdMS_TO_TICKS(1000./SAMPLE_FREQ));
        if(SAMPLE_FREQ == 0) vTaskDelay(pdMS_TO_TICKS(1)); // Keep cpu breathing

    }


}

//Created by maxwc on 02/04/2025.


#include "BMP280.h"
#include "MPU6050.h"
#include "HMC5883L.h"
#include "I2CManager.h"
#include "Tools.h"
#include "MadgwickAHRS.h"
#include "../../components/QMC5883LArduino/QMC5883LCompass.h"
#include "master_avionics.h"
#include <ground_station.h>

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

//=======================================================================================================================
static const char *TAG = "Main"; // tag for logging
#define SAMPLE_FREQ 20 // Hz | t[ms]= 1000/SAMPLE_FREQ
/*1000Hz = 1ms 750Hz = 1.33ms 500Hz = 2ms 250Hz = 4ms 200Hz = 5ms 150Hz = 6.67ms 100Hz = 10ms
 *50Hz = 20m 25Hz = 40ms 20Hz = 50ms 13.33Hz = 75ms 10Hz = 100ms -> too low 5Hz = 200ms 2Hz = 500ms 1Hz = 1000ms */

//#define MODE_AVIONICS // CHOSE ONE
#define MODE_GROUND_STATION

//=================================================================================================================================0

extern "C" void app_main(void)
{
    ESP_LOGI("MAIN", "Hello World");
    //================================TESTING=================================================================================================0

    //xTaskCreate(Tools::Blinking, "Blinking", configMINIMAL_STACK_SIZE , NULL, 5, NULL);
    //this is just to test the structure of the code / skeleton. See Sensor.h for what testing does
    //BMP280::testing();MPU6050::testing();I2CManager::testing();
    //================================END OF TESTING=================================================================================================0
    ESP_ERROR_CHECK(i2cdev_init()); //-> check read me for default values for the i2c bus

#ifdef MODE_AVIONICS
    // Initializing Madgwick filter
    MadgwickAHRS madgwick;
    madgwick.begin(1000./SAMPLE_FREQ); // t = 1/SAMPLE_FREQ
    IMUData10Axis data = {0};

    // Initiliazing Sensors
    BMP280 bmp; bmp.init();
    MPU6050 mpu; mpu.init();
    HMC5883L hmc; hmc.init();

    //Calibration
    bmp.calibrate(); mpu.calibrate(); hmc.calibrate();

    Master_avionics avionics; //calls the constructor



    // dynamically measure the sample frequency -> dt
    int64_t last_time = esp_timer_get_time();
    float dt = 0.0f;
#endif


#ifdef MODE_GROUND_STATION
    Ground_station ground_station;
#endif



    while(true) {
#ifdef MODE_AVIONICS
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

        //bmp.display();mpu.display();hmc.display();

        // process data from with Madgwick filter
        data.pressure = bmp.get_pressure();
        data.temperature = bmp.get_temperature();
        data.ax = mpu.get_accel_x(); data.ay = mpu.get_accel_y(); data.az = mpu.get_accel_z();
        data.gx = mpu.get_gyro_x(); data.gy = mpu.get_gyro_y(); data.gz = mpu.get_gyro_z();
        data.mx = hmc.get_mag_x(); data.my = hmc.get_mag_y(); data.mz = hmc.get_mag_z();

        madgwick.update(
            data.gx, data.gy, data.gz,
            data.ax, data.ay, data.az,
            data.mx, data.my, data.mz
        );
        Master_avionics::my_data_populate(madgwick.getPitchRadians(), madgwick.getYawRadians(), madgwick.getRollRadians(),
                             data.ax, data.ay, data.az,
                             data.temperature, data.pressure, bmp.get_altitude()
        );
        Master_avionics::send_packet();
        //SESP_LOGI(TAG, "PITCH: %f, YAW: %f, ROLL: %f", madgwick.getPitchRadians(),madgwick.getYawRadians(), madgwick.getRollRadians() );
#endif

#ifdef MODE_GROUND_STATION
        TelemetryPacket telemetry = Ground_station::get_telemetry();
        Tools::display_data_for_python(
            telemetry.pitch, telemetry.yaw, telemetry.roll,
            telemetry.ax, telemetry.ay, telemetry.az,
            telemetry.temperature, telemetry.pressure, telemetry.altitude
        );



#endif

        //printf("==================================================================================|\n");
        if(SAMPLE_FREQ != 0) vTaskDelay(pdMS_TO_TICKS(1000./SAMPLE_FREQ));
        if(SAMPLE_FREQ == 0) vTaskDelay(pdMS_TO_TICKS(1)); // Keep cpu breathing

    }
}

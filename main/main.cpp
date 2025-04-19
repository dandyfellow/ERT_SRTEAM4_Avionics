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
    #include "driver/uart.h"
}

//=======================================================================================================================
static const char *TAG = "Main"; // tag for logging
#define SAMPLE_FREQ 20 // Hz | t[ms]= 1000/SAMPLE_FREQ
/*1000Hz = 1ms 750Hz = 1.33ms 500Hz = 2ms 250Hz = 4ms 200Hz = 5ms 150Hz = 6.67ms 100Hz = 10ms
 *50Hz = 20m 25Hz = 40ms 20Hz = 50ms 13.33Hz = 75ms 10Hz = 100ms -> too low 5Hz = 200ms 2Hz = 500ms 1Hz = 1000ms */

    //#define MODE_AVIONICS // CHOSE ONE
    #define MODE_GROUND_STATION
    //#define MODE_SINGLE_ESP

//=================================================================================================================================0

bool system_armed = false;

void wait_for_uart_command(void* param) {
    char input[64];
    printf("Waiting for UART command...\n");

    while (true) {
        if (fgets(input, sizeof(input), stdin)) {
            printf("Received UART: %s\n", input);

            if (strstr(input, "START")) {
                system_armed = !system_armed;
                printf("System_armed : %s\n", system_armed ? "true" : "false");
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));  // CPU-friendly delay
    }
}


#ifdef MODE_AVIONICS
extern "C" void app_main(void) {
    ESP_ERROR_CHECK(i2cdev_init()); //-> check read me for default values for the i2c bus
#ifdef MODE_SINGLE_ESP
    xTaskCreate(wait_for_uart_command, "wait_for_uart_command", 4096, NULL, 5, NULL);
#endif

    Master_avionics avionics; //calls the constructor
    while(true) {
        system_armed = Master_avionics::get_ground_data().start;
        Master_avionics::set_packet_number(1);
        if(system_armed){
            // Initializing Madgwick filter
            MadgwickAHRS madgwick;
            madgwick.begin(SAMPLE_FREQ); // t = 1/SAMPLE_FREQ
            IMUData10Axis data = {0};

            // Initiliazing Sensors
            BMP280 bmp; bmp.init();
            MPU6050 mpu; mpu.init();
            HMC5883L hmc; hmc.init();

            bmp.calibrate(); mpu.calibrate(); hmc.calibrate();

            // dynamically measure the sample frequency -> dt
            int64_t last_time = esp_timer_get_time();
            float dt = 0.0f;

            while(system_armed) {
                system_armed = Master_avionics::get_ground_data().start;
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
                                                  data.temperature, data.pressure, bmp.get_altitude(), bmp.get_max_altitude(),
                                                  bmp.get_max_altitude_reached(), bmp.get_deploy_main_para_parachute(), bmp.get_starting_altitude()
                );
                Master_avionics::send_packet();
#ifdef MODE_SINGLE_ESP
                Master_avionics::display_data_for_python();
#endif

                if(SAMPLE_FREQ != 0) vTaskDelay(pdMS_TO_TICKS(1000./SAMPLE_FREQ));
                if(SAMPLE_FREQ == 0) vTaskDelay(pdMS_TO_TICKS(1)); // Keep cpu breathing
            }
            system_armed = false;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
#endif




#ifdef MODE_GROUND_STATION
extern "C" void app_main(void) {
    xTaskCreate(wait_for_uart_command, "wait_for_uart_command", 4096, NULL, 5, NULL);
    Ground_station ground_station; //for constructor

    while(true) {
        Esp_now_superclass::display_data_for_python();
        Ground_station::my_data_populate(system_armed);
        if(Ground_station::send_packet() != ESP_OK) ESP_LOGE(TAG, "Failed to send packet from ground station to avionics");



        if(SAMPLE_FREQ != 0) vTaskDelay(pdMS_TO_TICKS(1000./SAMPLE_FREQ));
        if(SAMPLE_FREQ == 0) vTaskDelay(pdMS_TO_TICKS(1)); // Keep cpu breathing


    }
}
#endif

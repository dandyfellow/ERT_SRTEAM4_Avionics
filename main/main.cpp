//Created by maxwc on 02/04/2025.
// This file has been edited by Elie "Fayorg" Baier (elie.baier@epfl.ch)

#include "BMP280.h"
#include "MPU6050.h"
#include "Tools.h"
#include "MadgwickAHRS.h"
#include "../../components/QMC5883LArduino/QMC5883LCompass.h"
#include "master_avionics.h"
#include <ground_station.h>
// #include <constantes.h> TODO: IDK where the fuck is this file??? @dandyfellow ??

extern "C" {
    #include "freertos/FreeRTOS.h"
    #include "esp_log.h"
    #include "freertos/task.h"
    #include "driver/gpio.h"
    #include <stdio.h>
    #include <string.h>
    #include "esp_timer.h"
}

// ======================================================================================================================
    #define MODE_AVIONICS // CHOSE ONE
    // #define MODE_GROUND_STATION
    //#define MODE_SINGLE_ESP
// ======================================================================================================================


static const char *TAG = "Main"; // tag for logging
long long boot_time = esp_timer_get_time();
bool system_armed = false;

void wait_for_uart_command(void* param) {
    char input[64];
    printf("Waiting for UART command...\n");

    while (true) {
        if (fgets(input, sizeof(input), stdin)) {
            printf("Received UART: %s\n", input);

            if (strstr(input, "START")) {
                system_armed = !system_armed;
                printf("System_armed_:_%s\n", system_armed ? "true" : "false"); // DO NOT MODIFY OUTPUT -> used in python
            }
            if (strstr(input, "BLINK")) {
                #define LED_PIN GPIO_NUM_2

                gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
                gpio_set_level(LED_PIN, true);
                vTaskDelay(pdMS_TO_TICKS(100));  // CPU-friendly delay
                gpio_set_level(LED_PIN, false);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));  // CPU-friendly delay
    }
}


#ifdef MODE_AVIONICS
extern "C" void app_main(void) {
    ESP_ERROR_CHECK(i2cdev_init());

    #ifdef MODE_SINGLE_ESP
        xTaskCreate(wait_for_uart_command, "wait_for_uart_command", 4096, NULL, 5, NULL);
    #endif

    system_armed = false;
    Master_avionics avionics;

    while(true) {
        system_armed = Master_avionics::get_ground_data().start; // SINGLE ESP MODE
        Master_avionics::set_packet_number(1);

        if(system_armed){
            // Initializing Madgwick filter
            MadgwickAHRS madgwick;
            madgwick.begin(SAMPLE_FREQ); //
            IMUData10Axis data = {0};

            // Initiliazing Sensors
            BMP280 bmp; bmp.init();
            MPU6050 mpu; mpu.init();
            HMC5883L hmc; hmc.init();

            bmp.calibrate(); mpu.calibrate(); hmc.calibrate();

            long long starting_time = esp_timer_get_time();

            while(system_armed) { // Main loop
                system_armed = Master_avionics::get_ground_data().start; // SINGLE ESP MODE
                //ESP_LOGI(TAG, "Starting the main loop: System_armed: %d", system_armed);
                //get data from sensors
                if(!bmp.read()) {
                    //bmp.init();
                }
                if(!mpu.read()) {
                    //mpu.init();
                }
                if(!hmc.read()) {
                    //hmc.init();
                }
                //bmp.display();mpu.display();hmc.display();

                // process data from with Madgwick filter, all the data is in the data struct
                data.pressure = bmp.get_pressure(); data.temperature = bmp.get_temperature();
                data.ax = mpu.get_accel_x(); data.ay = mpu.get_accel_y(); data.az = mpu.get_accel_z();
                data.gx = mpu.get_gyro_x(); data.gy = mpu.get_gyro_y(); data.gz = mpu.get_gyro_z();
                data.mx = hmc.get_mag_x(); data.my = hmc.get_mag_y(); data.mz = hmc.get_mag_z();

                //switch the data struct axis for the madgwick calculation
                Tools::switchAxis(data.ax, data.ay, data.az); Tools::switchAxis(data.gx, data.gy, data.gz); Tools::switchAxis(data.mx, data.my, data.mz);

                madgwick.update(data.gx, data.gy, data.gz, data.ax, data.ay, data.az, data.mx, data.my, data.mz);

                long long time = esp_timer_get_time() - starting_time;
                data.time = time/1000000.;
                data.pitch = madgwick.getPitchRadians(); data.yaw = madgwick.getYawRadians(); data.roll = madgwick.getRollRadians();
                data.max_altitude_reached = bmp.get_max_altitude_reached();
                data.deploy_main_para_parachute = bmp.get_deploy_main_para_parachute();

                Master_avionics::my_data_populate(data.pitch, data.yaw, data.roll, data.ax, data.ay, data.az, data.temperature, data.pressure, bmp.get_altitude(), bmp.get_max_altitude(), data.max_altitude_reached, data.max_altitude_reached, bmp.get_starting_altitude(), data.time);

                Master_avionics::send_packet();

#ifdef MODE_SINGLE_ESP
                Master_avionics::display_data_for_python();
#endif
                //if (SAMPLE_FREQ != 0) vTaskDelay(pdMS_TO_TICKS(1000 / SAMPLE_FREQ));
                if(SAMPLE_FREQ != 0) Tools::delay(1000/SAMPLE_FREQ);
                vTaskDelay(pdMS_TO_TICKS(1));
                //ESP_LOGI(TAG,"For time purpous");
            }
            system_armed = false;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
#endif

//==========================================================================================================================================================

#ifdef MODE_GROUND_STATION
extern "C" void app_main(void) {
    // Setting up the background tasks
    xTaskCreate(wait_for_uart_command, "wait_for_uart_command", 4096, NULL, 5, NULL);

    Ground_station ground_station; //for constructor

    while(true) {
        printf("TESTING");
        Esp_now_superclass::display_data_for_python();

        Esp_now_superclass::display_data_for_python();
        Ground_station::my_data_populate(system_armed);
        if(Ground_station::send_packet() != ESP_OK) ESP_LOGE(TAG, "Failed to send packet from ground station to avionics");

        ESP_LOGI(TAG, "system_armed: %d", system_armed);

        if(SAMPLE_FREQ != 0) Tools::delay(1000/SAMPLE_FREQ);
        //if (SAMPLE_FREQ != 0) vTaskDelay(pdMS_TO_TICKS(1000 / SAMPLE_FREQ));
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}
#endif
idf_component_register(
        SRCS
            "main.cpp"
            "sensors/Sensor.cpp"
            "sensors/MPU6050.cpp"
            "sensors/BMP280.cpp"
            "filters/MadgwickAHRS.cpp"
            "sensors/HMC5883L.cpp"
            "utils/Tools.cpp"
            "ESP_NOW/master_avionics.cpp"
            "ESP_NOW/ground_station.cpp"
            "ESP_NOW/esp_now_superclass.cpp"

        INCLUDE_DIRS
            "."
            "sensors"
            "filters"
            "utils"
            "ESP_NOW"


        REQUIRES
            driver
            freertos
            esp_idf_lib_helpers
            i2cdev
            bmp280
            mpu6050
            qmc5883l
            esp_timer
            QMC5883LArduino
            esp_wifi esp_event nvs_flash

)

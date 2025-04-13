idf_component_register(
        SRCS
            "main.cpp"
            "sensors/Sensor.cpp"
            "sensors/MPU6050.cpp"
            "sensors/BMP280.cpp"
            "filters/MadgwickAHRS.cpp"
            "utils/I2CManager.cpp"
            "sensors/HMC5883L.cpp"
            "utils/Tools.cpp"

        INCLUDE_DIRS
            "."
            "sensors"
            "filters"
            "utils"


        REQUIRES
            driver
            freertos
            esp_idf_lib_helpers
            i2cdev
            log
            bmp280
            mpu6050
            qmc5883l
            esp_timer
            QMC5883LArduino
)

idf_component_register(
        SRCS
            "main.cpp"
            "sensors/Sensor.cpp"
            "sensors/MPU6050.cpp"
            "sensors/BMP280.cpp"
            "filters/Madgwick.cpp"
            "utils/I2CManager.cpp"

        INCLUDE_DIRS
            "."
            "sensors"
            "filters"
            "utils"
)

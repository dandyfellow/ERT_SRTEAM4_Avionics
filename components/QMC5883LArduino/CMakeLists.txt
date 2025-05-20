# CMakeLists.txt for the QMC5883LArduino component

idf_component_register(
        SRCS "./QMC5883LCompass.cpp"
        INCLUDE_DIRS "."
        REQUIRES i2cdev
)
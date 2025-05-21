idf_component_register(
    SRCS QMC5883LCompass.cpp
    INCLUDE_DIRS .
    REQUIRES i2cdev log esp_idf_lib_helpers
)

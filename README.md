# Avionics for team 4 EPFL Space Race 2024-2025
## Overview
Coded in the ESP IDF framework, using C++. 
The avionics system is designed to be modular, with a focus on sensor integration and data processing. The code is structured to allow for easy addition of new sensors and functionalities.
## Components and libraries
Mostly based of UncleRus's libraries
### MPU6500
Uses the MPU6050 library. Slight change in adress from 6500 to 6050.

### BMP280
Nothing unusual

### QMC5883L 
Class is called HMC5883L because that's what's written on my sensor. The QCM is basically a chinese copy with the same functionalities, but different adresses for ports and such. 
Using the QMC5883L library from UncleRus.

### Default configs by the UncleRes library 
* i2cdev_init() is a wrapper by the UncleRes library with these settings for the i2c protocol
* #define I2CDEV_DEFAULT_SDA   21 -> added my own defaults as static const constexpr in Sensor
* #define I2CDEV_DEFAULT_SCL   22
* #define I2CDEV_DEFAULT_PORT  I2C_NUM_0
* #define I2CDEV_DEFAULT_FREQ  100000
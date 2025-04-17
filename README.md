# Avionics for team 4 EPFL Space Race 2024-2025
## Overview
Coded in the ESP IDF framework, using C++. 
The avionics system is designed to be modular, with a focus on sensor integration and data processing. The code is structured to allow for easy addition of new sensors and functionalities.

### How to use
Clone the project in the ESP-IDF directory.
Have ESP-IDF intalled
For displaying using the python script, you need pyton v3 or more.
I am using a virtual environment for python (venv) -> install the library in the venv!!!

Variables to be defined are the GPIO pins for SDA and SCL, they are in the sensor wrapper. 
The sample frequency is defined in the main.cpp file.

Chose whether you want the ground station or avioncs bay code by choosing the right #define in main.cpp

**Library requirements:**
```bash
pip install pyserial
pip install matplotlib
pip install PyOpenGL PyOpenGL_accelerate pygame
pip install pyopengl pygame


```

## Components and libraries
Mostly based of UncleRus's libraries
Madgwick filter : https://github.com/arduino-libraries/MadgwickAHRS.git
Displaying in OpenGL cube : https://github.com/mattzzw/Arduino-mpu6050.git
### MPU6500
Uses the MPU6050 library. Slight change in adress from 6500 to 6050.

#### BMP280
Nothing unusual

#### QMC5883L 
Class is called HMC5883L because that's what's written on my sensor. The QCM is basically a chinese copy with the same functionalities, but different adresses for ports and such. 
Using the QMC5883L library from UncleRus.

#### Magwick Filter and calibration
The Magwick filter is a sensor fusion algorithm. There is an adjustable #define SAMPLE_FREQ in the main.cpp file

Calibration done on the bmp280 for initial altitude and temperature, biais removal for the accelerometer and gyroscope.

The magnetomter is **NOT** calibrated. The math is pretty complexe. Check https://ww2.mathworks.cn/help/fusion/ug/magnetometer-calibration.html

### Default configs by the UncleRes library 
* i2cdev_init() is a wrapper by the UncleRes library with these settings for the i2c protocol
* #define I2CDEV_DEFAULT_SDA   21 -> added my own defaults as static const constexpr in Sensor
* #define I2CDEV_DEFAULT_SCL   22
* #define I2CDEV_DEFAULT_PORT  I2C_NUM_0
* #define I2CDEV_DEFAULT_FREQ  100000



### Remarks
The QMC5883L on my side doesn't work at first. I need to first run it using the arduino QMC5883LCompass.h Library XYZ Example Sketch. 
Learn more at [https://github.com/mprograms/QMC5883LCompass]    
Then, I can run the code in this repository. Otherwise I just get 0 values for mx, my, mz.

## To do list









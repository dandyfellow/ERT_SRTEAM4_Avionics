#pragma once
#include "driver/i2c.h"
#include "esp_err.h"

class QMC5883LCompass {
public:
	bool begin(i2c_port_t port, gpio_num_t sda, gpio_num_t scl, uint32_t freq = 100000);  // default 100kHz
	bool readRaw(int16_t &x, int16_t &y, int16_t &z);

private:
	i2c_port_t _port;
	uint8_t _addr;

	esp_err_t writeRegister(uint8_t reg, const uint8_t *data, size_t len);
	esp_err_t readRegister(uint8_t reg, uint8_t *data, size_t len);
};

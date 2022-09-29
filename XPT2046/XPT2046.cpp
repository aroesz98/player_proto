/*
 * XPT2046.cpp
 *
 *  Created on: Jan 27, 2022
 *      Author: asz
 */

#include <XPT2046.h>

void XPT2046_Touchscreen::init(void) {
	gpio_put(CS_PIN, 0);

	spiReceive(0x80);
	spiReceive(0x00);
	spiReceive(0x00);
	sleep_ms(10);

	gpio_put(CS_PIN, 1);
}

XPT2046_Touchscreen::XPT2046_Touchscreen(spi_inst_t *bus, uint16_t GPIO_CS_PIN, uint16_t GPIO_IRQ_PIN) {
	__bus = bus;

	CS_PIN	 = GPIO_CS_PIN;
	IRQ_PIN  = GPIO_IRQ_PIN;

	gpio_init(CS_PIN);
	gpio_init(IRQ_PIN);

	gpio_set_dir(CS_PIN, 1);
	gpio_set_dir(IRQ_PIN, 0);

	gpio_pull_up(IRQ_PIN);

	setRotation(3);
}

XPT2046_Touchscreen::~XPT2046_Touchscreen() { }

void XPT2046_Touchscreen::setRotation(uint8_t rotation) {
	__rotation = rotation % 4;
}

void XPT2046_Touchscreen::setSamplesNumber(uint8_t number_of_samples){
	if(number_of_samples >= 128) __no_samples = 128;
	__no_samples = number_of_samples;
}

bool XPT2046_Touchscreen::pressed(void)
{
	if(!gpio_get(IRQ_PIN)) return true;
	return false;
}

void XPT2046_Touchscreen::spiTransmit(uint8_t cmd) {
	spi_write_blocking(__bus, &cmd, 1);
}

uint8_t XPT2046_Touchscreen::spiReceive(uint8_t cmd) {
	uint8_t rx;
	spi_read_blocking(__bus, cmd, &rx, 1);
	return rx;
}

uint16_t XPT2046_Touchscreen::getVal(uint8_t address)
{
	uint16_t data = 0;
	gpio_put(CS_PIN, 0);

	spiTransmit(address);
	data = spiReceive(0x00);
	data <<= 8;

	data |= spiReceive(0x00);
	data >>= 3;

	gpio_put(CS_PIN, 1);
	return data;
}

bool XPT2046_Touchscreen::getTouch(uint16_t* x, uint16_t* y)
{
    uint32_t sum_x = 0;
    uint32_t sum_y = 0;
    uint8_t samples = 0;

    for(uint8_t i = 0; i < __no_samples; i++) {
        samples++;

        uint16_t x_raw;
        uint16_t y_raw;

        y_raw = getVal(__read_y);
        x_raw = getVal(__read_x);

        sum_x += x_raw;
        sum_y += y_raw;
    }

    if(samples < __no_samples) return false;

    uint32_t raw_x = (sum_x / __no_samples);
    if(raw_x < __min_x) raw_x = __min_x;
    if(raw_x > __max_x) raw_x = __max_x;

    uint32_t raw_y = (sum_y / __no_samples);
    if(raw_y < __min_y) raw_y = __min_y;
    if(raw_y > __max_y) raw_y = __max_y;

    switch(__rotation) {
    	case 0:
    	    if(touchCalibration_invert_x) {
    	    	*x = __scale_x - ((raw_x - __min_x) * __scale_x / (__max_x - __min_x));
    	    }

    	    else {
    	    	*x = (raw_x - __min_x) * __scale_x / (__max_x - __min_x);
    	    }

    	    if(touchCalibration_invert_y) {
    	    	*y = (raw_y - __min_y) * __scale_y / (__max_y - __min_y);
    	    }

    	    else {
    	    	*y = __scale_y - ((raw_y - __min_y) * __scale_y / (__max_y - __min_y));
    	    }
    	break;

    	case 1:
    	    if(touchCalibration_invert_x) {
    	    	*y = (raw_x - __min_x) * __scale_x / (__max_x - __min_x);
    	    }

    	    else {
    	    	*y = __scale_x - ((raw_x - __min_x) * __scale_x / (__max_x - __min_x));
    	    }

    	    if(touchCalibration_invert_y) {
    	    	*x = (raw_y - __min_y) * __scale_y / (__max_y - __min_y);
    	    }

    	    else {
    	    	*x = __scale_y - ((raw_y - __min_y) * __scale_y / (__max_y - __min_y));
    	    }
    	break;

    	case 2:
    	    if(touchCalibration_invert_x) {
    	    	*x = (raw_x - __min_x) * __scale_x / (__max_x - __min_x);
    	    }

    	    else {
    	    	*x = __scale_x - ((raw_x - __min_x) * __scale_x / (__max_x - __min_x));
    	    }

    	    if(touchCalibration_invert_y) {
    	    	*y = __scale_y - ((raw_y - __min_y) * __scale_y / (__max_y - __min_y));
    	    }

    	    else {
    	    	*y = (raw_y - __min_y) * __scale_y / (__max_y - __min_y);
    	    }
    	break;

    	case 3:
    	    if(touchCalibration_invert_x) {
    	    	*y = __scale_x - ((raw_x - __min_x) * __scale_x / (__max_x - __min_x));
    	    }

    	    else {
    	    	*y = (raw_x - __min_x) * __scale_x / (__max_x - __min_x);
    	    }

    	    if(touchCalibration_invert_y) {
    	    	*x = __scale_y - ((raw_y - __min_y) * __scale_y / (__max_y - __min_y));
    	    }

    	    else {
    	    	*x = (raw_y - __min_y) * __scale_y / (__max_y - __min_y);
    	    }
    	break;
    }

    return true;
}

void XPT2046_Touchscreen::getRaw(uint16_t* x, uint16_t* y)
{
    uint32_t sum_x = 0;
    uint32_t sum_y = 0;
    uint8_t samples = 0;

    for(uint8_t i = 0; i < __no_samples; i++) {
        samples++;

        uint16_t y_raw;
        uint16_t x_raw;

        y_raw = getVal(__read_y);
        x_raw = getVal(__read_x);

        sum_x += x_raw;
        sum_y += y_raw;
    }

    if(samples < __no_samples) return;

    uint32_t raw_x = (sum_x / __no_samples);
    uint32_t raw_y = (sum_y / __no_samples);


	*x = raw_x;
	*y = raw_y;
}

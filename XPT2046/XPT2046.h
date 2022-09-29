/*
 * XPT2046.h
 *
 *  Created on: Jan 27, 2022
 *      Author: asz
 */

#ifndef INC_XPT2046_H_
#define INC_XPT2046_H_

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "hardware/timer.h"

class XPT2046_Touchscreen {
	private:
		uint16_t __scale_x=480,
				 __scale_y=800;

		uint8_t __read_x=0xD0,
				__read_y=0x90,
				__read_z=0xC1,
				__no_samples=6;

		uint8_t __rotation = 0;

		uint16_t __min_x = 210,
				 __max_x = 3820,
				__min_y = 108,
				__max_y = 3978;

		bool touchCalibration_invert_x = false, touchCalibration_invert_y = false;

		spi_inst_t* __bus;

		uint CS_PIN, IRQ_PIN;

		void spiTransmit(uint8_t cmd);
		uint8_t spiReceive(uint8_t cmd);
		uint16_t getVal(uint8_t address);
	public:
		XPT2046_Touchscreen(spi_inst_t *bus, uint16_t GPIO_CS_PIN, uint16_t GPIO_IRQ_PIN);
		~XPT2046_Touchscreen();

		void init(void);
		void setRotation(uint8_t rotation);
		void setSamplesNumber(uint8_t number_of_samples);
		bool pressed(void);
		bool getTouch(uint16_t* x, uint16_t* y);
		void getRaw(uint16_t* x, uint16_t* y);
};

#endif

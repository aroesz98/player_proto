/*
 * TFTLIB_8BIT.cpp
 *
 *  Created on: Jan 10, 2022
 *  Updated on: Feb 20, 2022
 *      Author: asz
 */


#include <TFTLIB_8BIT.h>

using namespace std;

void PIN_INPUT (uint32_t GPIO_Mask)
{
    gpio_set_dir_in_masked(GPIO_Mask);
	gpio_set_pulls(0x3FC, 1, 0);
}

void PIN_OUTPUT (uint32_t GPIO_Mask)
{	
    gpio_set_dir_out_masked(GPIO_Mask);
}

/***************************************************************************************
** Function name:           TFTLIB_8BIT
** Description:             TFTLIB_8BIT Constructor
***************************************************************************************/
TFTLIB_8BIT::TFTLIB_8BIT(LCD_DRIVER drv, uint32_t GPIO_RD_PIN, uint32_t GPIO_WR_PIN, uint32_t GPIO_DC_PIN, uint32_t GPIO_CS_PIN, uint32_t GPIO_RST_PIN) {
	CS_PIN	 = GPIO_CS_PIN;

	DC_PIN	 = GPIO_DC_PIN;

	WR_PIN  = GPIO_WR_PIN;

	RD_PIN  = GPIO_RD_PIN;

	RST_PIN  = GPIO_RST_PIN;

	_type = drv;

	gpio_init(CS_PIN);
	gpio_init(DC_PIN);
	gpio_init(WR_PIN);
	gpio_init(RD_PIN);
	gpio_init(RST_PIN);

	gpio_set_dir(CS_PIN, 1);
	gpio_set_dir(DC_PIN, 1);
	gpio_set_dir(WR_PIN, 1);
	gpio_set_dir(RD_PIN, 1);
	gpio_set_dir(RST_PIN, 1);

	gpio_pull_up(CS_PIN);
	gpio_pull_up(DC_PIN);
	gpio_pull_up(WR_PIN);
	gpio_pull_up(RD_PIN);
	gpio_pull_up(RST_PIN);

	gpio_put(CS_PIN, 1);
	gpio_put(DC_PIN, 1);
	gpio_put(WR_PIN, 1);
	gpio_put(RD_PIN, 1);
	gpio_put(RST_PIN, 1);

	gpio_init_mask(0x3FC);
	PIN_OUTPUT(0x3FC);
}

/***************************************************************************************
** Function name:           ~TFTLIB_8BIT
** Description:             TFTLIB_8BIT Destructor
***************************************************************************************/
TFTLIB_8BIT::~TFTLIB_8BIT() {
	delete[] _buffer;
}

int16_t TFTLIB_8BIT::width(void){
	return _width;;
}

int16_t TFTLIB_8BIT::height(void){
	return _height;
}

uint16_t TFTLIB_8BIT::readID(void) {
	uint8_t data1, data2;
	uint16_t id = 0xFFFF;

	switch(_type) {
		case NT35510_PARALLEL:
			writeCommand16(0xDB00);
			PIN_INPUT(0x03FC);
			DC_H();
			CS_L();
			readByte();
			data1 = readByte();
			CS_H();
			PIN_OUTPUT(0x03FC);

			writeCommand16(0xDC00);
			PIN_INPUT(0x03FC);
			DC_H();
			CS_L();
			readByte();
			data2 = readByte();
			CS_H();
			PIN_OUTPUT(0x03FC);

			id = data1 << 8 | data2;
		break;

		case ILI9327_PARALLEL:
			writeCommand8(0xEF);

			PIN_INPUT(0x03FC);

			DC_H();
			CS_L();
			readByte();
			readByte();
			readByte();
			data1 = readByte();
			data2 = readByte();
			CS_H();
			PIN_OUTPUT(0x03FC);

			id = data1 << 8 | data2;
		break;

		case ILI9341_PARALLEL:
			writeCommand8(0xDB);

			PIN_INPUT(0x03FC);
			DC_H();
			CS_L();
			data1 = readByte();
			CS_H();
			PIN_OUTPUT(0x03FC);

			writeCommand8(0xDC);

			PIN_INPUT(0x03FC);
			DC_H();
			CS_L();
			data2 = readByte();
			CS_H();
			PIN_OUTPUT(0x03FC);

			id = data1 << 8 | data2;
		break;
	}

	return id;
}

/***************************************************************************************
** Function name:           init
** Description:             Init display with selected driver
***************************************************************************************/
void TFTLIB_8BIT::init(void){
	_display_width  = 480;
	_display_height = 800;

	if(_type == ILI9341_PARALLEL) {
		_display_width  = 240;
		_display_height = 320;
		#include "lcd_drivers/ili9341_parallel_drv.h"
	}

	else if(_type == ILI9327_PARALLEL) {
		_display_width  = 240;
		_display_height = 400;
		#include "lcd_drivers/ili9327_drv.h"
	}

	else if(_type == NT35510_PARALLEL) {
		_display_width  = 480;
		_display_height = 800;
		#include "lcd_drivers/nt35510_parallel_drv.h"
	}

	_tx0 = _tx1 = _ty0 = _ty1 = 0xFFFFFFFF;

	setRotation(3);
	fillScreen(BLACK);
	setFreeFont(&SegoeScript8pt7b);
	setCursor(0, 0);

	textfont  = 1;
	textsize  = 1;
	textcolor   =  0xFFFF; // White
	textbgcolor =  0x0000; // Black
	padX = 0;             // No padding
	isDigits   = false;   // No bounding box adjustment
	textwrapX  = true;    // Wrap text at end of line when using print stream
	textwrapY  = true;    // Wrap text at bottom of screen when using print stream
	textdatum = TL_DATUM; // Top Left text alignment is default
	_utf8     = true;
}

/***************************************************************************************
** Function name:           readByte
** Description:             Read 1 byte of data from display;
***************************************************************************************/
uint8_t TFTLIB_8BIT::readByte(void)
{
	uint32_t b = 0;

	RD_STROBE();
	
	b = ((gpio_get_all() & 0x3FC) >> 2);
	b = ((gpio_get_all() & 0x3FC) >> 2);
	b = ((gpio_get_all() & 0x3FC) >> 2);
	b = ((gpio_get_all() & 0x3FC) >> 2);

	RD_IDLE();

	return (b & 0xFF);
}

/***************************************************************************************
** Function name:           writeCommand8
** Description:             Writing command to parallel display
***************************************************************************************/
void TFTLIB_8BIT::writeCommand8 (uint8_t cmmd) {
	DC_L();
	CS_L();

	write8(cmmd);

	CS_H();
}

void TFTLIB_8BIT::writeCommand16 (uint16_t cmmd) {
	DC_L();
	CS_L();

	write16(cmmd);

	CS_H();
}

/***************************************************************************************
** Function name:           writeData8
** Description:             Write 8bit data to parallel display
***************************************************************************************/
void TFTLIB_8BIT::writeData8(uint8_t *data, uint32_t len) {
	DC_H();
	CS_L();

	while(len > 31) {
		write8(*data++); write8(*data++); write8(*data++); write8(*data++);
		write8(*data++); write8(*data++); write8(*data++); write8(*data++);
		write8(*data++); write8(*data++); write8(*data++); write8(*data++);
		write8(*data++); write8(*data++); write8(*data++); write8(*data++);
		write8(*data++); write8(*data++); write8(*data++); write8(*data++);
		write8(*data++); write8(*data++); write8(*data++); write8(*data++);
		write8(*data++); write8(*data++); write8(*data++); write8(*data++);
		write8(*data++); write8(*data++); write8(*data++); write8(*data++);
		len-=32;
	}

	while(len > 7) {
		write8(*data++); write8(*data++); write8(*data++); write8(*data++);
		write8(*data++); write8(*data++); write8(*data++); write8(*data++);
		len -= 8;
	}

	while(len-- > 0) write8(*data++);

	CS_H();
}

/***************************************************************************************
** Function name:           writeData16
** Description:             Write 16bit data to parallel display
***************************************************************************************/
void TFTLIB_8BIT::writeData16(uint16_t *data, uint32_t len) {
	DC_H();
	CS_L();

	while(len > 31) {
		write16(*data++); write16(*data++); write16(*data++); write16(*data++);
		write16(*data++); write16(*data++); write16(*data++); write16(*data++);
		write16(*data++); write16(*data++); write16(*data++); write16(*data++);
		write16(*data++); write16(*data++); write16(*data++); write16(*data++);
		write16(*data++); write16(*data++); write16(*data++); write16(*data++);
		write16(*data++); write16(*data++); write16(*data++); write16(*data++);
		write16(*data++); write16(*data++); write16(*data++); write16(*data++);
		write16(*data++); write16(*data++); write16(*data++); write16(*data++);
		len-=32;
	}

	while(len > 7) {
		write16(*data++); write16(*data++); write16(*data++); write16(*data++);
		write16(*data++); write16(*data++); write16(*data++); write16(*data++);
		len-=8;
	}

	while(len-- > 0) write16(*data++);
	CS_H();
}

/***************************************************************************************
** Function name:           writeSmallData8
** Description:             Write 8bit data via GPIO Port
***************************************************************************************/
inline void TFTLIB_8BIT::writeSmallData8(uint8_t data) {
	DC_H();
	CS_L();

	write8(data);

	CS_H();
}

/***************************************************************************************
** Function name:           writeSmallData16
** Description:             Write 16bit data via GPIO Port
***************************************************************************************/
inline void TFTLIB_8BIT::writeSmallData16 (uint16_t data) {
	DC_H();
	CS_L();

	write16(data);

	CS_H();
}

/***************************************************************************************
** Function name:           writeSmallData32
** Description:             Write 32bit data via GPIO Port
***************************************************************************************/
inline void TFTLIB_8BIT::writeSmallData32(uint32_t data) {
	DC_H();
	CS_L();

	write8(data>>24);
	write8(data>>16);
	write8(data>>8);
	write8(data);

	CS_H();
}

/***************************************************************************************
** Function name:           setRotation
** Description:             Set the rotation direction of the display
***************************************************************************************/
void TFTLIB_8BIT::setRotation(uint8_t m)
{
	m = m % 4;
	_rotation = m;

	if(_type == ILI9327_PARALLEL) {
		writeCommand8(MADCTL);
		switch (m) {
		case 0:
			writeSmallData8(MADCTL_MX | MADCTL_BGR);
			_width  = _display_width;
			_height = _display_height;
			break;
		case 1:
			writeSmallData8(MADCTL_MV | MADCTL_BGR);
			_width  = _display_height;
			_height = _display_width;
			break;
		case 2:
			writeSmallData8(MADCTL_MY | MADCTL_BGR);
			_width  = _display_width;
			_height = _display_height;
			break;
		case 3:
			writeSmallData8(MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
			_width  = _display_height;
			_height = _display_width;
			break;
		default:
			break;
		}
	}

	else if(_type == ILI9341_PARALLEL) {
		writeCommand8(MADCTL);
		switch (m) {
		case 0:
			writeSmallData8(MADCTL_MX | MADCTL_BGR);
			_width  = _display_width;
			_height = _display_height;
			break;
		case 1:
			writeSmallData8(MADCTL_MV | MADCTL_BGR);
			_width  = _display_height;
			_height = _display_width;
			break;
		case 2:
			writeSmallData8(MADCTL_MY | MADCTL_BGR);
			_width  = _display_width;
			_height = _display_height;
			break;
		case 3:
			writeSmallData8(MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
			_width  = _display_height;
			_height = _display_width;
			break;
		default:
			break;
		}
	}

	else if(_type == NT35510_PARALLEL) {
		writeCommand16(0x3600);
		switch (m) {
			case 0:
				writeSmallData16(MADCTL_RGB);
				_width  = _display_width;
				_height = _display_height;
			break;

			case 1:
				writeSmallData16(MADCTL_MX | MADCTL_MV | MADCTL_RGB);
				_width  = _display_height;
				_height = _display_width;
			break;

			case 2:
				writeSmallData16(MADCTL_MX | MADCTL_MY | MADCTL_RGB);
				_width  = _display_width;
				_height = _display_height;
			break;

			case 3:
				writeSmallData16(MADCTL_MV | MADCTL_MY | MADCTL_RGB);
				_width  = _display_height;
				_height = _display_width;
			break;

			default:
				break;
		}
	}
}

/***************************************************************************************
** Function name:           color565
** Description:             Convert value RGB888 to RGB565
***************************************************************************************/
uint16_t TFTLIB_8BIT::color565(uint8_t r, uint8_t g, uint8_t b) {
	uint16_t color = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
	return color;
}

/***************************************************************************************
** Function name:           color16to8
** Description:             Convert 16bit palette to 8bit palette
***************************************************************************************/
uint16_t TFTLIB_8BIT::color16to8(uint16_t c) {
  return ((c & 0xE000)>>8) | ((c & 0x0700)>>6) | ((c & 0x0018)>>3);
}

/***************************************************************************************
** Function name:           color8to16
** Description:             Convert 8bit palette to 16bit palette
***************************************************************************************/
uint16_t TFTLIB_8BIT::color8to16(uint8_t color) {
  uint8_t  blue[] = {0, 11, 21, 31}; // blue 2 to 5 bit colour lookup table
  uint16_t color16 = 0;

  //        =====Green=====     ===============Red==============
  color16  = (color & 0x1C)<<6 | (color & 0xC0)<<5 | (color & 0xE0)<<8;
  //        =====Green=====    =======Blue======
  color16 |= (color & 0x1C)<<3 | blue[color & 0x03];

  return color16;
}

/***************************************************************************************
** Function name:           alphaBlend
** Description:             Mix fgc & bgc with selected alpha channel(255 = full bgc)
***************************************************************************************/
uint16_t TFTLIB_8BIT::alphaBlend(uint8_t alpha, uint16_t fgc, uint16_t bgc)
{
  uint16_t fgR = ((fgc >> 10) & 0x3E) + 1;
  uint16_t fgG = ((fgc >>  4) & 0x7E) + 1;
  uint16_t fgB = ((fgc <<  1) & 0x3E) + 1;

  uint16_t bgR = ((bgc >> 10) & 0x3E) + 1;
  uint16_t bgG = ((bgc >>  4) & 0x7E) + 1;
  uint16_t bgB = ((bgc <<  1) & 0x3E) + 1;

  uint16_t r = (((fgR * alpha) + (bgR * (255 - alpha))) >> 9);
  uint16_t g = (((fgG * alpha) + (bgG * (255 - alpha))) >> 9);
  uint16_t b = (((fgB * alpha) + (bgB * (255 - alpha))) >> 9);

  return (r << 11) | (g << 5) | (b << 0);
}

/***************************************************************************************
** Function name:           setWindow8
** Description:             Set start/end address of drawed window (For parallel display)
***************************************************************************************/
void TFTLIB_8BIT::setWindow8(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
	CS_L();

	switch(_type) {
		case NT35510_PARALLEL:
			if(_tx0 != x0 || _tx1 != x1) {
				CS_L();
				DC_L();
				write16(0x2A00);
				DC_H();
				write8(0x00);
				write8(Byte8H(x0));

				DC_L();
				write16(0x2A01);
				DC_H();
				write8(0x00);
				write8(Byte8L(x0));

				DC_L();
				write16(0x2A02);
				DC_H();
				write8(0x00);
				write8(Byte8H(x1));

				DC_L();
				write16(0x2A03);
				DC_H();
				write8(0x00);
				write8(Byte8L(x1));
				_tx0 = x0;
				_tx1 = x1;
			}

			if(_ty0 != y0 || _ty1 != y1) {
				DC_L();
				write16(0x2B00);
				DC_H();
				write8(0x00);
				write8(Byte8H(y0));

				DC_L();
				write16(0x2B01);
				DC_H();
				write8(0x00);
				write8(Byte8L(y0));

				DC_L();
				write16(0x2B02);
				DC_H();
				write8(0x00);
				write8(Byte8H(y1));

				DC_L();
				write16(0x2B03);
				DC_H();
				write8(0x00);
				write8(Byte8L(y1));
				_ty0 = y0;
				_ty1 = y1;
			}

			DC_L();
			write16(0x2C00);
			DC_H();
			CS_H();
		break;

		case ILI9327_PARALLEL:
		    if (_rotation == 2) y0 += 32, y1 += 32;
		    if (_rotation == 3) x0 += 32, x1 += 32;

			if (_tx0 != x0 || _tx1 != x1) {
				writeCommand8(CASET);
				writeSmallData32(((uint32_t) x0 << 16) | x1);
				_tx0 = x0;
				_tx1 = x1;
			}

			if (_ty0 != y0 || _ty1 != y1) {
				writeCommand8(RASET);
				writeSmallData32(((uint32_t) y0 << 16) | y1);
				_ty0 = y0;
				_ty1 = y1;
			}

			writeCommand8(RAMWR);
		break;

		case ILI9341_PARALLEL:
			if (_tx0 != x0 || _tx1 != x1) {
				writeCommand8(CASET);
				writeSmallData32(((uint32_t) x0 << 16) | x1);
				_tx0 = x0;
				_tx1 = x1;
			}

			if (_ty0 != y0 || _ty1 != y1) {
				writeCommand8(RASET);
				writeSmallData32(((uint32_t) y0 << 16) | y1);
				_ty0 = y0;
				_ty1 = y1;
			}

			writeCommand8(RAMWR);
		break;
	}
}

/***************************************************************************************
** Function name:           setWindow8
** Description:             Set start/end address of drawed window (For parallel display)
***************************************************************************************/
void TFTLIB_8BIT::readWindow8(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	switch(_type) {
		case NT35510_PARALLEL:
			if(_tx0 != x0 || _tx1 != x1) {
				DC_L();
				CS_L();
				write16(0x2A00);
				DC_H();
				write8(0x00);
				write8(Byte8H(x0));

				DC_L();
				write16(0x2A01);
				DC_H();
				write8(0x00);
				write8(Byte8L(x0));

				DC_L();
				write16(0x2A02);
				DC_H();
				write8(0x00);
				write8(Byte8H(x1));

				DC_L();
				write16(0x2A03);
				DC_H();
				write8(0x00);
				write8(Byte8L(x1));
				_tx0 = x0;
				_tx1 = x1;
			}

			if(_ty0 != y0 || _ty1 != y1) {
				DC_L();
				write16(0x2B00);
				DC_H();
				write8(0x00);
				write8(Byte8H(y0));

				DC_L();
				write16(0x2B01);
				DC_H();
				write8(0x00);
				write8(Byte8L(y0));

				DC_L();
				write16(0x2B02);
				DC_H();
				write8(0x00);
				write8(Byte8H(y1));

				DC_L();
				write16(0x2B03);
				DC_H();
				write8(0x00);
				write8(Byte8L(y1));
				_ty0 = y0;
				_ty1 = y1;
			}

			DC_L();
			write16(0x2E00);
			DC_H();
			CS_H();
		break;

		case ILI9327_PARALLEL:
		    if (_rotation == 2) y0 += 32, y1 += 32;
		    if (_rotation == 3) x0 += 32, x1 += 32;

			if (_tx0 != x0 || _tx1 != x1) {
				writeCommand8(CASET);
				writeSmallData32(((uint32_t) x0 << 16) | x1);
				_tx0 = x0;
				_tx1 = x1;
			}

			if (_ty0 != y0 || _ty1 != y1) {
				writeCommand8(RASET);
				writeSmallData32(((uint32_t) y0 << 16) | y1);
				_ty0 = y0;
				_ty1 = y1;
			}

			writeCommand8(RAMRD);
		break;

		case ILI9341_PARALLEL:
			if (_tx0 != x0 || _tx1 != x1) {
				writeCommand8(CASET);
				writeSmallData32(((uint32_t) x0 << 16) | x1);
				_tx0 = x0;
				_tx1 = x1;
			}

			if (_ty0 != y0 || _ty1 != y1) {
				writeCommand8(RASET);
				writeSmallData32(((uint32_t) y0 << 16) | y1);
				_ty0 = y0;
				_ty1 = y1;
			}

			writeCommand8(RAMRD);
		break;
	}
}

/***************************************************************************************
** Function name:           pushPixels
** Description:             Write pixels from pointer (for JPEG Decoding)
***************************************************************************************/
void TFTLIB_8BIT::pushPixels8(const void* data_in, uint32_t len) {
	uint8_t *data = (uint8_t*)data_in;
	while (len-- > 0) writeData8(data++, len);
}

void TFTLIB_8BIT::pushPixels16(const void* data_in, uint32_t len) {
	DC_H();
	CS_L();
	uint16_t *data = (uint16_t*)data_in;
	while(len--) {
		write16(*data);
		data++;
	}
	CS_H();
}

/***************************************************************************************
** Function name:           pushBlock16
** Description:             Push block of 16bit data (For parallel display)
***************************************************************************************/
inline void TFTLIB_8BIT::pushBlock16(uint16_t color, uint32_t len = 1){
	DC_H();
	CS_L();

	while(len>127) {
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		len -= 128;
	}

	while(len>31) {
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		len -= 32;
	}

	while(len>7) {
		write16(color); write16(color); write16(color); write16(color);
		write16(color); write16(color); write16(color); write16(color);
		len -= 8;
	}

	while (len--) write16(color);

	CS_H();
}

/***************************************************************************************
** Function name:           fillScreen
** Description:             Fast color fillig screen function
***************************************************************************************/
void TFTLIB_8BIT::fillScreen(uint16_t color)
{
	setWindow8(0, 0, _width - 1, _height - 1);
	pushBlock16(color, _width * _height);
}

/***************************************************************************************
** Function name:           readPixel (PARALLEL DISPLAY ONLY!)
** Description:             Read single pixel at coords x&y
***************************************************************************************/
uint16_t TFTLIB_8BIT::readPixel(int32_t x0, int32_t y0)
{
	uint8_t data[3];

	readWindow8(x0, y0, x0, y0);

	DC_H();
	CS_L();

	// Set masked pins D0- D7 to input
	PIN_INPUT(0x3FC);

	readByte();
	data[0] = readByte();
	data[1] = readByte();
	data[2] = readByte();

	CS_H();

	// Set masked pins D0- D7 to output
	PIN_OUTPUT(0x03FC);

	return (((data[0] & 0xF8) << 8) | ((data[1] & 0xFC) << 3) | (data[2]  >> 3));
}

/***************************************************************************************
** Function name:           drawPixel
** Description:             Draw single pixel at coords x&y
***************************************************************************************/
void TFTLIB_8BIT::drawPixel(int32_t x, int32_t y, uint16_t color)
{
	if(x < 0) x = 0;
	if(y < 0) y = 0;
	if(x >= _width)		x = _width - 1;
	if(y >= _height)	y = _height - 1;

	setWindow8(x, y, x, y);
	pushBlock16(color, 1);
}

/***************************************************************************************
** Function name:           drawPixel (alpha blended)
** Description:             Draw a pixel blended with the screen or bg pixel colour
***************************************************************************************/
uint16_t TFTLIB_8BIT::drawPixel(int32_t x, int32_t y, uint32_t color, uint8_t alpha, uint32_t bg_color)
{
	if (bg_color == 0x00FFFFFF) bg_color = readPixel(x, y);
	color = alphaBlend(alpha, color, bg_color);
	drawPixel(x, y, color);
	return color;
}

/***************************************************************************************
** Function name:           drawPixelAlpha
** Description:             Draw single pixel with alpha channel
***************************************************************************************/
void TFTLIB_8BIT::drawPixelAlpha(int16_t x, int16_t y, uint16_t color, uint8_t alpha) {
	if(x < 0) x = 0;
	if(y < 0) y = 0;
	if(x >= _width)		x = _width - 1;
	if(y >= _height)	y = _height - 1;

	uint16_t px = readPixel(x, y);
	drawPixel(x, y, color, alphaBlend(alpha, color, px));
}

/***************************************************************************************
** Function name:           drawPixelAlpha
** Description:             Draw filled rectangle with vertical colour gradient
***************************************************************************************/
void TFTLIB_8BIT::fillRectVGradient(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color1, uint32_t color2)
{
	setWindow8(x, y, x + w - 1, y + h - 1);

	float delta = -255.0/h;
	float alpha = 255.0;
	uint32_t color = color1;

	while (h--) {
		pushBlock16(color, w);
		alpha += delta;
		color = alphaBlend((uint8_t)alpha, color1, color2);
	}
}

/***************************************************************************************
** Function name:           drawPixelAlpha
** Description:             Draw filled rectangle with horizontal colour gradient
***************************************************************************************/
void TFTLIB_8BIT::fillRectHGradient(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color1, uint32_t color2)
{
	float delta = -255.0/w;
	float alpha = 255.0;
	uint32_t color = color1;

	while (w--) {
		drawFastVLine(x++, y, h, color);
		alpha += delta;
		color = alphaBlend((uint8_t)alpha, color1, color2);
	}
}

/***************************************************************************************
** Function name:           drawSpot - maths intensive, so for small filled circles
** Description:             Draw an anti-aliased filled circle at ax,ay with radius r
***************************************************************************************/
void TFTLIB_8BIT::drawSpot(float ax, float ay, float r, uint32_t color) {
	// Filled circle can be created by the wide line function with zero line length
	drawWedgeLine( ax, ay, ax, ay, r, r, color, 0x00FFFFFF);
}

/***************************************************************************************
** Function name:           drawFastHLine
** Description:             Fast drawing Horizontal Line
***************************************************************************************/
void TFTLIB_8BIT::drawFastHLine(int32_t x, int32_t y, int32_t w, uint16_t color) {
	if (x < 0) { w += x; x = 0; }
	if ((x + w) > _width)  w = _width  - x;
	if (w < 1) return;

	setWindow8(x, y, x + w - 1, y);
	pushBlock16(color, w);
}

/***************************************************************************************
** Function name:           drawHLineAlpha
** Description:             Fast drawing Horizontal Line
***************************************************************************************/
void TFTLIB_8BIT::drawHLineAlpha(int32_t x, int32_t y, int32_t w, uint16_t color, uint8_t alpha) {
	if(x < 0) { w += x; x = 0; }
	if ((x + w) > _width)  w = _width  - x;
	if (w < 1) return;

	int32_t i = 0;

	while(i < w) {
		drawPixelAlpha(x + i, y, color, alpha);
		i++;
	}
}

/***************************************************************************************
** Function name:           drawFastVLine
** Description:             Drawing Vertical Line
***************************************************************************************/
void TFTLIB_8BIT::drawFastVLine(int32_t x, int32_t y, int32_t h, uint16_t color) {
	if (y < 0) { h += y; y = 0; }
	if ((y + h) > _height) h = _height - y;
	if (h < 1) return;

	setWindow8(x, y, x, y + h - 1);
	pushBlock16(color, h);
}

/***************************************************************************************
** Function name:           drawVLineAlpha
** Description:             Drawing Vertical Line
***************************************************************************************/
void TFTLIB_8BIT::drawVLineAlpha(int32_t x, int32_t y, int32_t h, uint16_t color, uint8_t alpha) {
	if (y < 0) { h += y; y = 0; }
	if ((y + h) > _height) h = _height - y;
	if (h < 1) return;

	int32_t i = 0;

	while(i < h) {
		drawPixelAlpha(x, y + i, color, alpha);
		++i;
	}
}

/***************************************************************************************
** Function name:           drawCircleHelper
** Description:             Support function for drawRoundRect()
***************************************************************************************/
inline void TFTLIB_8BIT::drawCircleHelper( int32_t x0, int32_t y0, int32_t rr, uint8_t cornername, uint16_t color) {
	if (rr <= 0) return;
	int32_t f     = 1 - rr;
	int32_t ddF_x = 1;
	int32_t ddF_y = -2 * rr;
	int32_t xe    = 0;
	int32_t xs    = 0;
	int32_t len   = 0;

	while (xe < rr--) {
		while (f < 0) {
			++xe;
			f += (ddF_x += 2);
		}

		f += (ddF_y += 2);

		if (xe-xs==1) {
			if (cornername & 0x1) { // left top
				drawPixel(x0 - xe, y0 - rr, color);
				drawPixel(x0 - rr, y0 - xe, color);
			}

			if (cornername & 0x2) { // right top
				drawPixel(x0 + rr    , y0 - xe, color);
				drawPixel(x0 + xs + 1, y0 - rr, color);
			}

			if (cornername & 0x4) { // right bottom
				drawPixel(x0 + xs + 1, y0 + rr    , color);
				drawPixel(x0 + rr, y0 + xs + 1, color);
			}

			if (cornername & 0x8) { // left bottom
				drawPixel(x0 - rr, y0 + xs + 1, color);
				drawPixel(x0 - xe, y0 + rr    , color);
			}
		}

		else {
			len = xe - xs++;
			if (cornername & 0x1) { // left top
				drawFastHLine(x0 - xe, y0 - rr, len, color);
				drawFastVLine(x0 - rr, y0 - xe, len, color);
			}

			if (cornername & 0x2) { // right top
				drawFastVLine(x0 + rr, y0 - xe, len, color);
				drawFastHLine(x0 + xs, y0 - rr, len, color);
			}

			if (cornername & 0x4) { // right bottom
				drawFastHLine(x0 + xs, y0 + rr, len, color);
				drawFastVLine(x0 + rr, y0 + xs, len, color);
			}

			if (cornername & 0x8) { // left bottom
				drawFastVLine(x0 - rr, y0 + xs, len, color);
				drawFastHLine(x0 - xe, y0 + rr, len, color);
			}
		}
		xs = xe;
	}
}

/***************************************************************************************
** Function name:           wideWedgeDistance
** Description:             Support function for drawWedgeLine
***************************************************************************************/
inline float TFTLIB_8BIT::wedgeLineDistance(float xpax, float ypay, float bax, float bay, float dr) {
	float h = fmaxf(fminf((xpax * bax + ypay * bay) / (bax * bax + bay * bay), 1.0f), 0.0f);
	float dx = xpax - bax * h, dy = ypay - bay * h;
	return sqrtf(dx * dx + dy * dy) + h * dr;
}

/***************************************************************************************
** Function name:           fillCircleHelper
** Description:             Support function for fillRoundRect()
***************************************************************************************/
inline void TFTLIB_8BIT::fillCircleHelper(int32_t x0, int32_t y0, int32_t r, uint8_t cornername, int32_t delta, uint16_t color) {
	int32_t f     = 1 - r;
	int32_t ddF_x = 1;
	int32_t ddF_y = -r - r;
	int32_t y     = 0;

	delta++;

	while (y < r) {
		if (f >= 0) {
			if (cornername & 0x1) drawFastHLine(x0 - y, y0 + r, y + y + delta, color);
			ddF_y += 2;
			f     += ddF_y;
			if (cornername & 0x2) drawFastHLine(x0 - y, y0 - r, y + y + delta, color);
			r--;
		}

		y++;
		if (cornername & 0x1) drawFastHLine(x0 - r, y0 + y, r + r + delta, color);

		ddF_x += 2;
		f     += ddF_x;
		if (cornername & 0x2) drawFastHLine(x0 - r, y0 - y, r + r + delta, color);
	}
}

/***************************************************************************************
** Function name:           fillCircleHelperAA
** Description:             Support function for fillRoundRectAA()
***************************************************************************************/
inline void TFTLIB_8BIT::fillCircleHelperAA(int32_t x0, int32_t y0, int32_t r, uint8_t cornername, int32_t delta, uint16_t color) {
	int32_t f     = 1 - r;
	int32_t ddF_x = 1;
	int32_t ddF_y = -r - r;
	int32_t y     = 0;

	delta++;

	while (y < r) {
		if (f >= 0) {
			if (cornername & 0x1) drawWideLine(x0 - y, y0 + r, x0 + y + delta, y0 + r, 1, color);
			ddF_y += 2;
			f     += ddF_y;
			if (cornername & 0x2) drawWideLine(x0 - y, y0 - r, x0 + y + delta, y0 - r, 1, color);
			r--;
		}

		y++;
		if (cornername & 0x1) drawWideLine(x0 - r, y0 + y, x0 + r + delta, y0 + y, 1, color);

		ddF_x += 2;
		f     += ddF_x;
		if (cornername & 0x2) drawWideLine(x0 - r, y0 - y, x0 + r + delta, y0 - y, 1, color);
	}
}

/***************************************************************************************
** Function name:           fillAlphaCircleHelper
** Description:             Support function for fillAlphaRoundRect()
***************************************************************************************/
inline void TFTLIB_8BIT::fillAlphaCircleHelper(int32_t x0, int32_t y0, int32_t r, uint8_t cornername, int32_t delta, uint16_t color, uint8_t alpha) {
	int32_t f     = 1 - r;
	int32_t ddF_x = 1;
	int32_t ddF_y = -r - r;
	int32_t y     = 0;

	delta++;

	while (y < r) {
		if (f >= 0) {
			if (cornername & 0x1) drawHLineAlpha(x0 - y, y0 + r, y + y + delta, color, alpha);
			ddF_y += 2;
			f     += ddF_y;
			if (cornername & 0x2) drawHLineAlpha(x0 - y, y0 - r, y + y + delta, color, alpha);
			r--;
		}

		y++;
		if (cornername & 0x1) drawHLineAlpha(x0 - r, y0 + y, r + r + delta, color, alpha);

		ddF_x += 2;
		f     += ddF_x;
		if (cornername & 0x2) drawHLineAlpha(x0 - r, y0 - y, r + r + delta, color, alpha);
	}
}

/***************************************************************************************
** Function name:           drawLine
** Description:             Draw a line with single color
***************************************************************************************/
void TFTLIB_8BIT::drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t color)
{
	bool steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		swap_coord(x0, y0);
		swap_coord(x1, y1);
	}

	if (x0 > x1) {
		swap_coord(x0, x1);
		swap_coord(y0, y1);
	}

	int32_t dx = x1 - x0, dy = abs(y1 - y0);

	int32_t err = dx >> 1, ystep = -1, xs = x0, dlen = 0;

	if (y0 < y1) ystep = 1;

	// Split into steep and not steep for FastH/V separation
	if (steep) {
		for (; x0 <= x1; x0++) {
			dlen++;
			err -= dy;
			if (err < 0) {
				if (dlen == 1) drawPixel(y0, xs, color);
				else drawFastVLine(y0, xs, dlen, color);
				dlen = 0;
				y0 += ystep; xs = x0 + 1;
				err += dx;
			}
		}
		if (dlen) drawFastVLine(y0, xs, dlen, color);
	}
	else {
		for (; x0 <= x1; x0++) {
			dlen++;
			err -= dy;
			if (err < 0) {
				if (dlen == 1) drawPixel(xs, y0, color);
				else drawFastHLine(xs, y0, dlen, color);
				dlen = 0;
				y0 += ystep; xs = x0 + 1;
				err += dx;
			}
		}
		if (dlen) drawFastHLine(xs, y0, dlen, color);
	}
}

/***************************************************************************************
** Function name:           drawWideLine
** Description:             Draw anti-aliased line with single color
***************************************************************************************/
void TFTLIB_8BIT::drawWideLine(float ax, float ay, float bx, float by, float wd, uint16_t fg_color, uint16_t bg_color) {
	drawWedgeLine( ax, ay, bx, by, wd/2.0, wd/2.0, fg_color, bg_color);
}

/***************************************************************************************
** Function name:           drawWedgeLine
** Description:             Draw anti-aliased line with single color
***************************************************************************************/
void TFTLIB_8BIT::drawWedgeLine(float ax, float ay, float bx, float by, float ar, float br, uint32_t fg_color, uint32_t bg_color)
{
	if ( (abs(ax - bx) < 0.01f) && (abs(ay - by) < 0.01f) ) bx += 0.01f;  // Avoid divide by zero

	// Find line bounding box
	int32_t x0 = (int32_t)floorf(fminf(ax-ar, bx-br));
	int32_t x1 = (int32_t) ceilf(fmaxf(ax+ar, bx+br));
	int32_t y0 = (int32_t)floorf(fminf(ay-ar, by-br));
	int32_t y1 = (int32_t) ceilf(fmaxf(ay+ar, by+br));

	// Establish x start and y start
	int32_t ys = ay;
	if ((ax-ar)>(bx-br)) ys = by;

	float rdt = ar - br; // Radius delta
	float alpha = 1.0f;
	ar += 0.5;

	uint16_t bg = bg_color;
	float xpax, ypay, bax = bx - ax, bay = by - ay;

	int32_t xs = x0;
	// Scan bounding box from ys down, calculate pixel intensity from distance to line
	for (int32_t yp = ys; yp <= y1; yp++) {
		bool swin = true;  // Flag to start new window area
		bool endX = false; // Flag to skip pixels
		ypay = yp - ay;
		for (int32_t xp = xs; xp <= x1; xp++) {
			if (endX) if (alpha <= LoAlphaTheshold) break;  // Skip right side
			xpax = xp - ax;
			alpha = ar - wedgeLineDistance(xpax, ypay, bax, bay, rdt);
			if (alpha <= LoAlphaTheshold ) continue;
			// Track edge to minimise calculations
			if (!endX) { endX = true; xs = xp; }
			if (alpha > HiAlphaTheshold) {
				if (swin) {
					setWindow8(xp, yp, width()-1, yp);
					swin = false;
				}

				pushBlock16(fg_color);
				continue;
			}
			//Blend color with background and plot
			if (bg_color == 0x00FFFFFF) {
				bg = readPixel(xp, yp); swin = true;
			}

			if (swin) {
				setWindow8(xp, yp, width()-1, yp);
				swin = false;
			}

			pushBlock16(alphaBlend((uint8_t)(alpha * PixelAlphaGain), fg_color, bg));
		}
	}

	// Reset x start to left side of box
	xs = x0;
	// Scan bounding box from ys-1 up, calculate pixel intensity from distance to line
	for (int32_t yp = ys-1; yp >= y0; yp--) {
		bool swin = true;  // Flag to start new window area
		bool endX = false; // Flag to skip pixels
		ypay = yp - ay;
		for (int32_t xp = xs; xp <= x1; xp++) {
			if (endX) if (alpha <= LoAlphaTheshold) break;  // Skip right side of drawn line
			xpax = xp - ax;
			alpha = ar - wedgeLineDistance(xpax, ypay, bax, bay, rdt);
			if (alpha <= LoAlphaTheshold ) continue;
			// Track line boundary
			if (!endX) { endX = true; xs = xp; }
			if (alpha > HiAlphaTheshold) {
				if (swin) { setWindow8(xp, yp, width()-1, yp); swin = false; }
				pushBlock16(fg_color);
				continue;
			}
			//Blend color with background and plot
			if (bg_color == 0x00FFFFFF) {
			bg = readPixel(xp, yp); swin = true;
			}
			if (swin) { setWindow8(xp, yp, width()-1, yp); swin = false; }
			pushBlock16(alphaBlend((uint8_t)(alpha * PixelAlphaGain), fg_color, bg));
		}
	}
}

/***************************************************************************************
** Function name:           fillSmoothCircle
** Description:             Draw a filled anti-aliased circle
***************************************************************************************/
void TFTLIB_8BIT::fillSmoothCircle(int32_t x, int32_t y, int32_t r, uint32_t color, uint32_t bg_color)
{
	if (r <= 0) return;

	drawFastHLine(x - r, y, 2 * r + 1, color);
	int32_t xs = 1;
	int32_t cx = 0;

	int32_t r1 = r * r;
	r++;
	int32_t r2 = r * r;

	for (int32_t cy = r - 1; cy > 0; cy--) {
		int32_t dy2 = (r - cy) * (r - cy);
		for (cx = xs; cx < r; cx++) {
			int32_t hyp2 = (r - cx) * (r - cx) + dy2;
			if (hyp2 <= r1) break;
			if (hyp2 >= r2) continue;
			float alphaf = (float)r - sqrtf(hyp2);
			if (alphaf > HiAlphaTheshold) break;
			xs = cx;
			if (alphaf < LoAlphaTheshold) continue;
			uint8_t alpha = alphaf * 255;

			if (bg_color == 0x00FFFFFF) {
				drawPixel(x + cx - r, y + cy - r, color, alpha, bg_color);
				drawPixel(x - cx + r, y + cy - r, color, alpha, bg_color);
				drawPixel(x - cx + r, y - cy + r, color, alpha, bg_color);
				drawPixel(x + cx - r, y - cy + r, color, alpha, bg_color);
			}

			else {
				uint16_t pcol = drawPixel(x + cx - r, y + cy - r, color, alpha, bg_color);
				drawPixel(x - cx + r, y + cy - r, pcol);
				drawPixel(x - cx + r, y - cy + r, pcol);
				drawPixel(x + cx - r, y - cy + r, pcol);
			}
		}
		drawFastHLine(x + cx - r, y + cy - r, 2 * (r - cx) + 1, color);
		drawFastHLine(x + cx - r, y - cy + r, 2 * (r - cx) + 1, color);
	}
}

/***************************************************************************************
** Function name:           fillSmoothRoundRect
** Description:             Draw a filled anti-aliased rounded corner rectangle
***************************************************************************************/
void TFTLIB_8BIT::fillSmoothRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint32_t color, uint32_t bg_color) {
	int32_t xs = 0;
	int32_t cx = 0;

	y += r;
	h -= 2*r;

	fillRect(x, y, w, h, color);

	h--;
	x += r;
	w -= 2*r+1;

	int32_t r1 = r * r;

	r++;

	int32_t r2 = r * r;

	for (int32_t cy = r - 1; cy > 0; cy--) {
		int32_t dy2 = (r - cy) * (r - cy);
		for (cx = xs; cx < r; cx++) {
			int32_t hyp2 = (r - cx) * (r - cx) + dy2;

			if (hyp2 <= r1) break;
			if (hyp2 >= r2) continue;

			float alphaf = (float)r - sqrtf(hyp2);

			if (alphaf > HiAlphaTheshold) break;

			xs = cx;

			if (alphaf < LoAlphaTheshold) continue;

			uint8_t alpha = alphaf * 255;

			drawPixel(x + cx - r, y + cy - r, color, alpha, bg_color);
			drawPixel(x - cx + r + w, y + cy - r, color, alpha, bg_color);
			drawPixel(x - cx + r + w, y - cy + r + h, color, alpha, bg_color);
			drawPixel(x + cx - r, y - cy + r + h, color, alpha, bg_color);
		}

		drawFastHLine(x + cx - r, y + cy - r, 2 * (r - cx) + 1 + w, color);
		drawFastHLine(x + cx - r, y - cy + r + h, 2 * (r - cx) + 1 + w, color);
	}
}

/***************************************************************************************
** Function name:           drawTriangle
** Description:             Draw a triangle with single color
***************************************************************************************/
void TFTLIB_8BIT::drawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint16_t color)
{
	drawLine(x1, y1, x2, y2, color);
	drawLine(x2, y2, x3, y3, color);
	drawLine(x3, y3, x1, y1, color);
}

/***************************************************************************************
** Function name:           drawTriangleAA
** Description:             Draw anti-aliased triangle with single color and specified thickness
***************************************************************************************/
void TFTLIB_8BIT::drawTriangleAA(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, int32_t thickness, uint16_t color) {
	drawWideLine(x1, y1, x2, y2, thickness, color);
	drawWideLine(x2, y2, x3, y3, thickness, color);
	drawWideLine(x3, y3, x1, y1, thickness, color);
}

/***************************************************************************************
** Function name:           drawRect
** Description:             Draw a rectangle with single color
***************************************************************************************/
void TFTLIB_8BIT::drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color)
{
	if (x < 0) { w += x; x = 0; }
	if (y < 0) { h += y; y = 0; }
	if ((x + w) > _width)  w = _width  - x;
	if ((y + h) > _height) h = _height - y;

	drawFastHLine(x, y, w, color);
	drawFastVLine(x, y, h, color);
	drawFastVLine(x+w, y, h, color);
	drawFastHLine(x, y+h, w, color);
}

/***************************************************************************************
** Function name:           drawRectAA
** Description:             Draw anti-aliased rectangle with single color
***************************************************************************************/
void TFTLIB_8BIT::drawRectAA(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color)
{
	if (x < 0) { w += x; x = 0; }
	if (y < 0) { h += y; y = 0; }
	if ((x + w) > _width)  w = _width  - x;
	if ((y + h) > _height) h = _height - y;

	drawWideLine(x, y, x + w, y, 1, color);
	drawWideLine(x, y, x, y + h, 1, color);
	drawWideLine(x + w, y, x + w, y + h, 1, color);
	drawWideLine(x, y + h, x + w, y + h, 1, color);
}

/***************************************************************************************
** Function name:           drawRoundRect
** Description:             Draw a rectangle with rounded corners
***************************************************************************************/
void TFTLIB_8BIT::drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color)
{
	if (x < 0) { w += x; x = 0; }
	if (y < 0) { h += y; y = 0; }
	if ((x + w) > _width)  w = _width  - x;
	if ((y + h) > _height) h = _height - y;

	drawFastHLine(x + r  , y    , w - r - r, color); // Top
	drawFastHLine(x + r  , y + h - 1, w - r - r, color); // Bottom
	drawFastVLine(x    , y + r  , h - r - r, color); // Left
	drawFastVLine(x + w - 1, y + r  , h - r - r, color); // Right

	// draw four corners
	drawCircleHelper(x + r    , y + r    , r, 1, color);
	drawCircleHelper(x + w - r - 1, y + r    , r, 2, color);
	drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
	drawCircleHelper(x + r    , y + h - r - 1, r, 8, color);
}

/***************************************************************************************
** Function name:           drawCircle
** Description:             Draw a circle with single color
***************************************************************************************/
void TFTLIB_8BIT::drawCircle(int32_t x0, int32_t y0, int32_t r, uint16_t color)
{
	int32_t f     = 1 - r;
	int32_t ddF_y = -2 * r;
	int32_t ddF_x = 1;
	int32_t xs    = -1;
	int32_t xe    = 0;
	int32_t len   = 0;

	bool first = true;
	do {
		while (f < 0) {
			++xe;
			f += (ddF_x += 2);
		}
		f += (ddF_y += 2);

		if (xe-xs>1) {
			if (first) {
				len = 2*(xe - xs)-1;
				drawFastHLine(x0 - xe, y0 + r, len, color);
				drawFastHLine(x0 - xe, y0 - r, len, color);
				drawFastVLine(x0 + r, y0 - xe, len, color);
				drawFastVLine(x0 - r, y0 - xe, len, color);
				first = false;
			}
			else {
				len = xe - xs++;
				drawFastHLine(x0 - xe, y0 + r, len, color);
				drawFastHLine(x0 - xe, y0 - r, len, color);
				drawFastHLine(x0 + xs, y0 - r, len, color);
				drawFastHLine(x0 + xs, y0 + r, len, color);

				drawFastVLine(x0 + r, y0 + xs, len, color);
				drawFastVLine(x0 + r, y0 - xe, len, color);
				drawFastVLine(x0 - r, y0 - xe, len, color);
				drawFastVLine(x0 - r, y0 + xs, len, color);
			}
		}
		else {
			++xs;
			drawPixel(x0 - xe, y0 + r, color);
			drawPixel(x0 - xe, y0 - r, color);
			drawPixel(x0 + xs, y0 - r, color);
			drawPixel(x0 + xs, y0 + r, color);

			drawPixel(x0 + r, y0 + xs, color);
			drawPixel(x0 + r, y0 - xe, color);
			drawPixel(x0 - r, y0 - xe, color);
			drawPixel(x0 - r, y0 + xs, color);
		}
		xs = xe;
	} while (xe < --r);
}

/***************************************************************************************
** Function name:           drawEllipse
** Description:             Draw an ellipse with single color
***************************************************************************************/
void TFTLIB_8BIT::drawEllipse(int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color)
{
	if(x0 - rx < 0 || x0 + rx > _width || y0 - ry < 0 || y0 + ry > _height || rx < 2 || ry < 2) return;
	int32_t x, y;
	int32_t rx2 = rx * rx;
	int32_t ry2 = ry * ry;
	int32_t fx2 = 4 * rx2;
	int32_t fy2 = 4 * ry2;
	int32_t s;

	for (x = 0, y = ry, s = 2*ry2+rx2*(1-2*ry); ry2*x <= rx2*y; x++) {
		drawPixel(x0 + x, y0 + y, color);
		drawPixel(x0 - x, y0 + y, color);
		drawPixel(x0 - x, y0 - y, color);
		drawPixel(x0 + x, y0 - y, color);
		if (s >= 0) {
			s += fx2 * (1 - y);
			y--;
		}
		s += ry2 * ((4 * x) + 6);
	}

	for (x = rx, y = 0, s = 2*rx2+ry2*(1-2*rx); rx2*y <= ry2*x; y++) {
		drawPixel(x0 + x, y0 + y, color);
		drawPixel(x0 - x, y0 + y, color);
		drawPixel(x0 - x, y0 - y, color);
		drawPixel(x0 + x, y0 - y, color);
		if (s >= 0) {
			s += fy2 * (1 - x);
			x--;
		}
		s += rx2 * ((4 * y) + 6);
	}
}

/***************************************************************************************
** Function name:           fillTriangle
** Description:             Draw filled triangle with fixed color
***************************************************************************************/
void TFTLIB_8BIT::fillTriangle ( int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color)
{
	int32_t a, b, y, last;

	if (y0 > y1) {
		swap_coord(y0, y1); swap_coord(x0, x1);
	}

	if (y1 > y2) {
		swap_coord(y2, y1); swap_coord(x2, x1);
	}

	if (y0 > y1) {
		swap_coord(y0, y1); swap_coord(x0, x1);
	}

	if (y0 == y2) {
		a = b = x0;
		if (x1 < a)      a = x1;
		else if (x1 > b) b = x1;
		if (x2 < a)      a = x2;
		else if (x2 > b) b = x2;
		drawFastHLine(a, y0, b - a + 1, color);
		return;
	}

	int32_t
	dx01 = x1 - x0,
	dy01 = y1 - y0,
	dx02 = x2 - x0,
	dy02 = y2 - y0,
	dx12 = x2 - x1,
	dy12 = y2 - y1,
	sa   = 0,
	sb   = 0;

	if (y1 == y2) last = y1;
	else last = y1 - 1;

	for (y = y0; y <= last; y++) {
		a   = x0 + sa / dy01;
		b   = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;

		if (a > b) swap_coord(a, b);
		drawFastHLine(a, y, b - a + 1, color);
	}

	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);

	for (; y <= y2; y++) {
		a   = x1 + sa / dy12;
		b   = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;

		if (a > b) swap_coord(a, b);
		drawFastHLine(a, y, b - a + 1, color);
	}
}

/***************************************************************************************
** Function name:           fillTriangleAA
** Description:             Draw anti-aliased filled triangle with fixed color
***************************************************************************************/
void TFTLIB_8BIT::fillTriangleAA( int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color)
{
	int32_t a, b, y, last;

	if (y0 > y1) {
		swap_coord(y0, y1); swap_coord(x0, x1);
	}

	if (y1 > y2) {
		swap_coord(y2, y1); swap_coord(x2, x1);
	}

	if (y0 > y1) {
		swap_coord(y0, y1); swap_coord(x0, x1);
	}

	if (y0 == y2) {
		a = b = x0;
		if (x1 < a)      a = x1;
		else if (x1 > b) b = x1;
		if (x2 < a)      a = x2;
		else if (x2 > b) b = x2;
		drawWideLine(a, y0, b + 1, y0, 1, color);
		return;
	}

	int32_t
	dx01 = x1 - x0,
	dy01 = y1 - y0,
	dx02 = x2 - x0,
	dy02 = y2 - y0,
	dx12 = x2 - x1,
	dy12 = y2 - y1,
	sa   = 0,
	sb   = 0;

	if (y1 == y2) last = y1;
	else last = y1 - 1;

	for (y = y0; y <= last; y++) {
		a   = x0 + sa / dy01;
		b   = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;

		if (a > b) swap_coord(a, b);
		drawWideLine(a, y, b + 1, y, 1, color);
	}

	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);

	for (; y <= y2; y++) {
		a   = x1 + sa / dy12;
		b   = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;

		if (a > b) swap_coord(a, b);
		drawWideLine(a, y, b + 1, y, 1, color);
	}
}

/***************************************************************************************
** Function name:           fillTriangleAlpha
** Description:             Draw filled triangle with alpha channel
***************************************************************************************/
void TFTLIB_8BIT::fillTriangleAlpha( int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color, uint8_t alpha) {
	int32_t a, b, y, last;

	if (y0 > y1) {
		swap_coord(y0, y1); swap_coord(x0, x1);
	}

	if (y1 > y2) {
		swap_coord(y2, y1); swap_coord(x2, x1);
	}

	if (y0 > y1) {
		swap_coord(y0, y1); swap_coord(x0, x1);
	}

	if (y0 == y2) {
		a = b = x0;
		if (x1 < a)      a = x1;
		else if (x1 > b) b = x1;
		if (x2 < a)      a = x2;
		else if (x2 > b) b = x2;
		drawHLineAlpha(a, y0, b - a, color, alpha);
		return;
	}

	int32_t
	dx01 = x1 - x0,
	dy01 = y1 - y0,
	dx02 = x2 - x0,
	dy02 = y2 - y0,
	dx12 = x2 - x1,
	dy12 = y2 - y1,
	sa   = 0,
	sb   = 0;

	if (y1 == y2) last = y1;
	else last = y1 - 1;

	for (y = y0; y <= last; y++) {
		a   = x0 + sa / dy01;
		b   = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;

		if (a > b) swap_coord(a, b);
		drawHLineAlpha(a, y, b - a, color, alpha);
	}

	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);
	for (; y <= y2; y++) {
		a   = x1 + sa / dy12;
		b   = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;

		if (a > b) swap_coord(a, b);
		drawHLineAlpha(a, y, b - a, color, alpha);
	}
}

/***************************************************************************************
** Function name:           fillRect
** Description:             Draw a filled rectangle with fixed color
***************************************************************************************/
void TFTLIB_8BIT::fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color) {
	if (x < 0) { w += x; x = 0; }
	if (y < 0) { h += y; y = 0; }
	if ((x + w) > _width)  w = _width  - x;
	if ((y + h) > _height) h = _height - y;

	setWindow8(x, y, x + w - 1, y + h - 1);
	pushBlock16(color, w * h);
}

/***************************************************************************************
** Function name:           fillRectAA
** Description:             Draw anti-aliased filled rectangle with fixed color
***************************************************************************************/
void TFTLIB_8BIT::fillRectAA(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color)
{
	setWindow8(x, y, x + w - 1, y + h - 1);
	pushBlock16(color, w * h);

	drawWideLine(x, y, x + w - 1, y, 1, color);
	drawWideLine(x, y + h - 1, x + w - 1, y + h - 1, 1, color);

	drawWideLine(x, y, x, y + h - 1, 1, color);
	drawWideLine(x + w, y, x + w, y + h - 1, 1, color);
}

/***************************************************************************************
** Function name:           fillRectAlpha
** Description:             Draw a filled rectangle with alpha channel
***************************************************************************************/
void TFTLIB_8BIT::fillRectAlpha(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color, uint8_t alpha)
{
	int z = 0;

	while(z < h) {
		drawHLineAlpha(x, y + z, w, color, alpha);
		++z;
	}
}

/***************************************************************************************
** Function name:           fillRoundRect
** Description:             Draw a filled rectangle with rounded corners & single color
***************************************************************************************/
void TFTLIB_8BIT::fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color) {
	fillRect(x, y + r, w, h - r - r, color);

	fillCircleHelper(x + r, y + h - r - 1, r, 1, w - r - r - 1, color);
	fillCircleHelper(x + r    , y + r, r, 2, w - r - r - 1, color);
}

/***************************************************************************************
** Function name:           fillRoundRectAA
** Description:             Draw a filled rectangle with rounded corners & single color
***************************************************************************************/
void TFTLIB_8BIT::fillRoundRectAA(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color) {
	fillRectAA(x, y + r, w, h - r - r, color);

	fillCircleHelperAA(x + r, y + h - r - 1, r, 1, w - r - r - 1, color);
	fillCircleHelperAA(x + r    , y + r, r, 2, w - r - r - 1, color);
}

/***************************************************************************************
** Function name:           fillAlphaRoundRect
** Description:             Draw filled rectangle with rounded corners & alpha channel
***************************************************************************************/
void TFTLIB_8BIT::fillAlphaRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color, uint8_t alpha) {
	fillRectAlpha(x, y + r, w, h - r - r, color, alpha);

	fillAlphaCircleHelper(x + r, y + h - r - 1, r, 1, w - r - r - 1, color, alpha);
	fillAlphaCircleHelper(x + r    , y + r, r, 2, w - r - r - 1, color, alpha);
}

/***************************************************************************************
** Function name:           fillCircle
** Description:             Draw a filled circle with single color
***************************************************************************************/
void TFTLIB_8BIT::fillCircle(int32_t x0, int32_t y0, int32_t r, uint16_t color) {
	int32_t  x  = 0;
	int32_t  dx = 1;
	int32_t  dy = r+r;
	int32_t  p  = -(r>>1);

	drawFastHLine(x0 - r, y0, dy+1, color);

	while(x<r){
		if(p>=0) {
			drawFastHLine(x0 - x + 1, y0 + r, dx-1, color);
			drawFastHLine(x0 - x + 1, y0 - r, dx-1, color);
			dy-=2;
			p-=dy;
			r--;
		}

		dx+=2;
		p+=dx;
		x++;

		drawFastHLine(x0 - r, y0 + x, dy+1, color);
		drawFastHLine(x0 - r, y0 - x, dy+1, color);
	}
}

/***************************************************************************************
** Function name:           fillCircleAA
** Description:             Draw anti-aliased filled circle with fixed color
***************************************************************************************/
void TFTLIB_8BIT::fillCircleAA(float x, float y, float r, uint16_t color) {
	drawWedgeLine(x, y, x, y, r, r, color, 0xFFFF);
}

/***************************************************************************************
** Function name:           fillCircleAlpha
** Description:             Draw filled circle with alpha channel
***************************************************************************************/
void TFTLIB_8BIT::fillCircleAlpha(int32_t x0, int32_t y0, int32_t r, uint16_t color, uint8_t alpha) {
	int32_t  x  = 0;
	int32_t  dx = 1;
	int32_t  dy = r+r;
	int32_t  p  = -(r>>1);

	drawHLineAlpha(x0 - r, y0, dy+1, color, alpha);

	while(x<r){
		if(p>=0) {
			drawHLineAlpha(x0 - x + 1, y0 + r, dx-1, color, alpha);
			drawHLineAlpha(x0 - x + 1, y0 - r, dx-1, color, alpha);
			dy-=2;
			p-=dy;
			r--;
		}

		dx+=2;
		p+=dx;
		x++;

		drawHLineAlpha(x0 - r, y0 + x, dy+1, color, alpha);
		drawHLineAlpha(x0 - r, y0 - x, dy+1, color, alpha);
	}
}

/***************************************************************************************
** Function name:           fillEllipse
** Description:             Draw a filled ellipse with single color
***************************************************************************************/
void TFTLIB_8BIT::fillEllipse(int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color) {
	int32_t x, y;
	int32_t rx2 = rx * rx;
	int32_t ry2 = ry * ry;
	int32_t fx2 = 4 * rx2;
	int32_t fy2 = 4 * ry2;
	int32_t s;

	for (x = 0, y = ry, s = 2*ry2+rx2*(1-2*ry); ry2*x <= rx2*y; x++) {
		drawFastHLine(x0 - x, y0 - y, x + x + 1, color);
		drawFastHLine(x0 - x, y0 + y, x + x + 1, color);

		if (s >= 0) {
			s += fx2 * (1 - y);
			y--;
		}
		s += ry2 * ((4 * x) + 6);
	}

	for (x = rx, y = 0, s = 2*rx2+ry2*(1-2*rx); rx2*y <= ry2*x; y++) {
		drawFastHLine(x0 - x, y0 - y, x + x + 1, color);
		drawFastHLine(x0 - x, y0 + y, x + x + 1, color);

		if (s >= 0) {
			s += fy2 * (1 - x);
			x--;
		}
		s += rx2 * ((4 * y) + 6);
	}
}

/***************************************************************************************
** Function name:           drawImage
** Description:             Draw image at coords x&y
***************************************************************************************/
void TFTLIB_8BIT::drawImage(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t *data) {
	_locked = true;
	setWindow8(x, y, x + w - 1, y + h - 1);
	writeData16(data, w * h);
	_locked = false;
}

/***************************************************************************************
** Function name:           pushImage
** Description:             plot 8 bit or 4 bit or 1 bit image or sprite using a line buffer
***************************************************************************************/
void TFTLIB_8BIT::pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t *data, bool bpp8,  uint16_t *cmap)
{
  setWindow8(x, y, x + w - 1, y + h - 1); // Sets CS low and sent RAMWR

  // Line buffer makes plotting faster
  uint16_t  lineBuf[w];

  if (bpp8)
  {
    uint8_t  blue[] = {0, 11, 21, 31}; // blue 2 to 5 bit colour lookup table

    _lastColor = -1; // Set to illegal value

    // Used to store last shifted colour
    uint8_t msbColor = 0;
    uint8_t lsbColor = 0;

    data += x + y * w;
    while (h--) {
      uint32_t len = w;
      uint8_t* ptr = (uint8_t*)data;
      uint8_t* linePtr = (uint8_t*)lineBuf;

      while(len--) {
        uint32_t color = read_byte(ptr++);

        // Shifts are slow so check if colour has changed first
        if (color != _lastColor) {
          //          =====Green=====     ===============Red==============
          msbColor = (color & 0x1C)>>2 | (color & 0xC0)>>3 | (color & 0xE0);
          //          =====Green=====    =======Blue======
          lsbColor = (color & 0x1C)<<3 | blue[color & 0x03];
          _lastColor = color;
        }

       *linePtr++ = msbColor;
       *linePtr++ = lsbColor;
      }

      pushPixels8(lineBuf, w);

      data += w;
    }
  }
  else if (cmap != nullptr) // Must be 4bpp
  {
    w = (w+1) & 0xFFFE;   // if this is a sprite, w will already be even; this does no harm.
    bool splitFirst = (x & 0x01) != 0; // split first means we have to push a single px from the left of the sprite / image

    if (splitFirst) {
      data += ((x - 1 + y * w) >> 1);
    }
    else {
      data += ((x + y * w) >> 1);
    }

    while (h--) {
      uint32_t len = w;
      uint8_t * ptr = (uint8_t*)data;
      uint16_t *linePtr = lineBuf;
      uint8_t colors; // two colors in one byte
      uint16_t index;

      if (splitFirst) {
        colors = read_byte(ptr);
        index = (colors & 0x0F);
        *linePtr++ = cmap[index];
        len--;
        ptr++;
      }

      while (len--)
      {
        colors = read_byte(ptr);
        index = ((colors & 0xF0) >> 4) & 0x0F;
        *linePtr++ = cmap[index];

        if (len--)
        {
          index = colors & 0x0F;
          *linePtr++ = cmap[index];
        } else {
          break;  // nothing to do here
        }

        ptr++;
      }

      pushPixels8(lineBuf, w);
      data += (w >> 1);
    }
  }
  else // Must be 1bpp
  {
    uint8_t * ptr = (uint8_t*)data;
    uint32_t ww =  (w+7)>>3; // Width of source image line in bytes
    for (int32_t yp = y;  yp < y + h; yp++)
    {
      uint8_t* linePtr = (uint8_t*)lineBuf;
      for (int32_t xp = x; xp < x + w; xp++)
      {
        uint16_t col = (read_byte(ptr + (xp>>3)) & (0x80 >> (xp & 0x7)) );
        if (col) {*linePtr++ = bitmap_fg>>8; *linePtr++ = (uint8_t) bitmap_fg;}
        else     {*linePtr++ = bitmap_bg>>8; *linePtr++ = (uint8_t) bitmap_bg;}
      }
      ptr += ww;
      pushPixels8(lineBuf, w);
    }
  }
}

void TFTLIB_8BIT::pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t *data, uint16_t transp)
{

  data += x + y * w;


  uint16_t  lineBuf[w];

  transp = transp >> 8 | transp << 8;

  while (h--) {
    int32_t len = w;
    uint16_t* ptr = (uint16_t*)data;
    int32_t px = x, sx = x;
    bool move = true;

    uint16_t np = 0;

    while (len--) {
      uint16_t color = read_word(ptr);
      if (transp != color) {
        if (move) { move = false; sx = px; }
        lineBuf[np] = color;
        np++;
      }
      else {
        move = true;
        if (np) {
          setWindow8(sx, y, sx + np - 1, y);
          pushPixels8(lineBuf, np);
          np = 0;
        }
      }
      px++;
      ptr++;
    }
    if (np) { setWindow8(sx, y, sx + np - 1, y); pushPixels8(lineBuf, np); }

    y++;
    data += w;
  }
}

/***************************************************************************************
** Function name:           pushImage
** Description:             plot 8 or 4 or 1 bit image or sprite with a transparent colour
***************************************************************************************/
void TFTLIB_8BIT::pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t *data, uint8_t transp, bool bpp8, uint16_t *cmap)
{
  // Line buffer makes plotting faster
  uint16_t  lineBuf[w];

  if (bpp8) { // 8 bits per pixel

    data += x + y * w;

    uint8_t  blue[] = {0, 11, 21, 31}; // blue 2 to 5 bit colour lookup table

    _lastColor = -1; // Set to illegal value

    // Used to store last shifted colour
    uint8_t msbColor = 0;
    uint8_t lsbColor = 0;

    while (h--) {
      int32_t len = w;
      uint8_t* ptr = data;
      uint8_t* linePtr = (uint8_t*)lineBuf;

      int32_t px = x, sx = x;
      bool move = true;
      uint16_t np = 0;

      while (len--) {
        if (transp != *ptr) {
          if (move) { move = false; sx = px; }
          uint8_t color = *ptr;

          // Shifts are slow so check if colour has changed first
          if (color != _lastColor) {
            //          =====Green=====     ===============Red==============
            msbColor = (color & 0x1C)>>2 | (color & 0xC0)>>3 | (color & 0xE0);
            //          =====Green=====    =======Blue======
            lsbColor = (color & 0x1C)<<3 | blue[color & 0x03];
            _lastColor = color;
          }
          *linePtr++ = msbColor;
          *linePtr++ = lsbColor;
          np++;
        }
        else {
          move = true;
          if (np) {
            setWindow8(sx, y, sx + np - 1, y);
            pushPixels8(lineBuf, np);
            linePtr = (uint8_t*)lineBuf;
            np = 0;
          }
        }
        px++;
        ptr++;
      }

      if (np) { setWindow8(sx, y, sx + np - 1, y); pushPixels8(lineBuf, np); }
      y++;
      data += w;
    }
  }
  else if (cmap != nullptr) // 4bpp with color map
  {
    w = (w+1) & 0xFFFE; // here we try to recreate iwidth from dwidth.
    bool splitFirst = ((x & 0x01) != 0);
    if (splitFirst) {
      data += ((x - 1 + y * w) >> 1);
    }
    else {
      data += ((x + y * w) >> 1);
    }

    while (h--) {
      uint32_t len = w;
      uint8_t * ptr = data;

      int32_t px = x, sx = x;
      bool move = true;
      uint16_t np = 0;

      uint8_t index;  // index into cmap.

      if (splitFirst) {
        index = (*ptr & 0x0F);  // odd = bits 3 .. 0
        if (index != transp) {
          move = false; sx = px;
          lineBuf[np] = cmap[index];
          np++;
        }
        px++; ptr++;
        len--;
      }

      while (len--)
      {
        uint8_t color = *ptr;

        // find the actual color you care about.  There will be two pixels here!
        // but we may only want one at the end of the row
        uint16_t index = ((color & 0xF0) >> 4) & 0x0F;  // high bits are the even numbers
        if (index != transp) {
          if (move) {
            move = false; sx = px;
          }
          lineBuf[np] = cmap[index];
          np++; // added a pixel
        }
        else {
          move = true;
          if (np) {
            setWindow8(sx, y, sx + np - 1, y);
            pushPixels8(lineBuf, np);
            np = 0;
          }
        }
        px++;

        if (len--)
        {
          index = color & 0x0F; // the odd number is 3 .. 0
          if (index != transp) {
            if (move) {
              move = false; sx = px;
             }
            lineBuf[np] = cmap[index];
            np++;
          }
          else {
            move = true;
            if (np) {
              setWindow8(sx, y, sx + np - 1, y);
              pushPixels8(lineBuf, np);
              np = 0;
            }
          }
          px++;
        }
        else {
          break;  // we are done with this row.
        }
        ptr++;  // we only increment ptr once in the loop (deliberate)
      }

      if (np) {
        setWindow8(sx, y, sx + np - 1, y);
        writeData16(lineBuf, np);
        np = 0;
      }
      data += (w>>1);
      y++;
    }
  }
  else { // 1 bit per pixel

    uint32_t ww =  (w+7)>>3; // Width of source image line in bytes
    uint16_t np = 0;

    for (int32_t yp = y;  yp < y + h; yp++)
    {
      int32_t px = x, sx = x;
      bool move = true;
      for (int32_t xp = x; xp < x + w; xp++)
      {
        if (data[(xp>>3)] & (0x80 >> (xp & 0x7))) {
          if (move) {
            move = false;
            sx = px;
          }
          np++;
        }
        else {
          move = true;
          if (np) {
            setWindow8(sx, y, sx + np - 1, y);
            writeData16((uint16_t*)bitmap_fg, np);
            np = 0;
          }
        }
        px++;
      }
      if (np) { setWindow8(sx, y, sx + np - 1, y); writeData16((uint16_t*)bitmap_fg, np); np = 0; }
      y++;
      data += ww;
    }
  }
}

/***************************************************************************************
** Function name:           drawBitmap
** Description:             Draw bitmap from array with fixed color
***************************************************************************************/
void TFTLIB_8BIT::drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t *bitmap, uint16_t color) {
	int32_t i, j, byteWidth = (w + 7) / 8;

	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++ ) {
			if (read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
				drawPixel(x + i, y + j, color);
			}
		}
	}
}

/***************************************************************************************
** Function name:           drawBitmap
** Description:             Draw an image stored in an array on the TFT
***************************************************************************************/
void TFTLIB_8BIT::drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t *bitmap, uint16_t color) {
	int32_t i, j, byteWidth = (w + 7) / 8;

	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++ ) {
			if (read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
				drawPixel(x + i, y + j, color);
			}
		}
	}
}


/***************************************************************************************
** Function name:           drawBitmap
** Description:             Draw an image stored in an array on the TFT
***************************************************************************************/
void TFTLIB_8BIT::drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t *bitmap, uint16_t fgcolor, uint16_t bgcolor)
{
	int32_t i, j, byteWidth = (w + 7) / 8;

	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++ ) {
			if (read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7)))  drawPixel(x + i, y + j, fgcolor);
			else drawPixel(x + i, y + j, bgcolor);
		}
	}
}

/***************************************************************************************
** Function name:           drawXBitmap
** Description:             Draw an image stored in an XBM array onto the TFT
***************************************************************************************/
void TFTLIB_8BIT::drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color)
{
	int32_t i, j, byteWidth = (w + 7) / 8;

	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++ ) {
			if (read_byte(bitmap + j * byteWidth + i / 8) & (1 << (i & 7))) {
				drawPixel(x + i, y + j, color);
			}
		}
	}
}


/***************************************************************************************
** Function name:           drawXBitmap
** Description:             Draw an XBM image with foreground and background colors
***************************************************************************************/
void TFTLIB_8BIT::drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color, uint16_t bgcolor)
{
	int32_t i, j, byteWidth = (w + 7) / 8;

	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++ ) {
			if (read_byte(bitmap + j * byteWidth + i / 8) & (1 << (i & 7)))
				drawPixel(x + i, y + j,   color);
			else drawPixel(x + i, y + j, bgcolor);
		}
	}
}

/***************************************************************************************
** Function name:           decodeUTF8
** Description:             Serial UTF-8 decoder with fall-back to extended ASCII
*************************************************************************************x*/
uint16_t TFTLIB_8BIT::decodeUTF8(uint8_t c) {
	if (!_utf8) return c;

	// 7 bit Unicode Code Point
	if ((c & 0x80) == 0x00) {
		decoderState = 0;
		return c;
	}

	if (decoderState == 0) {
		// 11 bit Unicode Code Point
		if ((c & 0xE0) == 0xC0) {
			decoderBuffer = ((c & 0x1F)<<6);
			decoderState = 1;
			return 0;
		}
		// 16 bit Unicode Code Point
		if ((c & 0xF0) == 0xE0) {
			decoderBuffer = ((c & 0x0F)<<12);
			decoderState = 2;
			return 0;
		}
	}

	else {
		if (decoderState == 2) {
			decoderBuffer |= ((c & 0x3F)<<6);
			decoderState--;
			return 0;
		}

		else {
			decoderBuffer |= (c & 0x3F);
			decoderState = 0;
			return decoderBuffer;
		}
	}

	decoderState = 0;
	return c;
}


/***************************************************************************************
** Function name:           decodeUTF8
** Description:             Line buffer UTF-8 decoder with fall-back to extended ASCII
*************************************************************************************x*/
uint16_t TFTLIB_8BIT::decodeUTF8(uint8_t *buf, uint16_t *index, uint16_t remaining) {
	uint16_t c = buf[(*index)++];
	//Serial.print("Byte from string = 0x"); Serial.println(c, HEX);

	// 7 bit Unicode
	if ((c & 0x80) == 0x00) return c;

	// 11 bit Unicode
	if (((c & 0xE0) == 0xC0) && (remaining > 1))
	return ((c & 0x1F)<<6) | (buf[(*index)++]&0x3F);

	// 16 bit Unicode
	if (((c & 0xF0) == 0xE0) && (remaining > 2)) {
		c = ((c & 0x0F)<<12) | ((buf[(*index)++]&0x3F)<<6);
		return  c | ((buf[(*index)++]&0x3F));
	}

	return c; // fall-back to extended ASCII
}

int16_t TFTLIB_8BIT::textWidth(const char *string) {
	return textWidth(string, textfont);
}

int16_t TFTLIB_8BIT::textWidth(const char *string, uint8_t font) {
	int32_t str_width = 0;
	uint16_t uniCode  = 0;

	if(gfxFont) { // New font
		while (*string) {
			uniCode = decodeUTF8(*string++);
			if ((uniCode >= gfxFont->first) && (uniCode <= gfxFont->last)) {
				uniCode -= gfxFont->first;
				GFXglyph *glyph  = &gfxFont->glyph[uniCode];
				// If this is not the  last character or is a digit then use xAdvance
				if (*string  || isDigits) str_width += glyph->xAdvance;
				// Else use the offset plus width since this can be bigger than xAdvance
				else str_width += (glyph->xOffset + glyph->width);
			}
		}
	}
	else {
		while (*string++) str_width += 6;
	}
	isDigits = false;
	return str_width * textsize;
}

/***************************************************************************************
** Function name:           getTextDatum
** Description:             Return the text datum value (as used by setTextDatum())
***************************************************************************************/
uint8_t TFTLIB_8BIT::getTextDatum(void) {
	return textdatum;
}

/***************************************************************************************
** Function name:           getCursor
** Description:             Get the text cursor x & y position
***************************************************************************************/
void TFTLIB_8BIT::getCursor(int16_t* x, int16_t* y) {
	 *x = cursor_x;
	 *y = cursor_y;
}

/***************************************************************************************
** Function name:           setTextPadding
** Description:             Define padding width (aids erasing old text and numbers)
***************************************************************************************/
uint16_t TFTLIB_8BIT::getTextPadding(void)
{
  return padX;
}

/***************************************************************************************
** Function name:           setCursor
** Description:             Set the text cursor x,y position
***************************************************************************************/
void TFTLIB_8BIT::setCursor(int16_t x, int16_t y) {
	cursor_x = x;
	cursor_y = y + gfxFont->yAdvance;
}

/***************************************************************************************
** Function name:           setTextSize
** Description:             Set the text size multiplier
***************************************************************************************/
void TFTLIB_8BIT::setTextSize(uint8_t s) {
	if (s>7) s = 7; // Limit the maximum size multiplier so byte variables can be used for rendering
	textsize = (s > 0) ? s : 1; // Don't allow font size 0
}

/***************************************************************************************
** Function name:           setFreeFont
** Descriptions:            Sets the GFX free font to use
***************************************************************************************/

void TFTLIB_8BIT::setFreeFont(const GFXfont *f) {
	textfont = 1;
	gfxFont = (GFXfont *)f;

	glyph_ab = 0;
	glyph_bb = 0;
	uint16_t numChars = gfxFont->last - gfxFont->first;

	// Find the biggest above and below baseline offsets
	for (uint8_t c = 0; c < numChars; c++) {
		GFXglyph *glyph1  = &(gfxFont->glyph[c]);
		int8_t ab = -(glyph1->yOffset);
		if (ab > glyph_ab) glyph_ab = ab;
		int8_t bb = glyph1->height - ab;
		if (bb > glyph_bb) glyph_bb = bb;
	}
	setTextDatum(TL_DATUM);
	setCursor(0, 0);
}

/***************************************************************************************
** Function name:           setTextWrap
** Description:             Define if text should wrap at end of line
***************************************************************************************/
void TFTLIB_8BIT::setTextWrap(bool wrapX, bool wrapY) {
	textwrapX = wrapX;
	textwrapY = wrapY;
}

/***************************************************************************************
** Function name:           setTextColor
** Description:             Set the font foreground and background colour
***************************************************************************************/
void TFTLIB_8BIT::setTextColor(uint32_t c, uint32_t b) {
	textcolor   = c;
	textbgcolor = b;
}

/***************************************************************************************
** Function name:           setTextDatum
** Description:             Set the text position reference datum
***************************************************************************************/
void TFTLIB_8BIT::setTextDatum(uint8_t d) {
	textdatum = d;
}

/***************************************************************************************
** Function name:           setTextPadding
** Description:             Define padding width (aids erasing old text and numbers)
***************************************************************************************/
void TFTLIB_8BIT::setTextPadding(uint16_t x_width) {
	padX = x_width;
}

int16_t TFTLIB_8BIT::fontHeight(int16_t font) {
	return (gfxFont->yAdvance * textsize);
}

int16_t TFTLIB_8BIT::fontHeight(void) {
	return fontHeight(textfont);
}

size_t TFTLIB_8BIT::write(uint8_t utf8) {
	uint16_t uniCode = decodeUTF8(utf8);

	if (!uniCode) return 1;
	if (utf8 == '\r') return 1;

	if (uniCode == '\n') uniCode+=22; // Make it a valid space character to stop errors
	else if (uniCode < 32) return 1;

	uint16_t cheight = 0;

	cheight = cheight * textsize;

	if(utf8 == '\n') {
		cursor_x  = 0;
		cursor_y += (int16_t)textsize * (uint8_t)read_byte(&gfxFont->yAdvance);
	}

    else {
		if (uniCode > read_word(&gfxFont->last )) return 1;
		if (uniCode < read_word(&gfxFont->first)) return 1;

		uint16_t   c2    = uniCode - read_word(&gfxFont->first);
		GFXglyph *glyph = &(((GFXglyph *)read_dword(&gfxFont->glyph))[c2]);
		uint8_t   w     = read_byte(&glyph->width),

		h     = read_byte(&glyph->height);

		if((w > 0) && (h > 0)) { // Is there an associated bitmap?
			int16_t xo = (int8_t)read_byte(&glyph->xOffset);
			if(textwrapX && ((cursor_x + textsize * (xo + w)) > width())) {
				// Drawing character would go off right edge; wrap to new line
				cursor_x  = 0;
				cursor_y += (int16_t)textsize * (uint8_t)read_byte(&gfxFont->yAdvance);
			}

			if (textwrapY && (cursor_y >= (int32_t) height())) cursor_y = 0;
			drawChar(cursor_x, cursor_y, uniCode, textcolor, textbgcolor, textsize);
		}
		cursor_x += read_byte(&glyph->xAdvance) * (int16_t)textsize;
    }

	return 1;
}


/***************************************************************************************
** Function name:           drawChar
** Description:             draw a Unicode glyph onto the screen
***************************************************************************************/
// Any UTF-8 decoding must be done before calling drawChar()
int16_t TFTLIB_8BIT::drawChar(uint16_t uniCode, int32_t x, int32_t y) {
	if (!uniCode) return 0;

	drawChar(x, y, uniCode, textcolor, textbgcolor, textsize);
	if((uniCode >= gfxFont->first) && (uniCode <= gfxFont->last)) {
		uint16_t   c2    = uniCode - gfxFont->first;
		GFXglyph *glyph = &(gfxFont->glyph[c2]);
		return (glyph->xAdvance * textsize);
	}
	else return 0;

	if (((uniCode < 32) || (uniCode > 127))) return 0;

	int32_t width  = 0;
	int32_t height = 0;
	uint32_t flash_address = 0;
	uniCode -= 32;

	int32_t w = width;
	int32_t pY      = y;
	uint8_t line = 0;

	w *= height; // Now w is total number of pixels in the character
	if ((textsize != 1) || (textcolor == textbgcolor)) {
		if (textcolor != textbgcolor) fillRect(x, pY, width * textsize, textsize * height, textbgcolor);
		int32_t px = 0, py = pY; // To hold character block start and end column and row values
		int32_t pc = 0; // Pixel count
		uint8_t np = textsize * textsize; // Number of pixels in a drawn pixel

		uint8_t tnp = 0; // Temporary copy of np for while loop
		uint8_t ts = textsize - 1; // Temporary copy of textsize
		// 16 bit pixel count so maximum font size is equivalent to 180x180 pixels in area
		// w is total number of pixels to plot to fill character block
		while (pc < w) {
			line = read_byte((uint8_t *)flash_address);
			flash_address++;
			if (line & 0x80) {
				line &= 0x7F;
				line++;
				if (ts) {
					px = x + textsize * (pc % width); // Keep these px and py calculations outside the loop as they are slow
					py = y + textsize * (pc / width);
				}
				else {
					px = x + pc % width; // Keep these px and py calculations outside the loop as they are slow
					py = y + pc / width;
				}
				while (line--) { // In this case the while(line--) is faster
					pc++; // This is faster than putting pc+=line before while()?
					setWindow8(px, py, px + ts, py + ts);

					if (ts) {
						tnp = np;
						while (tnp--) {write16(textcolor);}
					}
					else write16(textcolor);

					px += textsize;

					if (px >= (x + width * textsize)) {
						px = x;
						py += textsize;
					}
				}
			}
			else {
				line++;
				pc += line;
			}
		}
	}
	else {
		// Text colour != background && textsize = 1 and character is within screen area
		// so use faster drawing of characters and background using block write
		if ((x >= 0) && (x + width <= _width) && (y >= 0) && (y + height <= _height)) {
			setWindow8(x, y, x + width - 1, y + height - 1);

			// Maximum font size is equivalent to 180x180 pixels in area
			while (w > 0) {
				line = read_byte((uint8_t *)flash_address++); // 8 bytes smaller when incrementing here
				if (line & 0x80) {
					line &= 0x7F;
					line++; w -= line;
					pushBlock16(textcolor,line);
				}
				else {
					line++; w -= line;
					pushBlock16(textbgcolor,line);
				}
			}
		}
		else {
			int32_t px = x, py = y;  // To hold character block start and end column and row values
			int32_t pc = 0;          // Pixel count
			int32_t pl = 0;          // Pixel line length
			uint16_t pcol = 0;       // Pixel color

			while (pc < w) {
				line = read_byte((uint8_t *)flash_address);
				flash_address++;
				if (line & 0x80) { pcol = textcolor; line &= 0x7F; }
				else pcol = textbgcolor;
				line++;
				px = x + pc % width;
				py = y + pc / width;

				pl = 0;
				pc += line;
				while (line--) { // In this case the while(line--) is faster
					pl++;
					if ((px+pl) >= (x + width)) {
						drawFastHLine(px, py, pl, pcol);
						pl = 0;
						px = x;
						py ++;
					}
				}
				if (pl)drawFastHLine(px, py, pl, pcol);
			}
		}
	}
	// End of RLE font rendering
	return width * textsize;    // x +
}

void TFTLIB_8BIT::drawChar(int32_t x, int32_t y, uint16_t c, uint32_t color, uint32_t bg, uint8_t size) {
	if ((x >= _width)			|| // Clip right
	(y >= _height)				|| // Clip bottom
	((x + 6 * size - 1) < 0)	|| // Clip left
	((y + 8 * size - 1) < 0))   // Clip top
	return;

	if (c < 32) return;
	if ((c >= read_word(&gfxFont->first)) && (c <= read_word(&gfxFont->last ))) {
		c -= read_word(&gfxFont->first);
		GFXglyph *glyph  = &(((GFXglyph *)read_dword(&gfxFont->glyph))[c]);
		uint8_t  *bitmap = (uint8_t *)read_dword(&gfxFont->bitmap);

		uint32_t bo = read_word(&glyph->bitmapOffset);
		uint8_t  w  = read_byte(&glyph->width),
		h  = read_byte(&glyph->height);

		int8_t   xo = read_byte(&glyph->xOffset),
		yo = read_byte(&glyph->yOffset);
		uint8_t  xx, yy, bits=0, bit=0;
		int16_t  xo16 = 0, yo16 = 0;

		if(bg != color && bg != 0xFFFFFFFF) fillRect(x+xo, y+yo, w, h, bg);

		if(size > 1) {
			xo16 = xo;
			yo16 = yo;
		}

		// GFXFF rendering speed up
		uint16_t hpc = 0; // Horizontal foreground pixel count
		for(yy=0; yy<h; yy++) {
			for(xx=0; xx<w; xx++) {
				if(bit == 0) {
					bits = read_byte(&bitmap[bo++]);
					bit  = 0x80;
				}

				if(bits & bit) hpc++;
				else {
					if (hpc) {
						if(size == 1) drawFastHLine(x+xo+xx-hpc, y+yo+yy, hpc, color);
						else fillRect(x+(xo16+xx-hpc)*size, y+(yo16+yy)*size, size*hpc, size, color);
						hpc=0;
					}
				}
				bit >>= 1;
			}

			// Draw pixels for this line as we are about to increment yy
			if (hpc) {
				if(size == 1) drawFastHLine(x+xo+xx-hpc, y+yo+yy, hpc, color);
				else fillRect(x+(xo16+xx-hpc)*size, y+(yo16+yy)*size, size*hpc, size, color);
				hpc=0;
			}
		}
	}
}


/***************************************************************************************
** Function name:           drawString (with or without user defined font)
** Description :            draw string with padding if it is defined
***************************************************************************************/
int16_t TFTLIB_8BIT::drawString(const String& string, int32_t poX, int32_t poY)
{
  int16_t len = string.length() + 2;
  char buffer[len];
  string.toCharArray(buffer, len);
  return drawString(buffer, poX, poY);
}

int16_t TFTLIB_8BIT::drawString(const char *string, int32_t poX, int32_t poY) {
	int16_t sumX = 0;
	uint8_t padding = 1, baseline = 0;
	uint16_t cwidth = textWidth(string, textfont); // Find the pixel width of the string in the font
	uint16_t cheight = 8 * textsize;

	cheight = glyph_ab * textsize;
	poY += cheight; // Adjust for baseline datum of free fonts
	baseline = cheight;
	padding =101; // Different padding method used for Free Fonts

	// We need to make an adjustment for the bottom of the string (eg 'y' character)
	if ((textdatum == BL_DATUM) || (textdatum == BC_DATUM) || (textdatum == BR_DATUM)) {
		cheight += glyph_bb * textsize;
	}

    baseline = gfxFont->yAdvance;
    cheight  = fontHeight();

	if (textdatum || padX) {
		switch(textdatum) {
			case TC_DATUM:
				poX -= cwidth/2;
				padding += 1;
			break;

			case TR_DATUM:
				poX -= cwidth;
				padding += 2;
			break;

			case ML_DATUM:
				poY -= cheight/2;
			break;

			case MC_DATUM:
				poX -= cwidth/2;
				poY -= cheight/2;
				padding += 1;
			break;

			case MR_DATUM:
				poX -= cwidth;
				poY -= cheight/2;
				padding += 2;
			break;

			case BL_DATUM:
				poY -= cheight;
			break;

			case BC_DATUM:
				poX -= cwidth/2;
				poY -= cheight;
				padding += 1;
			break;

			case BR_DATUM:
				poX -= cwidth;
				poY -= cheight;
				padding += 2;
			break;

			case L_BASELINE:
				poY -= baseline;
			break;

			case C_BASELINE:
				poX -= cwidth/2;
				poY -= baseline;
				padding += 1;
			break;

			case R_BASELINE:
				poX -= cwidth;
				poY -= baseline;
				padding += 2;
			break;
		}
		// Check coordinates are OK, adjust if not
		if (poX < 0) poX = 0;
		if (poX+cwidth > width())   poX = width() - cwidth;
		if (poY < 0) poY = 0;
		if (poY+cheight-baseline> height()) poY = height() - cheight;
	}


	int8_t xo = 0;

	if (textcolor != textbgcolor) {
		cheight = (glyph_ab + glyph_bb) * textsize;
		// Get the offset for the first character only to allow for negative offsets
		uint16_t c2 = 0;
		uint16_t len = strlen(string);
		uint16_t n = 0;

		while (n < len && c2 == 0) c2 = decodeUTF8((uint8_t*)string, &n, len - n);

		if((c2 >= gfxFont->first) && (c2 <= gfxFont->last)) {
			c2 -= gfxFont->first;
			GFXglyph *glyph = &(gfxFont->glyph[c2]);
			xo = (glyph->xOffset * textsize);
			// Adjust for negative xOffset
			if (xo > 0) xo = 0;
			else cwidth -= xo;
			// Add 1 pixel of padding all round
			//cheight +=2;
			//fillRect(poX+xo-1, poY - 1 - glyph_ab * textsize, cwidth+2, cheight, textbgcolor);
			fillRect(poX+xo, poY - glyph_ab * textsize, cwidth, cheight, textbgcolor);
		}
		padding -=100;
	}

	uint16_t len = strlen(string);
	uint16_t n = 0;

	while (n < len) {
		uint16_t uniCode = decodeUTF8((uint8_t*)string, &n, len - n);
		sumX += drawChar(uniCode, poX+sumX, poY);
	}

	if((padX>cwidth) && (textcolor!=textbgcolor)) {
		int16_t padXc = poX+cwidth+xo;
		poX +=xo; // Adjust for negative offset start character
		poY -= glyph_ab * textsize;
		sumX += poX;

		switch(padding) {
			case 1:
				fillRect(padXc,poY,padX-cwidth,cheight, textbgcolor);
			break;

			case 2:
				fillRect(padXc,poY,(padX-cwidth)>>1,cheight, textbgcolor);
				padXc = (padX-cwidth)>>1;
				if (padXc>poX) padXc = poX;
				fillRect(poX - padXc,poY,(padX-cwidth)>>1,cheight, textbgcolor);
			break;

			case 3:
				if (padXc>padX) padXc = padX;
				fillRect(poX + cwidth - padXc,poY,padXc-cwidth,cheight, textbgcolor);
			break;
		}
	}
	return sumX;
}


/***************************************************************************************
** Function name:           drawCentreString (deprecated, use setTextDatum())
** Descriptions:            draw string centred on dX
***************************************************************************************/
int16_t TFTLIB_8BIT::drawCentreString(const String& string, int32_t dX, int32_t poY) {
	int16_t len = string.length() + 2;
	char buffer[len];
	string.toCharArray(buffer, len);
	return drawCentreString(buffer, dX, poY);
}

int16_t TFTLIB_8BIT::drawCentreString(const char *string, int32_t dX, int32_t poY) {
	uint8_t tempdatum = textdatum;
	int32_t sumX = 0;
	textdatum = MC_DATUM;
	sumX = drawString(string, dX, poY);
	textdatum = tempdatum;
	return sumX;
}


/***************************************************************************************
** Function name:           drawRightString
** Descriptions:            draw string right justified to dX
***************************************************************************************/
int16_t TFTLIB_8BIT::drawRightString(const String& string, int32_t dX, int32_t poY) {
	int16_t len = string.length() + 2;
	char buffer[len];
	string.toCharArray(buffer, len);
	return drawRightString(buffer, dX, poY);
}

int16_t TFTLIB_8BIT::drawRightString(const char *string, int32_t dX, int32_t poY) {
	uint8_t tempdatum = textdatum;
	int16_t sumX = 0;
	textdatum = TR_DATUM;
	sumX = drawString(string, dX, poY);
	textdatum = tempdatum;
	return sumX;
}


/***************************************************************************************
** Function name:           drawNumber
** Description:             draw a long integer
***************************************************************************************/
int16_t TFTLIB_8BIT::drawNumber(long long_num, int32_t poX, int32_t poY) {
	isDigits = true; // Eliminate jiggle in monospaced fonts
	char str[12];
	itoa(long_num, str, 10);
	return drawString(str, poX, poY);
}

/***************************************************************************************
** Function name:           drawFloat
** Descriptions:            drawFloat, prints 7 non zero digits maximum
***************************************************************************************/
// Assemble and print a string, this permits alignment relative to a datum
// looks complicated but much more compact and actually faster than using print class
int16_t TFTLIB_8BIT::drawFloat(float floatNumber, uint8_t dp, int32_t poX, int32_t poY) {
	isDigits = true;
	char str[14];               // Array to contain decimal string
	uint8_t ptr = 0;            // Initialise pointer for array
	int8_t  digits = 1;         // Count the digits to avoid array overflow
	float rounding = 0.5;       // Round up down delta

	if (dp > 7) dp = 7; // Limit the size of decimal portion

	// Adjust the rounding value
	for (uint8_t i = 0; i < dp; ++i) rounding /= 10.0;

	if (floatNumber < -rounding) {   // add sign, avoid adding - sign to 0.0!
		str[ptr++] = '-'; // Negative number
		str[ptr] = 0; // Put a null in the array as a precaution
		digits = 0;   // Set digits to 0 to compensate so pointer value can be used later
		floatNumber = -floatNumber; // Make positive
	}

	floatNumber += rounding; // Round up or down

	if (floatNumber >= 2147483647) {
		strcpy(str, "...");
		return drawString(str, poX, poY);
	}
	// No chance of overflow from here on

	// Get integer part
	uint32_t temp = (uint32_t)floatNumber;

	// Put integer part into array
	itoa(temp, str + ptr, 10);

	// Find out where the null is to get the digit count loaded
	while ((uint8_t)str[ptr] != 0) ptr++; // Move the pointer along
	digits += ptr;                  // Count the digits

	str[ptr++] = '.'; // Add decimal point
	str[ptr] = '0';   // Add a dummy zero
	str[ptr + 1] = 0; // Add a null but don't increment pointer so it can be overwritten

	// Get the decimal portion
	floatNumber = floatNumber - temp;

	// Get decimal digits one by one and put in array
	// Limit digit count so we don't get a false sense of resolution
	uint8_t i = 0;
	while ((i < dp) && (digits < 9)) { // while (i < dp) for no limit but array size must be increased
		i++;
		floatNumber *= 10;       // for the next decimal
		temp = floatNumber;      // get the decimal
		itoa(temp, str + ptr, 10);
		ptr++; digits++;         // Increment pointer and digits count
		floatNumber -= temp;     // Remove that digit
	}

	// Finally we can plot the string and return pixel length
	return drawString(str, poX, poY);
}

/***************************************************************************************************************************
** 												Test functions for display benchmark
****************************************************************************************************************************/
uint32_t TFTLIB_8BIT::testFillScreen() {
	unsigned long start = time_us_32();
	fillScreen(BLACK);
	fillScreen(RED);
	fillScreen(GREEN);
	fillScreen(BLUE);
	fillScreen(BLACK);
	return (time_us_32() - start) / 1000;
}

uint32_t TFTLIB_8BIT::testText() {
	fillScreen(BLACK);
	setFreeFont(&FreeSerif9pt7b);
	unsigned long start = time_us_32();
	setTextColor(WHITE);
	setTextSize(1);
	setTextDatum(TL_DATUM);
	drawString("Hello World!", 0, 0);

	setTextColor(YELLOW);
	setTextSize(2);
	drawFloat(1234.56, 2, 0, 16);

	setCursor(0, 48);
	setTextColor(RED);
	setTextSize(3);
	println(0xDEADBEEF, HEX);

	setCursor(0, 96);
	setTextColor(GREEN);
	setTextSize(5);
	println("Groop");

	setCursor(0, 176);
	setTextSize(2);
	println("I implore thee,");

	setCursor(0, 192);
	setTextSize(1);
	println("my foonting turlingdromes.");
	println("And hooptiously drangle me");
	println("with crinkly bindlewurdles,");
	println("Or I will rend thee");
	println("in the gobberwarts");
	println("with my blurglecruncheon,");
	println("see if I don't!");
	return (time_us_32() - start) / 1000;
}

uint32_t TFTLIB_8BIT::testLines(uint16_t color) {
	unsigned long	start, t;
	int				x1, y1, x2, y2,
					w = _width,
					h = _height;

	fillScreen(BLACK);

	x1 = y1 = 0;
	y2    = h - 1;
	start = time_us_32();
	for (x2 = 0; x2 < w; x2 += 6) drawLine(x1, y1, x2, y2, color);
	x2    = w - 1;
	for (y2 = 0; y2 < h; y2 += 6) drawLine(x1, y1, x2, y2, color);
	t     = (time_us_32() - start) / 1000; // fillScreen doesn't count against timing

	fillScreen(BLACK);

	x1    = w - 1;
	y1    = 0;
	y2    = h - 1;
	start = time_us_32();
	for (x2 = 0; x2 < w; x2 += 6) drawLine(x1, y1, x2, y2, color);
	x2    = 0;
	for (y2 = 0; y2 < h; y2 += 6) drawLine(x1, y1, x2, y2, color);
	t    += (time_us_32() - start) / 1000;

	fillScreen(BLACK);

	x1    = 0;
	y1    = h - 1;
	y2    = 0;
	start = time_us_32();
	for (x2 = 0; x2 < w; x2 += 6) drawLine(x1, y1, x2, y2, color);
	x2    = w - 1;
	for (y2 = 0; y2 < h; y2 += 6) drawLine(x1, y1, x2, y2, color);
	t    += (time_us_32() - start) / 1000;

	fillScreen(BLACK);

	x1    = w - 1;
	y1    = h - 1;
	y2    = 0;
	start = time_us_32();
	for (x2 = 0; x2 < w; x2 += 6) drawLine(x1, y1, x2, y2, color);
	x2    = 0;
	for (y2 = 0; y2 < h; y2 += 6) drawLine(x1, y1, x2, y2, color);

	return (time_us_32() - start) / 1000;
}

uint32_t TFTLIB_8BIT::testFastLines(uint16_t color1, uint16_t color2) {
	unsigned long start;
	int           x = 0, y = 0, w = _width, h = _height;

	fillScreen(BLACK);
	start = time_us_32();
	for (y = 0; y < h; y += 5) drawFastHLine(0, y, w, color1);
	for (x = 0; x < w; x += 5) drawFastVLine(x, 0, h, color2);

	return (time_us_32() - start) / 1000;
}

uint32_t TFTLIB_8BIT::testRects(uint16_t color) {
	unsigned long	start;
	int				n, i, i2,
					cx = _width  / 2,
					cy = _height / 2;

	fillScreen(BLACK);
	n     = min(_width, _height);
	start = time_us_32();
	for (i = 2; i < n; i += 6) {
		i2 = i / 2;
		drawRect(cx - i2, cy - i2, i, i, color);
	}

	return (time_us_32() - start) / 1000;
}

uint32_t TFTLIB_8BIT::testFilledRects(uint16_t color1, uint16_t color2) {
	unsigned long	start, t = 0;
	int				n, i, i2,
					cx = _width  / 2 - 1,
					cy = _height / 2 - 1;

	fillScreen(BLACK);
	n = min(_width, _height);
	for (i = n - 1; i > 0; i -= 6) {
		i2    = i / 2;
		start = time_us_32();
		fillRect(cx - i2, cy - i2, i, i, color1);
		t    += (time_us_32() - start) / 1000;
		// Outlines are not included in timing results
		drawRect(cx - i2, cy - i2, i, i, color2);
	}
	return t;
}

uint32_t TFTLIB_8BIT::testFilledCircles(uint8_t radius, uint16_t color) {
	unsigned long	start;
	int				x, y, w = _width, h = _height, r2 = radius * 2;

	fillScreen(BLACK);
	start = time_us_32();
	for (x = radius; x < w; x += r2) {
		for (y = radius; y < h; y += r2) {
			fillCircle(x, y, radius, color);
		}
	}
	return (time_us_32() - start) / 1000;
}

uint32_t TFTLIB_8BIT::testCircles(uint8_t radius, uint16_t color) {
	unsigned long	start;
	int				x, y, r2 = radius * 2,
					w = _width  + radius,
					h = _height + radius;

	// Screen is not cleared for this one -- this is
	// intentional and does not affect the reported time.
	start = time_us_32();
	for (x = 0; x < w; x += r2) {
		for (y = 0; y < h; y += r2) {
			drawCircle(x, y, radius, color);
		}
	}

	return (time_us_32() - start) / 1000;
}

uint32_t TFTLIB_8BIT::testTriangles() {
	unsigned long	start;
	int				n, i, cx = _width  / 2 - 1,
					cy = _height / 2 - 1;

	fillScreen(BLACK);
	n     = min(cx, cy);
	start = time_us_32();

	for (i = 0; i < n; i += 5) {
		drawTriangle(
		cx    , cy - i, // peak
		cx - i, cy + i, // bottom left
		cx + i, cy + i, // bottom right
		color565(0, 0, i));
	}

	return (time_us_32() - start) / 1000;
}

uint32_t TFTLIB_8BIT::testFilledTriangles() {
	unsigned long	start, t = 0;
	int				i, cx = _width  / 2 - 1,
					cy = _height / 2 - 1;

	fillScreen(BLACK);
	start = time_us_32();
	for (i = min(cx, cy); i > 10; i -= 5) {
		start = time_us_32();
		fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i, color565(0, i, i));
		t += (time_us_32() - start) / 1000;
		drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i, color565(i, i, 0));
	}

	return t;
}

uint32_t TFTLIB_8BIT::testRoundRects() {
	unsigned long	start;
	int				w, i, i2,
					cx = _width  / 2 - 1,
					cy = _height / 2 - 1;

	fillScreen(BLACK);
	w     = min(_height, _width);
	start = time_us_32();
	for (i = 0; i < w; i += 6) {
		i2 = i / 2;
		drawRoundRect(cx - i2, cy - i2, i, i, i / 8, color565(i, 0, 0));
	}

	return (time_us_32() - start) / 1000;
}

uint32_t TFTLIB_8BIT::testFilledRoundRects() {
	unsigned long	start;
	int				i, i2,
					cx = _width  / 2 - 1,
					cy = _height / 2 - 1;

	fillScreen(BLACK);
	start = time_us_32();
	for (i = min(_width , _height); i > 20; i -= 6) {
		i2 = i / 2;
		fillRoundRect(cx - i2, cy - i2, i, i, i / 8, color565(0, i, 0));
	}

	return (time_us_32() - start) / 1000;
}

void TFTLIB_8BIT::benchmark(void){
	char buffer[32];
	uint32_t t1 = testFillScreen();
	uint32_t t2 = testText();

	uint32_t t3 = testLines(CYAN);
	uint32_t t4 = testFastLines(RED, BLUE);

	uint32_t t5 = testRects(GREEN);
	uint32_t t6 = testFilledRects(YELLOW, MAGENTA);

	uint32_t t7 = testCircles(10, WHITE);
	uint32_t t8 = testFilledCircles(10, MAGENTA);

	uint32_t t9 = testTriangles();
	uint32_t t10 = testFilledTriangles();

	uint32_t t11 = testRoundRects();
	uint32_t t12 = testFilledRoundRects();

	uint32_t total = t1 + t2 + t3 + t4 + t5 + t6 + t7 + t8 + t9 + t10 + t11 + t12;

	fillScreen(BLACK);

	setFreeFont(&FreeSerif9pt7b);
	setTextColor(RED, BLACK);
	setTextDatum(TL_DATUM);

	sprintf(buffer, "Fillscreen:         %lums", t1);
	drawString(buffer, 0, 0);

	sprintf(buffer, "Text:               %lums", t2);
	drawString(buffer, 0, 16);

	sprintf(buffer, "Lines:              %lums", t3);
	drawString(buffer, 0, 32);

	sprintf(buffer, "Hor/Vert Lines:     %lums", t4);
	drawString(buffer, 0, 48);

	sprintf(buffer, "Rect(outline):      %lums", t5);
	drawString(buffer, 0, 64);

	sprintf(buffer, "Rect(filled):       %lums", t6);
	drawString(buffer, 0, 80);

	sprintf(buffer, "Circ(outline):      %lums", t7);
	drawString(buffer, 0, 96);

	sprintf(buffer, "Circ(filled):       %lums", t8);
	drawString(buffer, 0, 112);

	sprintf(buffer, "Tri(outline):       %lums", t9);
	drawString(buffer, 0, 128);

	sprintf(buffer, "Tri(filled):        %lums", t10);
	drawString(buffer, 0, 144);

	sprintf(buffer, "Round Rects:        %lums", t11);
	drawString(buffer, 0, 160);

	sprintf(buffer, "Filled Round Rects: %lums", t12);
	drawString(buffer, 0, 176);

	sprintf(buffer, "Time total:         %lums", total);
	drawString(buffer, 0, 192);

	sleep_ms(4000);
}

ButtonWidget::ButtonWidget(TFTLIB_8BIT *disp) {
  _tft       = disp;
  _xd        = 0;
  _yd        = 0;
  _textdatum = MC_DATUM;
  _label[9]  = '\0';
  _currstate  = false;
  _laststate  = false;
  _inverted  = false;
}

void ButtonWidget::setPressAction(actionCallback action)
{
  pressAction = action;
}

void ButtonWidget::setReleaseAction(actionCallback action)
{
  releaseAction = action;
}

// Classic initButton() function: pass center & size
void ButtonWidget::initButton(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t textcolor, char *label, uint8_t textsize)
{
  // Tweak arguments and pass to the newer initButtonUL() function...
  initButtonUL(x - (w / 2), y - (h / 2), w, h, outline, fill, textcolor, label, textsize);
}

// Newer function instead accepts upper-left corner & size
void ButtonWidget::initButtonUL(int16_t x1, int16_t y1, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t textcolor, char *label, uint8_t textsize)
{
  _x1           = x1;
  _y1           = y1;
  _w            = w;
  _h            = h;
  _outlinecolor = outline;
  _outlinewidth = 2;
  _fillcolor    = fill;
  _textcolor    = textcolor;
  _textsize     = textsize;
  strncpy(_label, label, 9);
  _pressTime    = 0xFFFFFFFF;
  _releaseTime  = 0xFFFFFFFF;
}

// Adjust text datum and x, y deltas
void ButtonWidget::setLabelDatum(int16_t x_delta, int16_t y_delta, uint8_t datum)
{
  _xd        = x_delta;
  _yd        = y_delta;
  _textdatum = datum;
}

void ButtonWidget::drawButton(bool inverted, String long_name) {
  uint16_t fill, outline, text;

  _inverted  = inverted;

  if(!inverted) {
    fill    = _fillcolor;
    outline = _outlinecolor;
    text    = _textcolor;
  } else {
    fill    = _textcolor;
    outline = _outlinecolor;
    text    = _fillcolor;
  }

  uint8_t r = min(_w, _h) / 4; // Corner radius
  _tft->fillRoundRect(_x1, _y1, _w, _h, r, fill);
  _tft->drawRoundRect(_x1, _y1, _w, _h, r, outline);

  if (_tft->textfont == 255) {
    _tft->setCursor(_x1 + (_w / 8),
                    _y1 + (_h / 4));
    _tft->setTextColor(text);
    _tft->setTextSize(_textsize);
    _tft->print(_label);
  }
  else {
    _tft->setTextColor(text, fill);
    _tft->setTextSize(_textsize);

    uint8_t tempdatum = _tft->getTextDatum();
    _tft->setTextDatum(_textdatum);
    uint16_t tempPadding = _tft->getTextPadding();
    _tft->setTextPadding(0);

    if (long_name == "")
      _tft->drawString(_label, _x1 + (_w/2) + _xd, _y1 + (_h/2) - 4 + _yd);
    else
      _tft->drawString(long_name, _x1 + (_w/2) + _xd, _y1 + (_h/2) - 4 + _yd);

    _tft->setTextDatum(tempdatum);
    _tft->setTextPadding(tempPadding);
  }
}

void ButtonWidget::drawSmoothButton(bool inverted, int16_t outlinewidth, uint32_t bgcolor, String long_name) {
  uint16_t fill, outline, text;
  if (bgcolor != 0x00FFFFFF) _bgcolor = bgcolor;
  if (outlinewidth >=0) _outlinewidth = outlinewidth;
  _inverted  = inverted;

  if(!inverted) {
    fill    = _fillcolor;
    outline = _outlinecolor;
    text    = _textcolor;
  } else {
    fill    = _textcolor;
    outline = _outlinecolor;
    text    = _fillcolor;
  }

  uint8_t r = min(_w, _h) / 4; // Corner radius
  if (outlinewidth > 0) _tft->fillSmoothRoundRect(_x1, _y1, _w, _h, r, outline, _bgcolor);
  _tft->fillSmoothRoundRect(_x1+_outlinewidth, _y1+_outlinewidth, _w-(2*_outlinewidth), _h-(2*_outlinewidth), r-_outlinewidth, fill, outline);

  if (_tft->textfont == 255) {
    _tft->setCursor(_x1 + (_w / 8),
                    _y1 + (_h / 4));
    _tft->setTextColor(text);
    _tft->setTextSize(_textsize);
    _tft->print(_label);
  }
  else {
    _tft->setTextColor(text, fill);
    _tft->setTextSize(_textsize);

    uint8_t tempdatum = _tft->getTextDatum();
    _tft->setTextDatum(_textdatum);
    uint16_t tempPadding = _tft->getTextPadding();
    _tft->setTextPadding(0);

    if (long_name == "")
      _tft->drawString(_label, _x1 + (_w/2) + _xd, _y1 + (_h/2) - 4 + _yd);
    else
      _tft->drawString(long_name, _x1 + (_w/2) + _xd, _y1 + (_h/2) - 4 + _yd);

    _tft->setTextDatum(tempdatum);
    _tft->setTextPadding(tempPadding);
  }
}

bool ButtonWidget::contains(int16_t x, int16_t y) {
  return ((x >= _x1) && (x < (_x1 + _w)) &&
          (y >= _y1) && (y < (_y1 + _h)));
}

void ButtonWidget::press(bool p) {
  _laststate = _currstate;
  _currstate = p;
}

bool ButtonWidget::isPressed()    { return _currstate; }
bool ButtonWidget::justPressed()  { return (_currstate && !_laststate); }
bool ButtonWidget::justReleased() { return (!_currstate && _laststate); }

TFTLIB_Sprites::TFTLIB_Sprites(TFTLIB_8BIT* displ) {
	_lcd = displ;     // Pointer to tft class so we can call member functions

	_iwidth    = 0; // Initialise width and height to 0 (it does not exist yet)
	_iheight   = 0;
	_bpp = 16;

	_created = false;
	_vpOoB   = true;

	_xs = 0;  // window bounds for pushColor
	_ys = 0;
	_xe = 0;
	_ye = 0;

	_xptr = 0; // pushColor coordinate
	_yptr = 0;

	_colorMap = nullptr;
}


/***************************************************************************************
** Function name:           createSprite
** Description:             Create a sprite (bitmap) of defined width and height
***************************************************************************************/
// cast returned value to (uint8_t*) for 8 bit or (uint16_t*) for 16 bit colours
void* TFTLIB_Sprites::createSprite(int16_t w, int16_t h, uint8_t frames) {
	if ( _created ) return _img8_1;

	if ( w < 1 || h < 1 ) return nullptr;

	_iwidth  = _dwidth  = _bitwidth = w;
	_iheight = _dheight = h;

	_sx = 0;
	_sy = 0;
	_sw = w;
	_sh = h;
	_scolor = BLACK;

	_img8   = (uint8_t*) callocSprite(w, h, frames);
	_img8_1 = _img8;
	_img8_2 = _img8;
	_img    = (uint16_t*) _img8;
	_img4   = _img8;

	if ( (_bpp == 16) && (frames > 1) ) {
		_img8_2 = _img8 + (w * h * 2 + 1);
	}

	if ( (_bpp == 8) && (frames > 1) ) {
		_img8_2 = _img8 + (w * h + 1);
	}

	if ((_bpp == 4) && (_colorMap == nullptr)) createPalette(default_4bit_palette);

	if ((_bpp == 1) && (frames > 1)) {
		w = (w+7) & 0xFFF8;
		_img8_2 = _img8 + ( (w>>3) * h + 1 );
	}

	if (_img8) {
		_created = true;
		rotation = 0;
		setViewport(0, 0, _dwidth, _dheight);
		setPivot(_iwidth/2, _iheight/2);
		return _img8_1;
	}

	return nullptr;
}


/***************************************************************************************
** Function name:           getPointer
** Description:             Returns pointer to start of sprite memory area
***************************************************************************************/
void* TFTLIB_Sprites::getPointer(void) {
	if (!_created) return nullptr;
	return _img8_1;
}


/***************************************************************************************
** Function name:           created
** Description:             Returns true if sprite has been created
***************************************************************************************/
bool TFTLIB_Sprites::created(void) {
	return _created;
}


/***************************************************************************************
** Function name:           ~TFTLIB_Sprites
** Description:             Class destructor
***************************************************************************************/
TFTLIB_Sprites::~TFTLIB_Sprites(void) {
	deleteSprite();
}


/***************************************************************************************
** Function name:           callocSprite
** Description:             Allocate a memory area for the Sprite and return pointer
***************************************************************************************/
void* TFTLIB_Sprites::callocSprite(int16_t w, int16_t h, uint8_t frames) {
	// Add one extra "off screen" pixel to point out-of-bounds setWindow() coordinates
	// this means push/writeColor functions do not need additional bounds checks
	uint8_t* ptr8 = nullptr;

	if (frames > 2) frames = 2; // Currently restricted to 2 frame buffers
	if (frames < 1) frames = 1;

	if (_bpp == 16) {
		ptr8 = ( uint8_t*) calloc(frames * w * h + frames, sizeof(uint16_t));
	}

	else if (_bpp == 8) {
		ptr8 = ( uint8_t*) calloc(frames * w * h + frames, sizeof(uint8_t));
	}

	else if (_bpp == 4) {
		w = (w+1) & 0xFFFE; // width needs to be multiple of 2, with an extra "off screen" pixel
		_iwidth = w;
		ptr8 = ( uint8_t*) calloc(((frames * w * h) >> 1) + frames, sizeof(uint8_t));
	}

	else {
		w =  (w+7) & 0xFFF8;
		_iwidth = w;         // _iwidth is rounded up to be multiple of 8, so might not be = _dwidth
		_bitwidth = w;       // _bitwidth will not be rotated whereas _iwidth may be

		ptr8 = ( uint8_t*) calloc(frames * (w>>3) * h + frames, sizeof(uint8_t));
	}

	return ptr8;
}


/***************************************************************************************
** Function name:           createPalette (from RAM array)
** Description:             Set a palette for a 4-bit per pixel sprite
***************************************************************************************/
void TFTLIB_Sprites::createPalette(uint16_t colorMap[], uint8_t colors) {
	if (_colorMap != nullptr) {
		free(_colorMap);
	}

	if (colorMap == nullptr) {
		// Create a color map using the default FLASH map
		createPalette(default_4bit_palette);
		return;
	}

	// Allocate and clear memory for 16 color map
	_colorMap = (uint16_t *)calloc(16, sizeof(uint16_t));

	if (colors > 16) colors = 16;

	// Copy map colors
	for (uint8_t i = 0; i < colors; i++) {
		_colorMap[i] = colorMap[i];
	}
}


/***************************************************************************************
** Function name:           createPalette (from FLASH array)
** Description:             Set a palette for a 4-bit per pixel sprite
***************************************************************************************/
void TFTLIB_Sprites::createPalette(const uint16_t colorMap[], uint8_t colors) {
	if (colorMap == nullptr) {
		// Create a color map using the default FLASH map
		colorMap = default_4bit_palette;
	}

	// Allocate and clear memory for 16 color map
	_colorMap = (uint16_t *)calloc(16, sizeof(uint16_t));

	if (colors > 16) colors = 16;

	// Copy map colors
	for (uint8_t i = 0; i < colors; i++) {
		_colorMap[i] = read_word(colorMap++);
	}
}


/***************************************************************************************
** Function name:           frameBuffer
** Description:             For 1 bpp Sprites, select the frame used for graphics
***************************************************************************************/
// Frames are numbered 1 and 2
void* TFTLIB_Sprites::frameBuffer(int8_t f) {
	if (!_created) return nullptr;

	if ( f == 2 ) _img8 = _img8_2;
	else          _img8 = _img8_1;

	if (_bpp == 16) _img = (uint16_t*)_img8;

	if (_bpp == 4) _img4 = _img8;

	return _img8;
}


/***************************************************************************************
** Function name:           setColorDepth
** Description:             Set bits per pixel for colour (1, 8 or 16)
***************************************************************************************/
void* TFTLIB_Sprites::setColorDepth(int8_t b) {
	// Do not re-create the sprite if the colour depth does not change
	if (_bpp == b) return _img8_1;

	// Validate the new colour depth
	if ( b > 8 ) _bpp = 16;  // Bytes per pixel
	else if ( b > 4 ) _bpp = 8;
	else if ( b > 1 ) _bpp = 4;
	else _bpp = 1;

	// Can't change an existing sprite's colour depth so delete it
	if (_created) free(_img8_1);

	// If it existed, re-create the sprite with the new colour depth
	if (_created) {
		_created = false;
		return createSprite(_dwidth, _dheight);
	}

	return nullptr;
}


/***************************************************************************************
** Function name:           getColorDepth
** Description:             Get bits per pixel for colour (1, 8 or 16)
***************************************************************************************/
int8_t TFTLIB_Sprites::getColorDepth(void) {
	if (_created) return _bpp;
	else return 0;
}


/***************************************************************************************
** Function name:           setBitmapColor
** Description:             Set the 1bpp foreground foreground and background colour
***************************************************************************************/
void TFTLIB_Sprites::setBitmapColor(uint16_t c, uint16_t b) {
	if (c == b) b = ~c;
	_lcd->bitmap_fg = c;
	_lcd->bitmap_bg = b;
}


/***************************************************************************************
** Function name:           setPaletteColor
** Description:             Set the 4bpp palette color at the given index
***************************************************************************************/
void TFTLIB_Sprites::setPaletteColor(uint8_t index, uint16_t color) {
	if (_colorMap == nullptr || index > 15) return; // out of bounds
	_colorMap[index] = color;
}


/***************************************************************************************
** Function name:           getPaletteColor
** Description:             Return the palette color at 4bpp index, or 0 on error.
***************************************************************************************/
uint16_t TFTLIB_Sprites::getPaletteColor(uint8_t index) {
	if (_colorMap == nullptr || index > 15) return 0; // out of bounds
	return _colorMap[index];
}


/***************************************************************************************
** Function name:           deleteSprite
** Description:             Delete the sprite to free up memory (RAM)
***************************************************************************************/
void TFTLIB_Sprites::deleteSprite(void) {
	if (_colorMap != nullptr) {
		free(_colorMap);
		_colorMap = nullptr;
	}

	if (_created) {
		free(_img8_1);
		_img8 = nullptr;
		_created = false;
		_vpOoB   = true;
	}
}


/***************************************************************************************
** Function name:           pushRotated - Fast fixed point integer maths version
** Description:             Push rotated Sprite to TFT screen
***************************************************************************************/
#define FP_SCALE 10
bool TFTLIB_Sprites::pushRotated(int16_t angle, uint32_t transp) {

	// Bounding box parameters
	int16_t min_x;
	int16_t min_y;
	int16_t max_x;
	int16_t max_y;

	// Get the bounding box of this rotated source Sprite relative to Sprite pivot
	getRotatedBounds(angle, &min_x, &min_y, &max_x, &max_y);

	uint16_t sline_buffer[max_x - min_x + 1];

	int32_t xt = min_x - _lcd->_xPivot;
	int32_t yt = min_y - _lcd->_yPivot;
	uint32_t xe = _dwidth << FP_SCALE;
	uint32_t ye = _dheight << FP_SCALE;
	uint16_t tpcolor = (uint16_t)transp;

	if (transp != 0x00FFFFFF) {
		if (_bpp == 4) tpcolor = _colorMap[transp & 0x0F];
		tpcolor = tpcolor>>8 | tpcolor<<8; // Working with swapped color bytes
	}

	// Scan destination bounding box and fetch pixels from source Sprite
	for (int32_t y = min_y; y <= max_y; y++, yt++) {
		int32_t x = min_x;
		uint32_t xs = (_cosra * xt - (_sinra * yt - (_xPivot << FP_SCALE)) + (1 << (FP_SCALE - 1)));
		uint32_t ys = (_sinra * xt + (_cosra * yt + (_yPivot << FP_SCALE)) + (1 << (FP_SCALE - 1)));

		while ((xs >= xe || ys >= ye) && x < max_x) { x++; xs += _cosra; ys += _sinra; }
		if (x == max_x) continue;

		uint32_t pixel_count = 0;
		do {
			uint32_t rp;
			int32_t xp = xs >> FP_SCALE;
			int32_t yp = ys >> FP_SCALE;
			if (_bpp == 16) {rp = _img[xp + yp * _iwidth]; }
			else { rp = readPixel(xp, yp); /*rp = (uint16_t)(rp>>8 | rp<<8);*/ }
			if (transp != 0x00FFFFFF && tpcolor == rp) {
				if (pixel_count) {
					// TFT window is already clipped, so this is faster than pushImage()
					_lcd->setWindow8(x - pixel_count, y, x - 1, y);
					_lcd->pushPixels16(sline_buffer, pixel_count);
					pixel_count = 0;
				}
			}

			else sline_buffer[pixel_count++] = rp;

		} while (++x < max_x && (xs += _cosra) < xe && (ys += _sinra) < ye);

		if (pixel_count) {
			// TFT window is already clipped, so this is faster than pushImage()
			_lcd->setWindow8(x - pixel_count, y, x - 1, y);
			_lcd->pushPixels16(sline_buffer, pixel_count);
		}
	}

	return true;
}


/***************************************************************************************
** Function name:           pushRotated - Fast fixed point integer maths version
** Description:             Push a rotated copy of the Sprite to another Sprite
***************************************************************************************/
// Not compatible with 4bpp
bool TFTLIB_Sprites::pushRotated(TFTLIB_Sprites *spr, int16_t angle, uint32_t transp) {
	if ( !_created  || _bpp == 4) return false; // Check this Sprite is created
	if ( !spr->_created  || spr->_bpp == 4) return false;  // Ckeck destination Sprite is created

	// Bounding box parameters
	int16_t min_x;
	int16_t min_y;
	int16_t max_x;
	int16_t max_y;

	// Get the bounding box of this rotated source Sprite
	if ( !getRotatedBounds(spr, angle, &min_x, &min_y, &max_x, &max_y) ) return false;

	uint16_t sline_buffer[max_x - min_x + 1];

	int32_t xt = min_x - spr->_xPivot;
	int32_t yt = min_y - spr->_yPivot;
	uint32_t xe = _dwidth << FP_SCALE;
	uint32_t ye = _dheight << FP_SCALE;
	uint16_t tpcolor = (uint16_t)transp;

	if (transp != 0x00FFFFFF) {
		if (_bpp == 4) tpcolor = _colorMap[transp & 0x0F];
		tpcolor = tpcolor>>8 | tpcolor<<8; // Working with swapped color bytes
	}

	// Scan destination bounding box and fetch transformed pixels from source Sprite
	for (int32_t y = min_y; y <= max_y; y++, yt++) {
		int32_t x = min_x;
		uint32_t xs = (_cosra * xt - (_sinra * yt - (_xPivot << FP_SCALE)) + (1 << (FP_SCALE - 1)));
		uint32_t ys = (_sinra * xt + (_cosra * yt + (_yPivot << FP_SCALE)) + (1 << (FP_SCALE - 1)));

		while ((xs >= xe || ys >= ye) && x < max_x) { x++; xs += _cosra; ys += _sinra; }
		if (x == max_x) continue;

		uint32_t pixel_count = 0;
		do {
			uint32_t rp;
			int32_t xp = xs >> FP_SCALE;
			int32_t yp = ys >> FP_SCALE;
			if (_bpp == 16) rp = _img[xp + yp * _iwidth];
			else { rp = readPixel(xp, yp); /*rp = (uint16_t)(rp>>8 | rp<<8);*/ }
			if (transp != 0x00FFFFFF && tpcolor == rp) {
				if (pixel_count) {
					spr->pushImage(x - pixel_count, y, pixel_count, 1, sline_buffer);
					pixel_count = 0;
				}
			}

			else sline_buffer[pixel_count++] = rp;

		} while (++x < max_x && (xs += _cosra) < xe && (ys += _sinra) < ye);

		if (pixel_count) spr->pushImage(x - pixel_count, y, pixel_count, 1, sline_buffer);
	}
	return true;
}


/***************************************************************************************
** Function name:           getRotatedBounds
** Description:             Get TFT bounding box of a rotated Sprite wrt pivot
***************************************************************************************/
bool TFTLIB_Sprites::getRotatedBounds(int16_t angle, int16_t *min_x, int16_t *min_y, int16_t *max_x, int16_t *max_y) {
	// Get the bounding box of this rotated source Sprite relative to Sprite pivot
	getRotatedBounds(angle, width(), height(), _xPivot, _yPivot, min_x, min_y, max_x, max_y);

	// Move bounding box so source Sprite pivot coincides with TFT pivot
	*min_x += _lcd->_xPivot;
	*max_x += _lcd->_xPivot;
	*min_y += _lcd->_yPivot;
	*max_y += _lcd->_yPivot;

	// Return if bounding box is outside of TFT viewport
	if (*min_x > _lcd->_vpW) return false;
	if (*min_y > _lcd->_vpH) return false;
	if (*max_x < _lcd->_vpX) return false;
	if (*max_y < _lcd->_vpY) return false;

	// Clip bounding box to be within TFT viewport
	if (*min_x < _lcd->_vpX) *min_x = _lcd->_vpX;
	if (*min_y < _lcd->_vpY) *min_y = _lcd->_vpY;
	if (*max_x > _lcd->_vpW) *max_x = _lcd->_vpW;
	if (*max_y > _lcd->_vpH) *max_y = _lcd->_vpH;

	return true;
}


/***************************************************************************************
** Function name:           getRotatedBounds
** Description:             Get destination Sprite bounding box of a rotated Sprite wrt pivot
***************************************************************************************/
bool TFTLIB_Sprites::getRotatedBounds(TFTLIB_Sprites *spr, int16_t angle, int16_t *min_x, int16_t *min_y, int16_t *max_x, int16_t *max_y) {
	// Get the bounding box of this rotated source Sprite relative to Sprite pivot
	getRotatedBounds(angle, width(), height(), _xPivot, _yPivot, min_x, min_y, max_x, max_y);

	// Move bounding box so source Sprite pivot coincides with destination Sprite pivot
	*min_x += spr->_xPivot;
	*max_x += spr->_xPivot;
	*min_y += spr->_yPivot;
	*max_y += spr->_yPivot;

	// Return if bounding box is completely outside of destination Sprite
	if (*min_x > spr->width()) return true;
	if (*min_y > spr->height()) return true;
	if (*max_x < 0) return true;
	if (*max_y < 0) return true;

	// Clip bounding box to Sprite boundaries
	if (*min_x < 0) min_x = 0;
	if (*min_y < 0) min_y = 0;
	if (*max_x > spr->width())  *max_x = spr->width();
	if (*max_y > spr->height()) *max_y = spr->height();

	return true;
}


/***************************************************************************************
** Function name:           rotatedBounds
** Description:             Get bounding box of a rotated Sprite wrt pivot
***************************************************************************************/
void TFTLIB_Sprites::getRotatedBounds(int16_t angle, int16_t w, int16_t h, int16_t xp, int16_t yp,
                                   int16_t *min_x, int16_t *min_y, int16_t *max_x, int16_t *max_y) {
	// Trig values for the rotation
	float radAngle = -angle * 0.0174532925; // Convert degrees to radians
	float sina = sinf(radAngle);
	float cosa = cosf(radAngle);

	w -= xp; // w is now right edge coordinate relative to xp
	h -= yp; // h is now bottom edge coordinate relative to yp

	// Calculate new corner coordinates
	int16_t x0 = -xp * cosa - yp * sina;
	int16_t y0 =  xp * sina - yp * cosa;

	int16_t x1 =  w * cosa - yp * sina;
	int16_t y1 = -w * sina - yp * cosa;

	int16_t x2 =  h * sina + w * cosa;
	int16_t y2 =  h * cosa - w * sina;

	int16_t x3 =  h * sina - xp * cosa;
	int16_t y3 =  h * cosa + xp * sina;

	// Find bounding box extremes, enlarge box to accomodate rounding errors
	*min_x = x0-2;
	if (x1 < *min_x) *min_x = x1-2;
	if (x2 < *min_x) *min_x = x2-2;
	if (x3 < *min_x) *min_x = x3-2;

	*max_x = x0+2;
	if (x1 > *max_x) *max_x = x1+2;
	if (x2 > *max_x) *max_x = x2+2;
	if (x3 > *max_x) *max_x = x3+2;

	*min_y = y0-2;
	if (y1 < *min_y) *min_y = y1-2;
	if (y2 < *min_y) *min_y = y2-2;
	if (y3 < *min_y) *min_y = y3-2;

	*max_y = y0+2;
	if (y1 > *max_y) *max_y = y1+2;
	if (y2 > *max_y) *max_y = y2+2;
	if (y3 > *max_y) *max_y = y3+2;

	_sinra = roundf(sina * (1<<FP_SCALE));
	_cosra = roundf(cosa * (1<<FP_SCALE));
}


/***************************************************************************************
** Function name:           pushSprite
** Description:             Push the sprite to the TFT at x, y
***************************************************************************************/
void TFTLIB_Sprites::pushSprite(int32_t x, int32_t y) {
	if (!_created) return;

	if (_bpp == 16) _lcd->drawImage(x, y, _dwidth, _dheight, _img );

	else if (_bpp == 4) _lcd->pushImage(x, y, _dwidth, _dheight, _img4, false, _colorMap);

	else _lcd->pushImage(x, y, _dwidth, _dheight, _img8, (bool)(_bpp == 8));
}


/***************************************************************************************
** Function name:           pushSprite
** Description:             Push the sprite to the TFT at x, y with transparent colour
***************************************************************************************/
void TFTLIB_Sprites::pushSprite(int32_t x, int32_t y, uint16_t transp) {
  if (!_created) return;

  if (_bpp == 16) _lcd->pushImage(x, y, _dwidth, _dheight, _img, transp );

  else if (_bpp == 8) {
    transp = (uint8_t)((transp & 0xE000)>>8 | (transp & 0x0700)>>6 | (transp & 0x0018)>>3);
    _lcd->pushImage(x, y, _dwidth, _dheight, _img8, (uint8_t)transp, (bool)true);
  }
}


/***************************************************************************************
** Function name:           pushToSprite
** Description:             Push the sprite to another sprite at x, y
***************************************************************************************/
bool TFTLIB_Sprites::pushToSprite(TFTLIB_Sprites *dspr, int32_t x, int32_t y) {
	if (!_created) return false;
	if (!dspr->created()) return false;

	// Check destination sprite compatibility
	int8_t ds_bpp = dspr->getColorDepth();
	if (_bpp == 16 && ds_bpp != 16 && ds_bpp !=  8) return false;
	if (_bpp ==  8 && ds_bpp !=  8) return false;
	if (_bpp ==  4 && ds_bpp !=  4) return false;
	if (_bpp ==  1 && ds_bpp !=  1) return false;

	dspr->pushImage(x, y, _dwidth, _dheight, _img, _bpp);

	return true;
}


/***************************************************************************************
** Function name:           pushToSprite
** Description:             Push the sprite to another sprite at x, y with transparent colour
***************************************************************************************/
bool TFTLIB_Sprites::pushToSprite(TFTLIB_Sprites *dspr, int32_t x, int32_t y, uint16_t transp) {
	if ( !_created  || !dspr->_created) return false; // Check Sprites exist

	// Check destination sprite compatibility
	int8_t ds_bpp = dspr->getColorDepth();
	if (_bpp == 16 && ds_bpp != 16 && ds_bpp !=  8) return false;
	if (_bpp ==  8 && ds_bpp !=  8) return false;
	if (_bpp ==  4 || ds_bpp ==  4) return false;
	if (_bpp ==  1 && ds_bpp !=  1) return false;

	uint16_t sline_buffer[width()];

	transp = transp>>8 | transp<<8;

	// Scan destination bounding box and fetch transformed pixels from source Sprite
	for (int32_t ys = 0; ys < height(); ys++) {
		int32_t ox = x;
		uint32_t pixel_count = 0;

		for (int32_t xs = 0; xs < width(); xs++) {
			uint16_t rp = 0;
			if (_bpp == 16) rp = _img[xs + ys * width()];
			else { rp = readPixel(xs, ys); /*rp = rp>>8 | rp<<8;*/ }
			//dspr->drawPixel(xs, ys, rp);

			if (transp == rp) {
				if (pixel_count) {
					dspr->pushImage(ox, y, pixel_count, 1, sline_buffer, _bpp);
					ox += pixel_count;
					pixel_count = 0;
				}
				ox++;
			}

			else sline_buffer[pixel_count++] = rp;
		}

		if (pixel_count) dspr->pushImage(ox, y, pixel_count, 1, sline_buffer);
		y++;
	}
	return true;
}


/***************************************************************************************
** Function name:           pushSprite
** Description:             Push a cropped sprite to the TFT at tx, ty
***************************************************************************************/
bool TFTLIB_Sprites::pushSprite(int32_t tx, int32_t ty, int32_t sx, int32_t sy, int32_t sw, int32_t sh) {
	if (!_created) return false;

	// Perform window boundary checks and crop if needed
	setWindow(sx, sy, sx + sw - 1, sy + sh - 1);

	// Calculate new sprite window bounding box width and height
	sw = _xe - _xs + 1;
	sh = _ye - _ys + 1;

	if (_ys >= _iheight) return false;

	if (_bpp == 16) {
		// Check if a faster block copy to screen is possible
		if ( sx == 0 && sw == _dwidth) _lcd->drawImage(tx, ty, sw, sh, _img + _iwidth * _ys);

		else // Render line by line
			while (sh--) _lcd->drawImage(tx, ty++, sw, 1, _img + _xs + _iwidth * _ys++);
	}

	else if (_bpp == 8) {
		// Check if a faster block copy to screen is possible
		if ( sx == 0 && sw == _dwidth) _lcd->pushImage(tx, ty, sw, sh, _img8 + _iwidth * _ys, (bool)true );

		else // Render line by line
			while (sh--) _lcd->pushImage(tx, ty++, sw, 1, _img8 + _xs + _iwidth * _ys++, (bool)true );
	}

	return true;
}


/***************************************************************************************
** Function name:           readPixelValue
** Description:             Read the color map index of a pixel at defined coordinates
***************************************************************************************/
uint16_t TFTLIB_Sprites::readPixelValue(int32_t x, int32_t y)
{
	if (_vpOoB  || !_created) return 0xFF;

	x+= _xDatum;
	y+= _yDatum;

	// Range checking
	if ((x < _vpX) || (y < _vpY) ||(x >= _vpW) || (y >= _vpH)) return 0xFF;

	if (_bpp == 16) return readPixel(x - _xDatum, y - _yDatum);

	if (_bpp == 8) return _img8[x + y * _iwidth];

	return 0;
}

/***************************************************************************************
** Function name:           readPixel
** Description:             Read 565 colour of a pixel at defined coordinates
***************************************************************************************/
uint16_t TFTLIB_Sprites::readPixel(int32_t x, int32_t y) {
	if (_vpOoB  || !_created) return 0xFFFF;

	x+= _xDatum;
	y+= _yDatum;

	// Range checking
	if ((x < _vpX) || (y < _vpY) ||(x >= _vpW) || (y >= _vpH)) return 0xFFFF;

	if (_bpp == 16) {
		uint16_t color = _img[x + y * _iwidth];
		return color; //>> 8) | (color << 8);
	}

	if (_bpp == 8) {
		uint16_t color = _img8[x + y * _iwidth];
		if (color != 0) {
			uint8_t  blue[] = {0, 11, 21, 31};
			color = (color & 0xE0)<<8 | (color & 0xC0)<<5 | (color & 0x1C)<<6 | (color & 0x1C)<<3 | blue[color & 0x03];
		}
		return color;
	}

	if (rotation == 1) {
		uint16_t tx = x;
		x = _dheight - y - 1;
		y = tx;
	}

	else if (rotation == 2) {
		x = _dwidth - x - 1;
		y = _dheight - y - 1;
	}

	else if (rotation == 3) {
		uint16_t tx = x;
		x = y;
		y = _dwidth - tx - 1;
	}

	uint16_t color = (_img8[(x + y * _bitwidth)>>3] << (x & 0x7)) & 0x80;

	if (color) return _lcd->bitmap_fg;
	else       return _lcd->bitmap_bg;
}


/***************************************************************************************
** Function name:           pushImage
** Description:             push image into a defined area of a sprite
***************************************************************************************/
void  TFTLIB_Sprites::pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t *data, uint8_t sbpp) {
	if (data == nullptr || !_created) return;

	if (_bpp == 16) {
		// Pointer within original image
		uint8_t *ptro = (uint8_t *)data + ((x + y * w) << 1);
		// Pointer within sprite image
		uint8_t *ptrs = (uint8_t *)_img + ((x + y * _iwidth) << 1);

		while (h--) {
			memcpy(ptrs, ptro, w<<1);
			ptro += w << 1;
			ptrs += _iwidth << 1;
		}
	}

	else if (_bpp == 8 && sbpp == 8) {
		// Pointer within original image
		uint8_t *ptro = (uint8_t *)data + (x + y * w);
		// Pointer within sprite image
		uint8_t *ptrs = (uint8_t *)_img + (x + y * _iwidth);

		while (h--) {
			memcpy(ptrs, ptro, w);
			ptro += w;
			ptrs += _iwidth;
		}
	}

	else if (_bpp == 8) {
		uint16_t lastColor = 0;
		uint8_t  color8    = 0;
		for (int32_t yp = _ys; yp < _ys + h; yp++) {
			int32_t xyw = x + y * _iwidth;
			int32_t dxypw = _xs + yp * w;
			for (int32_t xp = _xs; xp < _xs + w; xp++) {
				uint16_t color = data[dxypw++];
				if (color != lastColor) {
					// When data source is a sprite, the bytes are already swapped
					//if(!_swapBytes) color8 = (uint8_t)((color & 0xE0) | (color & 0x07)<<2 | (color & 0x1800)>>11);
					/*else*/ color8 = (uint8_t)((color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3);
				}

				lastColor = color;
				_img8[xyw++] = color8;
			}
			y++;
		}
	}
}


/***************************************************************************************
** Function name:           pushImage
** Description:             push 565 colour FLASH (PROGMEM) image into a defined area
***************************************************************************************/
void  TFTLIB_Sprites::pushImage(int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t *data) {
	// Partitioned memory FLASH processor
	if (data == nullptr || !_created) return;

	if (_bpp == 16) {
		for (int32_t yp = _ys; yp < _ys + h; yp++) {
			int32_t ox = x;
			for (int32_t xp = _xs; xp < _xs + w; xp++) {
				uint16_t color = read_word(data + xp + yp * w);
				//if(_swapBytes) color = color<<8 | color>>8;
				_img[ox + y * _iwidth] = color;
				ox++;
			}
			y++;
		}
	}

	else if (_bpp == 8) {
		for (int32_t yp = _ys; yp < _ys + h; yp++) {
			int32_t ox = x;
			for (int32_t xp = _xs; xp < _xs + w; xp++) {
				uint16_t color = read_word(data + xp + yp * w);
				//if(_swapBytes) color = color<<8 | color>>8;
				_img8[ox + y * _iwidth] = (uint8_t)((color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3);
				ox++;
			}
			y++;
		}
	}
}


/***************************************************************************************
** Function name:           setWindow
** Description:             Set the bounds of a window in the sprite
***************************************************************************************/
// Intentionally not constrained to viewport area, does not manage 1bpp rotations
void TFTLIB_Sprites::setWindow(int32_t x0, int32_t y0, int32_t x1, int32_t y1) {
	if (x0 > x1) swap_coord(x0, x1);
	if (y0 > y1) swap_coord(y0, y1);

	int32_t w = width();
	int32_t h = height();

	if ((x0 >= w) || (x1 < 0) || (y0 >= h) || (y1 < 0)) { // Point to that extra "off screen" pixel
		_xs = 0;
		_ys = _dheight;
		_xe = 0;
		_ye = _dheight;
	}

	else {
		if (x0 < 0) x0 = 0;
		if (x1 >= w) x1 = w - 1;
		if (y0 < 0) y0 = 0;
		if (y1 >= h) y1 = h - 1;

		_xs = x0;
		_ys = y0;
		_xe = x1;
		_ye = y1;
	}

	_xptr = _xs;
	_yptr = _ys;
}


/***************************************************************************************
** Function name:           pushColor
** Description:             Send a new pixel to the set window
***************************************************************************************/
void TFTLIB_Sprites::pushColor(uint16_t color) {
	if (!_created ) return;

	// Write the colour to RAM in set window
	if (_bpp == 16) _img [_xptr + _yptr * _iwidth] = (uint16_t) (color >> 8) | (color << 8);

	else  if (_bpp == 8)
		_img8[_xptr + _yptr * _iwidth] = (uint8_t )((color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3);

	else if (_bpp == 4) {
		uint8_t c = (uint8_t)color & 0x0F;

		if ((_xptr & 0x01) == 0) {
			_img4[(_xptr + _yptr * _iwidth)>>1] = (c << 4) | (_img4[(_xptr + _yptr * _iwidth)>>1] & 0x0F);  // new color is in bits 7 .. 4
		}

		else {
			_img4[(_xptr + _yptr * _iwidth)>>1] = (_img4[(_xptr + _yptr * _iwidth)>>1] & 0xF0) | c; // new color is the low bits
		}
	}

	else drawPixel(_xptr, _yptr, color);

	// Increment x
	_xptr++;

	// Wrap on x and y to start, increment y if needed
	if (_xptr > _xe) {
		_xptr = _xs;
		_yptr++;
		if (_yptr > _ye) _yptr = _ys;
	}
}


/***************************************************************************************
** Function name:           pushColor
** Description:             Send a "len" new pixels to the set window
***************************************************************************************/
void TFTLIB_Sprites::pushColor(uint16_t color, uint32_t len) {
	if (!_created ) return;

	uint16_t pixelColor;

	if (_bpp == 16) pixelColor = color/*(uint16_t) (color >> 8) | (color << 8)*/;

	else  if (_bpp == 8) pixelColor = (color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3;

	else pixelColor = (uint16_t) color; // for 1bpp or 4bpp

	while(len--) writeColor(pixelColor);
}


/***************************************************************************************
** Function name:           writeColor
** Description:             Write a pixel with pre-formatted colour to the set window
***************************************************************************************/
void TFTLIB_Sprites::writeColor(uint16_t color) {
	if (!_created ) return;

	// Write 16 bit RGB 565 encoded colour to RAM
	if (_bpp == 16) _img [_xptr + _yptr * _iwidth] = color;

	// Write 8 bit RGB 332 encoded colour to RAM
	else if (_bpp == 8) _img8[_xptr + _yptr * _iwidth] = (uint8_t) color;

	else if (_bpp == 4) {
		uint8_t c = (uint8_t)color & 0x0F;
		if ((_xptr & 0x01) == 0)
			_img4[(_xptr + _yptr * _iwidth)>>1] = (c << 4) | (_img4[(_xptr + _yptr * _iwidth)>>1] & 0x0F);  // new color is in bits 7 .. 4
		else
			_img4[(_xptr + _yptr * _iwidth)>>1] = (_img4[(_xptr + _yptr * _iwidth)>>1] & 0xF0) | c; // new color is the low bits (x is odd)
	}

	else drawPixel(_xptr, _yptr, color);

	// Increment x
	_xptr++;

	// Wrap on x and y to start, increment y if needed
	if (_xptr > _xe) {
		_xptr = _xs;
		_yptr++;
		if (_yptr > _ye) _yptr = _ys;
	}
}


/***************************************************************************************
** Function name:           setScrollRect
** Description:             Set scroll area within the sprite and the gap fill colour
***************************************************************************************/
// Intentionally not constrained to viewport area
void TFTLIB_Sprites::setScrollRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color) {
	if ((x >= _iwidth) || (y >= _iheight) || !_created ) return;

	if (x < 0) { w += x; x = 0; }
	if (y < 0) { h += y; y = 0; }

	if ((x + w) > _iwidth ) w = _iwidth  - x;
	if ((y + h) > _iheight) h = _iheight - y;

	if ( w < 1 || h < 1) return;

	_sx = x;
	_sy = y;
	_sw = w;
	_sh = h;

	_scolor = color;
}


/***************************************************************************************
** Function name:           scroll
** Description:             Scroll dx,dy pixels, positive right,down, negative left,up
***************************************************************************************/
void TFTLIB_Sprites::scroll(int16_t dx, int16_t dy) {
	if (abs(dx) >= _sw || abs(dy) >= _sh) {
		fillRect (_sx, _sy, _sw, _sh, _scolor);
		return;
	}

	// Fetch the scroll area width and height set by setScrollRect()
	uint32_t w  = _sw - abs(dx); // line width to copy
	uint32_t h  = _sh - abs(dy); // lines to copy
	int32_t iw  = _iwidth;       // rounded up width of sprite

	// Fetch the x,y origin set by setScrollRect()
	uint32_t tx = _sx; // to x
	uint32_t fx = _sx; // from x
	uint32_t ty = _sy; // to y
	uint32_t fy = _sy; // from y

	// Adjust for x delta
	if (dx <= 0) fx -= dx;
	else tx += dx;

	// Adjust for y delta
	if (dy <= 0) fy -= dy;
	else { // Scrolling down so start copy from bottom
		ty = ty + _sh - 1; // "To" pointer
		iw = -iw;          // Pointer moves backwards
		fy = ty - dy;      // "From" pointer
	}

	// Calculate "from y" and "to y" pointers in RAM
	uint32_t fyp = fx + fy * _iwidth;
	uint32_t typ = tx + ty * _iwidth;

	// Now move the pixels in RAM
	if (_bpp == 16) {
		while (h--) { // move pixel lines (to, from, byte count)
			memmove( _img + typ, _img + fyp, w<<1);
			typ += iw;
			fyp += iw;
		}
	}

	else if (_bpp == 8) {
		while (h--) { // move pixel lines (to, from, byte count)
			memmove( _img8 + typ, _img8 + fyp, w);
			typ += iw;
			fyp += iw;
		}
	}

	else if (_bpp == 4) {
		// could optimize for scrolling by even # pixels using memove (later)
		if (dx >  0) { tx += w; fx += w; } // Start from right edge
		while (h--) { // move pixels one by one
			for (uint16_t xp = 0; xp < w; xp++) {
				if (dx <= 0) drawPixel(tx + xp, ty, readPixelValue(fx + xp, fy));
				if (dx >  0) drawPixel(tx - xp, ty, readPixelValue(fx - xp, fy));
			}

			if (dy <= 0)  { ty++; fy++; }
			else  { ty--; fy--; }
		}
	}

	else if (_bpp == 1 ) {
		if (dx >  0) { tx += w; fx += w; } // Start from right edge
		while (h--) { // move pixels one by one
			for (uint16_t xp = 0; xp < w; xp++) {
				if (dx <= 0) drawPixel(tx + xp, ty, readPixelValue(fx + xp, fy));
				if (dx >  0) drawPixel(tx - xp, ty, readPixelValue(fx - xp, fy));
			}

			if (dy <= 0)  { ty++; fy++; }
			else  { ty--; fy--; }
		}
	}

	else return; // Not 1, 4, 8 or 16 bpp

	// Fill the gap left by the scrolling
	if (dx > 0) fillRect(_sx, _sy, dx, _sh, _scolor);
	if (dx < 0) fillRect(_sx + _sw + dx, _sy, -dx, _sh, _scolor);
	if (dy > 0) fillRect(_sx, _sy, _sw, dy, _scolor);
	if (dy < 0) fillRect(_sx, _sy + _sh + dy, _sw, -dy, _scolor);
}


/***************************************************************************************
** Function name:           fillSprite
** Description:             Fill the whole sprite with defined colour
***************************************************************************************/
void TFTLIB_Sprites::fillSprite(uint32_t color) {
	if (!_created || _vpOoB) return;

	// Use memset if possible as it is super fast
	if(_xDatum == 0 && _yDatum == 0  &&  _xWidth == width()) {
		if(_bpp == 16) {
			if ( (uint8_t)color == (uint8_t)(color>>8) ) memset(_img,  (uint8_t)color, _iwidth * _yHeight * 2);
			else fillRect(_vpX, _vpY, _xWidth, _yHeight, color);
		}

		else if (_bpp == 8) {
			color = (color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3;
			memset(_img8, (uint8_t)color, _iwidth * _yHeight);
		}

		else if (_bpp == 4) {
			uint8_t c = ((color & 0x0F) | (((color & 0x0F) << 4) & 0xF0));
			memset(_img4, c, (_iwidth * _yHeight) >> 1);
		}

		else if (_bpp == 1) {
			if(color) memset(_img8, 0xFF, (_bitwidth>>3) * _dheight + 1);
			else      memset(_img8, 0x00, (_bitwidth>>3) * _dheight + 1);
		}
	}
	else fillRect(_vpX - _xDatum, _vpY - _yDatum, _xWidth, _yHeight, color);
}


/***************************************************************************************
** Function name:           width
** Description:             Return the width of sprite
***************************************************************************************/
// Return the size of the sprite
int16_t TFTLIB_Sprites::width(void) {
	if (!_created ) return 0;

	if (_bpp > 1) {
		if (_vpDatum) return _xWidth;
		return _dwidth;
	}

	if (rotation & 1) {
		if (_vpDatum) return _xWidth;
		return _dheight;
	}

	if (_vpDatum) return _xWidth;
	return _dwidth;
}


/***************************************************************************************
** Function name:           height
** Description:             Return the height of sprite
***************************************************************************************/
int16_t TFTLIB_Sprites::height(void) {
	if (!_created ) return 0;

	if (_bpp > 1) {
		if (_vpDatum) return _yHeight;
		return _dheight;
	}

	if (rotation & 1) {
		if (_vpDatum) return _yHeight;
		return _dwidth;
	}

	if (_vpDatum) return _yHeight;
	return _dheight;
}


/***************************************************************************************
** Function name:           setRotation
** Description:             Rotate coordinate frame for 1bpp sprite
***************************************************************************************/
// Does nothing for 4, 8 and 16 bpp sprites.
void TFTLIB_Sprites::setRotation(uint8_t r) {
	if (_bpp != 1) return;

	rotation = r;

	if (rotation&1) resetViewport();
	else resetViewport();
}


/***************************************************************************************
** Function name:           getRotation
** Description:             Get rotation for 1bpp sprite
***************************************************************************************/
uint8_t TFTLIB_Sprites::getRotation(void) {
	return rotation;
}


/***************************************************************************************
** Function name:           drawPixel
** Description:             push a single pixel at an arbitrary position
***************************************************************************************/
void TFTLIB_Sprites::drawPixel(int32_t x, int32_t y, uint32_t color) {
	if (!_created) return;

	x+= _xDatum;
	y+= _yDatum;

	if (_bpp == 16) {
		_img[x+y*_iwidth] = (uint16_t) color;
	}

	else if (_bpp == 8) {
		_img8[x+y*_iwidth] = (uint8_t)((color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3);
	}
}


/***************************************************************************************
** Function name:           drawLine
** Description:             draw a line between 2 arbitrary points
***************************************************************************************/
void TFTLIB_Sprites::drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color) {
	if (!_created) return;

	bool steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		swap_coord(x0, y0);
		swap_coord(x1, y1);
	}

	if (x0 > x1) {
		swap_coord(x0, x1);
		swap_coord(y0, y1);
	}

	int32_t dx = x1 - x0, dy = abs(y1 - y0);;

	int32_t err = dx >> 1, ystep = -1, xs = x0, dlen = 0;

	if (y0 < y1) ystep = 1;

	// Split into steep and not steep for FastH/V separation
	if (steep) {
		for (; x0 <= x1; x0++) {
			dlen++;
			err -= dy;
			if (err < 0) {
				err += dx;
				if (dlen == 1) drawPixel(y0, xs, color);
				else drawFastVLine(y0, xs, dlen, color);
				dlen = 0; y0 += ystep; xs = x0 + 1;
			}
		}
		if (dlen) drawFastVLine(y0, xs, dlen, color);
	}

	else {
		for (; x0 <= x1; x0++) {
			dlen++;
			err -= dy;
			if (err < 0) {
				err += dx;
				if (dlen == 1) drawPixel(xs, y0, color);
				else drawFastHLine(xs, y0, dlen, color);
				dlen = 0; y0 += ystep; xs = x0 + 1;
			}
		}
		if (dlen) drawFastHLine(xs, y0, dlen, color);
	}
}


/***************************************************************************************
** Function name:           drawFastVLine
** Description:             draw a vertical line
***************************************************************************************/
void TFTLIB_Sprites::drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color) {

	if (_bpp == 16) {
		int32_t yp = x + _iwidth * y;

		while (h--) {
			_img[yp] = (uint16_t) color;
			yp += _iwidth;
		}
	}

	else if (_bpp == 8) {
		color = (color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3;
		while (h--) _img8[x + _iwidth * y++] = (uint8_t) color;
	}
}


/***************************************************************************************
** Function name:           drawFastHLine
** Description:             draw a horizontal line
***************************************************************************************/
void TFTLIB_Sprites::drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color) {
	if (_bpp == 16) {
		while (w--) _img[_iwidth * y + x++] = (uint16_t) color;
	}

	else if (_bpp == 8) {
		color = (color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3;
		memset(_img8+_iwidth * y + x, (uint8_t)color, w);
	}
}

void TFTLIB_Sprites::fillCircle(int32_t x0, int32_t y0, int32_t r, uint16_t color) {
	int32_t  x  = 0;
	int32_t  dx = 1;
	int32_t  dy = r+r;
	int32_t  p  = -(r>>1);

	drawFastHLine(x0 - r, y0, dy+1, color);

	while(x<r){
		if(p>=0) {
			drawFastHLine(x0 - x + 1, y0 + r, dx-1, color);
			drawFastHLine(x0 - x + 1, y0 - r, dx-1, color);
			dy-=2;
			p-=dy;
			r--;
		}

		dx+=2;
		p+=dx;
		x++;

		drawFastHLine(x0 - r, y0 + x, dy+1, color);
		drawFastHLine(x0 - r, y0 - x, dy+1, color);
	}
}


/***************************************************************************************
** Function name:           fillRect
** Description:             draw a filled rectangle
***************************************************************************************/
void TFTLIB_Sprites::fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
	if (!_created) return;

	x+= _xDatum;
	y+= _yDatum;

	int32_t yp = _iwidth * y + x;

	if (_bpp == 16) {
		uint32_t iw = w;
		int32_t ys = yp;
		if(h--)  {while (iw--) _img[yp++] = (uint16_t) color;}
		yp = ys;
		while (h--) {
			yp += _iwidth;
			memcpy( _img+yp, _img+ys, w<<1);
		}
	}

	else if (_bpp == 8) {
		color = (color & 0xE000)>>8 | (color & 0x0700)>>6 | (color & 0x0018)>>3;
		while (h--) {
			memset(_img8 + yp, (uint8_t)color, w);
			yp += _iwidth;
		}
	}
}

/***************************************************************************************
** Function name:           setViewport
** Description:             Set the clipping region for the TFT screen
***************************************************************************************/
void TFTLIB_Sprites::setViewport(int32_t x, int32_t y, int32_t w, int32_t h, bool vpDatum) {
	// Viewport metrics (not clipped)
	_xDatum  = x; // Datum x position in screen coordinates
	_yDatum  = y; // Datum y position in screen coordinates
	_xWidth  = w; // Viewport width
	_yHeight = h; // Viewport height

	// Full size default viewport
	_vpDatum = false; // Datum is at top left corner of screen (true = top left of viewport)
	_vpOoB   = false; // Out of Bounds flag (true is all of viewport is off screen)
	_vpX = 0;         // Viewport top left corner x coordinate
	_vpY = 0;         // Viewport top left corner y coordinate
	_vpW = width();   // Equivalent of TFT width  (Nb: viewport right edge coord + 1)
	_vpH = height();  // Equivalent of TFT height (Nb: viewport bottom edge coord + 1)

	// Clip viewport to screen area
	if (x<0) { w += x; x = 0; }
	if (y<0) { h += y; y = 0; }
	if ((x + w) > width() ) { w = width()  - x; }
	if ((y + h) > height() ) { h = height() - y; }

	// Check if viewport is entirely out of bounds
	if (w < 1 || h < 1) {
		// Set default values and Out of Bounds flag in case of error
		_xDatum = 0;
		_yDatum = 0;
		_xWidth  = width();
		_yHeight = height();
		_vpOoB = true;      // Set Out of Bounds flag to inhibit all drawing
		return;
	}

	if (!vpDatum) {
		_xDatum = 0; // Reset to top left of screen if not using a viewport datum
		_yDatum = 0;
		_xWidth  = width();
		_yHeight = height();
	}

	// Store the clipped screen viewport metrics and datum position
	_vpX = x;
	_vpY = y;
	_vpW = x + w;
	_vpH = y + h;
	_vpDatum = vpDatum;
}

/***************************************************************************************
** Function name:           checkViewport
** Description:             Check if any part of specified area is visible in viewport
***************************************************************************************/
bool TFTLIB_Sprites::checkViewport(int32_t x, int32_t y, int32_t w, int32_t h) {
	if (_vpOoB) return false;
	x+= _xDatum;
	y+= _yDatum;

	if ((x >= _vpW) || (y >= _vpH)) return false;

	int32_t dx = 0;
	int32_t dy = 0;
	int32_t dw = w;
	int32_t dh = h;

	if (x < _vpX) { dx = _vpX - x; dw -= dx; x = _vpX; }
	if (y < _vpY) { dy = _vpY - y; dh -= dy; y = _vpY; }

	if ((x + dw) > _vpW ) dw = _vpW - x;
	if ((y + dh) > _vpH ) dh = _vpH - y;

	if (dw < 1 || dh < 1) return false;

	return true;
}

/***************************************************************************************
** Function name:           resetViewport
** Description:             Reset viewport to whole TFT screen, datum at 0,0
***************************************************************************************/
void TFTLIB_Sprites::resetViewport(void) {
	// Reset viewport to the whole screen (or sprite) area
	_vpDatum = false;
	_vpOoB   = false;
	_xDatum = 0;
	_yDatum = 0;
	_vpX = 0;
	_vpY = 0;
	_vpW = width();
	_vpH = height();
	_xWidth  = width();
	_yHeight = height();
}

/***************************************************************************************
** Function name:           getViewportX
** Description:             Get x position of the viewport datum
***************************************************************************************/
int32_t  TFTLIB_Sprites::getViewportX(void) {
	return _xDatum;
}

/***************************************************************************************
** Function name:           getViewportY
** Description:             Get y position of the viewport datum
***************************************************************************************/
int32_t  TFTLIB_Sprites::getViewportY(void) {
	return _yDatum;
}

/***************************************************************************************
** Function name:           getViewportWidth
** Description:             Get width of the viewport
***************************************************************************************/
int32_t TFTLIB_Sprites::getViewportWidth(void) {
	return _xWidth;
}

/***************************************************************************************
** Function name:           getViewportHeight
** Description:             Get height of the viewport
***************************************************************************************/
int32_t TFTLIB_Sprites::getViewportHeight(void) {
	return _yHeight;
}

/***************************************************************************************
** Function name:           getViewportDatum
** Description:             Get datum flag of the viewport (true = viewport corner)
***************************************************************************************/
bool  TFTLIB_Sprites::getViewportDatum(void) {
	return _vpDatum;
}



/***************************************************************************************
** Function name:           frameViewport
** Description:             Draw a frame inside or outside the viewport of width w
***************************************************************************************/
void TFTLIB_Sprites::frameViewport(uint16_t color, int32_t w) {
	// Save datum position
	bool _dT = _vpDatum;

	// If w is positive the frame is drawn inside the viewport
	// a large positive width will clear the screen inside the viewport
	if (w>0) {
		// Set vpDatum true to simplify coordinate derivation
		_vpDatum = true;
		fillRect(0, 0, _vpW - _vpX, w, color);                // Top
		fillRect(0, w, w, _vpH - _vpY - w - w, color);        // Left
		fillRect(_xWidth - w, w, w, _yHeight - w - w, color); // Right
		fillRect(0, _yHeight - w, _xWidth, w, color);         // Bottom
	}

	else {
		w = -w;

		// Save old values
		int32_t _xT = _vpX; _vpX = 0;
		int32_t _yT = _vpY; _vpY = 0;
		int32_t _wT = _vpW;
		int32_t _hT = _vpH;

		// Set vpDatum false so frame can be drawn outside window
		_vpDatum = false; // When false the full width and height is accessed
		_vpH = height();
		_vpW = width();

		// Draw frame
		fillRect(_xT - w - _xDatum, _yT - w - _yDatum, _wT - _xT + w + w, w, color); // Top
		fillRect(_xT - w - _xDatum, _yT - _yDatum, w, _hT - _yT, color);             // Left
		fillRect(_wT - _xDatum, _yT - _yDatum, w, _hT - _yT, color);                 // Right
		fillRect(_xT - w - _xDatum, _hT - _yDatum, _wT - _xT + w + w, w, color);     // Bottom

		// Restore old values
		_vpX = _xT;
		_vpY = _yT;
		_vpW = _wT;
		_vpH = _hT;
	}

	// Restore vpDatum
	_vpDatum = _dT;
}

/***************************************************************************************
** Function name:           setPivot
** Description:             Set the pivot point on the TFT
*************************************************************************************x*/
void TFTLIB_Sprites::setPivot(int16_t x, int16_t y) {
	_xPivot = x;
	_yPivot = y;
}


/***************************************************************************************
** Function name:           getPivotX
** Description:             Get the x pivot position
***************************************************************************************/
int16_t TFTLIB_Sprites::getPivotX(void) {
	return _xPivot;
}


/***************************************************************************************
** Function name:           getPivotY
** Description:             Get the y pivot position
***************************************************************************************/
int16_t TFTLIB_Sprites::getPivotY(void) {
	return _yPivot;
}

template <typename T> static inline void
swap_val(T& a, T& b) { T t = a; a = b; b = t; }
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/***************************************************************************************
** Function name:           SliderWidget
** Description:             Constructor with pointers to TFT and sprite instances
***************************************************************************************/
SliderWidget::SliderWidget(TFTLIB_8BIT *tft, TFTLIB_Sprites *spr) {
  _tft = tft;
  _spr = spr;
}

/***************************************************************************************
** Function name:           createSlider
** Description:             Create slider with slot parameters
***************************************************************************************/
bool SliderWidget::createSlider(uint16_t slotWidth, uint16_t slotLength, uint16_t slotColor, uint16_t bgColor, bool orientation) {
  _slotWidth   = slotWidth;
  _slotLength  = slotLength;
  _slotColor   = slotColor;
  _slotBgColor     = bgColor;
  _horiz = orientation;
  _kposPrev = slotLength/2;
  return 0;
}

/***************************************************************************************
** Function name:           createKnob
** Description:             Create the slider control knob with parameters
***************************************************************************************/
void SliderWidget::createKnob(uint16_t kwidth, uint16_t kheight, uint16_t kradius, uint16_t kcolor1, uint16_t kcolor2) {
  _kwidth  = kwidth;
  _kheight = kheight;
  _kradius = kradius;
  _kcolor1 = kcolor1;
  _kcolor2 = kcolor2;

  _sliderPos =  _slotLength/2;
}


/***************************************************************************************
** Function name:           setSliderScale
** Description:             Set slider scale range with movement delay in us per pixel
***************************************************************************************/
void SliderWidget::setSliderScale(int16_t min, int16_t max, uint16_t usdelay) {
  setSliderScale(min, max);
  _usdelay = usdelay;
}

/***************************************************************************************
** Function name:           setSliderScale
** Description:             Set slider scale range (no movement delay)
***************************************************************************************/
void SliderWidget::setSliderScale(int16_t min, int16_t max) {

  if (min > max) {
    _invert = true;
    swap_val(min, max);
  }
  else {
    _invert = false;
  }
  _sliderMin = min;
  _sliderMax = max;
  _sliderPos = min;
}

/***************************************************************************************
** Function name:           setSliderPosition
** Description:             Set slider position to a value in the set scale range
***************************************************************************************/
void SliderWidget::setSliderPosition(int16_t val)
{
  moveTo(_invert ? _sliderMax - val : val);
}

/***************************************************************************************
** Function name:           getSliderPosition
** Description:             Get the current slider value in set scale range
***************************************************************************************/
int16_t SliderWidget::getSliderPosition(void) {
  return _invert ? _sliderMax - _sliderPos : _sliderPos;
}

/***************************************************************************************
** Function name:           checkTouch
** Description:             Check is touch x, are inside slider box, if so move slider
***************************************************************************************/
bool SliderWidget::checkTouch(uint16_t tx, uint16_t ty)
{
  if (tx >= _sxs && tx <= _sxe && ty >= _sys && ty <= _sye) {
    uint16_t tp, kd;
    if(_horiz) {
      tp = tx - _sxs;
      kd = _kwidth;
    }
    else {
      tp = ty - _sys;
      kd = _kheight;
    }
    int16_t tv = map(tp, _slotWidth/2 + kd/2 + 1, _slotLength - _slotWidth/2 - kd/2 - 1, _sliderMin, _sliderMax);
    tv = constrain( tv, _sliderMin, _sliderMax);
    moveTo(tv);
    return true;
  }
  return false;
}

/***************************************************************************************
** Function name:           drawSlider
** Description:             drawSlider to TFT screen with set parameters
***************************************************************************************/
void SliderWidget::drawSlider(uint16_t x, uint16_t y, slider_t param)
{
  // createSlider
  _slotWidth = param.slotWidth;
  _slotLength = param.slotLength;
  _slotColor = param.slotColor;
  _slotBgColor = param.slotBgColor;
  _horiz = param.orientation;

  // createKnob
  _kwidth = param.knobWidth;
  _kheight = param.knobHeight;
  _kradius = param.knobRadius;
  _kcolor1 = param.knobColor;
  _kcolor2 = param.knobLineColor;

  // setSliderScale
  setSliderScale(param.sliderLT, param.sliderRB);

  _sliderPos = param.startPosition;
  _usdelay = param.sliderDelay;


  _kposPrev = _slotLength/2;

  drawSlider(x, y);
}


/***************************************************************************************
** Function name:           drawSlider
** Description:             drawSlider to TFT screen and set slider position value
***************************************************************************************/
void SliderWidget::drawSlider(uint16_t x, uint16_t y)
{
  if(_horiz) {
    _xpos = x + 2;
    _ypos = y + _kheight/2 + 2;
    //_tft->drawWideLine(_xpos, _ypos, _xpos + _slotLength, _ypos_ypos,_slotWidth, _slotColor, _slotBgColor);
    _tft->fillSmoothRoundRect(_xpos, _ypos - _slotWidth/2, _slotLength, _slotWidth, _slotWidth/2, _slotColor, _slotBgColor);
    _sxs = _xpos - 1;
    _sys = _ypos - _kheight/2 - 1;
    _sxe = _xpos + _slotLength + 2;
    _sye = _ypos + _kheight/2 + 3;
  }
  else  {
    _xpos = x + _kwidth/2 + 2;
    _ypos = y + 2;
    _tft->fillSmoothRoundRect(_xpos - _slotWidth/2, _ypos, _slotWidth, _slotLength, _slotWidth/2, _slotColor, _slotBgColor);
    _sxs = _xpos - _kwidth/2 - 1;
    _sys = _ypos - 1;
    _sxe = _xpos + _kwidth/2 + 3;
    _sye = _ypos + _slotLength + 2;
  }
  uint16_t kd = (_horiz ? _kwidth : _kheight);
  _kposPrev = map(_sliderPos, _sliderMin, _sliderMax, _slotWidth/2 + 1, _slotLength - _slotWidth/2 - kd - 1);
  setSliderPosition(_sliderPos);
}

/***************************************************************************************
** Function name:           moveTo (private fn)
** Description:             Move the slider to a new value in set range
***************************************************************************************/
void SliderWidget::moveTo(int16_t val)
{
  val = constrain( val, _sliderMin, _sliderMax );

  //if (val == _sliderPos) return;

  _sliderPos = val;

  uint16_t kd = (_horiz ? _kwidth : _kheight);
  uint16_t kpos = map(val, _sliderMin, _sliderMax, _slotWidth/2 + 1, _slotLength - _slotWidth/2 - kd - 1);

  _spr->createSprite(_kwidth + 2, _kheight + 2);

  if (_usdelay == 0 && abs(kpos - _kposPrev) > kd + 1) {
    drawKnob(kpos);
    _kposPrev = kpos;
  }
  else {
    int8_t dp = 1;
    if (kpos < _kposPrev) dp = -1;
    _usdelay /= 1000;
    while (kpos != _kposPrev) {_kposPrev += dp;  drawKnob(_kposPrev); sleep_ms(_usdelay);}
  }

  _spr->deleteSprite();
}

/***************************************************************************************
** Function name:           drawKnob (private fn)
** Description:             Draw the slider control knob at pixel kpos position
***************************************************************************************/
void SliderWidget::drawKnob(uint16_t kpos)
{
  uint16_t x, y;

  if(_horiz) {
    x = _xpos + kpos - 1;
    y = _ypos - _kheight/2 - 1;
    _spr->fillRect(0, _kheight/2 - _slotWidth/2 + 1, _kwidth + 2, _slotWidth, _slotColor);
  }
  else  {
    x = _xpos - _kwidth/2 - 1;
    y = _ypos + kpos - 1;
    _spr->fillRect(_kwidth/2 - _slotWidth/2 + 1, 0, _slotWidth, _kheight + 2, _slotColor);
  }

  if (_usdelay == 0 && abs(kpos - _kposPrev) > (_horiz ? _kwidth : _kheight) + 1) {
    if(_horiz) _spr->pushSprite(_xpos + _kposPrev, y);
    else _spr->pushSprite(_xpos, y + _kposPrev);
  }

  // Draw slider outline
  _spr->fillSmoothRoundRect(1, 1, _kwidth, _kheight, _kradius, _kcolor1, _slotBgColor);

  // Draw marker stripe
  if(_kcolor1 != _kcolor2) {
    if(_horiz) {
      _spr->drawFastVLine(_kwidth/2 + 1, 1, _kheight, _kcolor2);
    }
    else  {
      _spr->drawFastHLine(1, _kheight/2 + 1, _kwidth, _kcolor2);
    }
  }
  _spr->pushSprite(x,y);
}

/***************************************************************************************
** Function name:           getBoundingBox
** Description:             Return the bounding box as coordinates
***************************************************************************************/
void SliderWidget::getBoundingBox(int16_t *xs, int16_t *ys, int16_t *xe, int16_t *ye)
{
  // Bounds already corrected for Sprite wipe action
  *xs = _sxs;
  *ys = _sys;
  *xe = _sxe;
  *ye = _sye;
}


/***************************************************************************************
** Function name:           getBoundingRect
** Description:             Return outside bounding box rectangle x,y and width,height
***************************************************************************************/
void SliderWidget::getBoundingRect(int16_t *x, int16_t *y, uint16_t *w, uint16_t *h)
{
  // Corrected to be outside slider draw zone
  *x = _sxs - 1;
  *y = _sys - 1;
  *w = _sxe - _sxs + 1;
  *h = _sye - _sys + 1;
}

GraphWidget::GraphWidget(TFTLIB_8BIT *tft)
{
  _tft = tft;
}

/***************************************************************************************
** Function name:           createGraph
** Description:             Create graph with parameters
***************************************************************************************/
bool GraphWidget::createGraph(uint16_t graphWidth, uint16_t graphHeight, uint16_t bgColor)
{
  _width   = graphWidth;
  _height  = graphHeight;
  _bgColor = bgColor;

  return 0;
}

/***************************************************************************************
** Function name:           setGraphScale
** Description:             Set slider scale range with movement delay in us per pixel
***************************************************************************************/
void GraphWidget::setGraphScale(float xmin, float xmax, float ymin, float ymax)
{
  _xMin = xmin;
  _xMax = xmax;
  _yMin = ymin;
  _yMax = ymax;
}

/***************************************************************************************
** Function name:           setGraphScale
** Description:             Set slider scale range with movement delay in us per pixel
***************************************************************************************/
void GraphWidget::setGraphGrid(float xsval, float xinc, float ysval, float yinc, uint16_t gridColor)
{
  _xGridStart = xsval;
  _xGridInc   = xinc;
  _yGridStart = ysval;
  _yGridInc   = yinc;
  _gridColor  = gridColor;
}

/***************************************************************************************
** Function name:           drawGraph
** Description:             drawGraph to TFT screen with set parameters
***************************************************************************************/
void GraphWidget::drawGraph(uint16_t x, uint16_t y)
{
  _tft->fillRect(x, y, _width, _height, _bgColor);

  for (float px = _xGridStart; px <= _xMax; px += _xGridInc)
  {
    uint16_t gx = map(px, _xMin, _xMax, 0, _width);
    _tft->drawFastVLine(x + gx, y, _height, _gridColor);
  }

  for (float py = _yGridStart; py <= _yMax; py += _yGridInc)
  {
    uint16_t gy = map(py, _yMin, _yMax, _height, 0);
    _tft->drawFastHLine(x, y + gy, _width, _gridColor);
  }

  _xpos = x;
  _ypos = y;
}

/***************************************************************************************
** Function name:           setGraphPosition
** Description:             Set the graph top left corner position without drawing it
***************************************************************************************/
void GraphWidget::setGraphPosition(uint16_t x, uint16_t y)
{
  _xpos = x;
  _ypos = y;
}

/***************************************************************************************
** Function name:           getGraphPosition
** Description:             Get the graph top left corner position
***************************************************************************************/
void GraphWidget::getGraphPosition(uint16_t *x, uint16_t *y)
{
  *x = _xpos;
  *y = _ypos;
}

/***************************************************************************************
** Function name:           getBoundingBox
** Description:             Return the bounding box as coordinates
***************************************************************************************/
void GraphWidget::getBoundingBox(int16_t *xs, int16_t *ys, int16_t *xe, int16_t *ye)
{
  // Bounds already corrected for Sprite wipe action
  *xs = _xpos;
  *ys = _ypos;
  *xe = _xpos + _width;
  *ye = _ypos + _height;
}

/***************************************************************************************
** Function name:           getBoundingRect
** Description:             Return outside bounding box rectangle x,y and width,height
***************************************************************************************/
void GraphWidget::getBoundingRect(int16_t *x, int16_t *y, uint16_t *w, uint16_t *h)
{
  *x = _xpos;
  *y = _ypos;
  *w = _width;
  *h = _height;
}

/***************************************************************************************
** Function name:           getPointX
** Description:             Get x pixel coordinates of a position on display
***************************************************************************************/
int16_t GraphWidget::getPointX(float xval)
{
  int16_t gx = 0.5 + mapFloat(xval, _xMin, _xMax, 0, _width);

  return _xpos + gx;
}

/***************************************************************************************
** Function name:           getPointY
** Description:             Get y pixel coordinates of a position on display
***************************************************************************************/
int16_t GraphWidget::getPointY(float yval)
{
  int16_t gy = 0.5 + mapFloat(yval, _yMin, _yMax, _height, 0);

  return _ypos + gy;
}

/***************************************************************************************
** Function name:           addLine
** Description:             Add new line segment on graph
***************************************************************************************/
bool GraphWidget::addLine(float xs, float ys, float xe, float ye, uint16_t col)
{
  if (clipTrace(&xs, &ys, &xe, &ye))
  {
    // All or part of line is in graph area
    int16_t cxs = _xpos + 0.5 + mapFloat(xs, _xMin, _xMax, 0, _width);
    int16_t cys = _ypos + 0.5 + mapFloat(ys, _yMin, _yMax, _height, 0);
    int16_t cxe = _xpos + 0.5 + mapFloat(xe, _xMin, _xMax, 0, _width);
    int16_t cye = _ypos + 0.5 + mapFloat(ye, _yMin, _yMax, _height, 0);
    _tft->drawLine(cxs, cys, cxe, cye, col);
    return true;
  }

  // No part of line was in graph area
  return false;
}


/***************************************************************************************
** Function name:           regionCode
** Description:             Compute region code for a point(x, y)
***************************************************************************************/
uint16_t GraphWidget::regionCode(float x, float y)
{
	// Assume inside
	int code = INSIDE;

	if (x < _xMin) // to the left of rectangle
		code |= LEFT;
	else if (x > _xMax) // to the right of rectangle
		code |= RIGHT;
	if (y < _yMin) // below the rectangle
		code |= BOTTOM;
	else if (y > _yMax) // above the rectangle
		code |= TOP;
	return code;
}

/***************************************************************************************
** Function name:           clipTrace
** Description:             Implement Cohen-Sutherland clipping algorithm
***************************************************************************************/
// https://en.wikipedia.org/wiki/Cohen%E2%80%93Sutherland_algorithm
bool GraphWidget::clipTrace(float *xs, float *ys, float *xe, float *ye)
{
  float x1 = *xs;
  float y1 = *ys;
  float x2 = *xe;
  float y2 = *ye;

	// Compute region codes
	uint16_t code1 = regionCode(x1, y1);
	uint16_t code2 = regionCode(x2, y2);
	bool inside = false;

	while (true) {
		if ((code1 == 0) && (code2 == 0)) {
			// bitwise OR is 0: both points inside window; trivially accept and exit loop
			inside = true;
			break;
		}
		else if (code1 & code2) {
			// bitwise AND is not 0: both points share an outside zone (LEFT, RIGHT, TOP,
			// or BOTTOM), so both must be outside window; exit loop (accept is false)
			break;
		}
		else {
			// failed both tests, so calculate the line segment to clip
			// from an outside point to an intersection with clip edge
			uint16_t code_out;
			float x = 0, y = 0;

			// At least one endpoint is outside the clip rectangle; pick it.
			if (code1 != 0)
				code_out = code1;
			else
				code_out = code2;

			// Now find the intersection point;
			// use formulas:
			//   slope = (y1 - y0) / (x1 - x0)
			//   x = x0 + (1 / slope) * (ym - y0), where ym is ymin or ymax
			//   y = y0 + slope * (xm - x0), where xm is xmin or xmax
			// No need to worry about divide-by-zero because, in each case, the
			// code_out bit being tested guarantees the denominator is non-zero
			if (code_out & TOP) {
				// point is above the clip rectangle
				x = x1 + (x2 - x1) * (_yMax - y1) / (y2 - y1);
				y = _yMax;
			}
			else if (code_out & BOTTOM) {
				// point is below the rectangle
				x = x1 + (x2 - x1) * (_yMin - y1) / (y2 - y1);
				y = _yMin;
			}
			else if (code_out & RIGHT) {
				// point is to the right of rectangle
				y = y1 + (y2 - y1) * (_xMax - x1) / (x2 - x1);
				x = _xMax;
			}
			else if (code_out & LEFT) {
				// point is to the left of rectangle
				y = y1 + (y2 - y1) * (_xMin - x1) / (x2 - x1);
				x = _xMin;
			}

			// Now we move outside point to intersection point to clip
			// and get ready for next pass.
			if (code_out == code1) {
				x1 = x;
				y1 = y;
				code1 = regionCode(x1, y1);
			}
			else {
				x2 = x;
				y2 = y;
				code2 = regionCode(x2, y2);
			}
		}
	}

  // Return the clipped coordinates
  if (inside)
  {
    *xs = x1;
    *ys = y1;
    *xe = x2;
    *ye = y2;
  }

  // Return true if a line segment need to be drawn in rectangle
  return inside;
}

TraceWidget::TraceWidget(GraphWidget *gw) {
  _gw = gw;
}

/***************************************************************************************
** Function name:           startTrace
** Description:             Start a new trace
***************************************************************************************/
void TraceWidget::startTrace(uint16_t ptColor)
{
  _newTrace = true;
  _ptColor = ptColor;
  _xpt = 0;
  _ypt = 0;
}

/***************************************************************************************
** Function name:           addPoint
** Description:             Add new point on graph
***************************************************************************************/
bool TraceWidget::addPoint(float xval, float yval)
{
  // Map point to display pixel coordinate
  _xpt = 0.5 + _gw->getPointX(xval);
  _ypt = 0.5 + _gw->getPointY(yval);

  // If it is a new trace then a single pixel is drawn
  if (_newTrace)
  {
    _newTrace = false;
    _xval = xval;
    _yval = yval;
  }

  // Draw the line segment on graph (gets clipped to graph area)
  bool updated = _gw->addLine(_xval, _yval, xval, yval, _ptColor);

  // Store last graph scale point for this trace
  _xval = xval;
  _yval = yval;

  // Returns true if part of resulting line drawn in graph area
  return updated;
}

/***************************************************************************************
** Function name:           getLastPointX
** Description:             Get x pixel coordinates of last point plotted
***************************************************************************************/
uint16_t TraceWidget::getLastPointX(void)
{
  return _xpt;
}

/***************************************************************************************
** Function name:           getLastPointX
** Description:             Get y pixel coordinates of last point plotted
***************************************************************************************/
uint16_t TraceWidget::getLastPointY(void)
{
  return _ypt;
}

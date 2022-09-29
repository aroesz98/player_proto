/*
 * TFTLIB_8BIT.h
 *
 *  Created on: Jan 10, 2022
 *  Updated on: Feb 20, 2022
 *      Author: asz
 */
#pragma GCC push_options
#pragma GCC optimize ("O3")

#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "pico/stdlib.h"
#include "pico/float.h"

#include "algorithm"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Print.h"
#include "GFXFF/gfxfont.h"

enum COLORS{
	WHITE		= 0xFFFF,
	BLACK		= 0x0000,
	GRAY		= 0X8430,
	LIGHTGREY	= 0xD69A,
	RED			= 0xF800,
	GREEN		= 0x07E0,
	BLUE		= 0x001F,
	LIGHTBLUE	= 0X7D7C,
	SKYBLUE		= 0x867D,
	YELLOW		= 0xFFE0,
	PURPLE		= 0x780F,
	MAGENTA		= 0xF81F,
	VIOLET		= 0x915C,
	CYAN		= 0x7FFF,
	DARKCYAN	= 0x03EF,
	ORANGE		= 0xFB20,
	PINK		= 0xFE19,
	BROWN		= 0x9A60,
	GOLD		= 0xFEA0,
	SILVER		= 0xC618,
	BRED		= 0XF81F,
	GRED		= 0XFFE0,
	GBLUE		= 0X07FF,
	BRRED		= 0XFC07,
	GRAYBLUE	= 0X5458,
	LGRAY		= 0XC618,
	LGRAYBLUE	= 0XA651,
	LBBLUE		= 0X2B12,
	NAVY		= 0x000F,
	MAROON		= 0x7800,
	OLIVE		= 0x7BE0,
	GREENYELLOW	= 0xB7E0,

	DKBLUE		= 0x000D,
	DKTEAL		= 0x020C,
	DKGREEN		= 0x03E0,
	DKCYAN		= 0x03EF,
	DKRED		= 0x6000,
	DKMAGENTA	= 0x8008,
	DKYELLOW	= 0x8400,
	DKORANGE 	= 0x8200,
	DKPINK		= 0x9009,
	DKPURPLE	= 0x4010,
	DKGREY		= 0x4A49,
};

enum DATUM {
	TL_DATUM = 0, // Top left (default)
	TC_DATUM = 1, // Top centre
	TR_DATUM = 2, // Top right
	ML_DATUM = 3, // Middle left
	CL_DATUM = 3, // Centre left, same as above
	MC_DATUM = 4, // Middle centre
	CC_DATUM = 4, // Centre centre, same as above
	MR_DATUM = 5, // Middle right
	CR_DATUM = 5, // Centre right, same as above
	BL_DATUM = 6, // Bottom left
	BC_DATUM = 7, // Bottom centre
	BR_DATUM = 8, // Bottom right
	L_BASELINE = 9, // Left character baseline (Line the 'A' character would sit on)
	C_BASELINE = 10, // Centre character baseline
	R_BASELINE = 11, // Right character baseline
};

/* Control Registers and constant codes */
#define SWRESET			0x01
#define CASET			0x2A
#define RASET			0x2B
#define RAMWR			0x2C
#define RAMRD			0x2E

#define SLPIN			0x10
#define SLPOUT			0x11
#define DISPOFF			0x28
#define DISPON			0x29
#define FRMCTR1			0xB1
#define FRMCTR2			0xB2
#define PWCTR1			0xC0
#define PWCTR2			0xC1
#define PWCTR3			0xC2
#define PWCTR4			0xC3
#define PWCTR5			0xC4
#define VMCTR1			0xC5
#define VMCTR2			0xC7

#define COLMOD			0x3A
#define COLOR_MODE_16bit 0x55
#define COLOR_MODE_18bit 0x66

#define MADCTL			0x36
#define MADCTL_MH		0x04
#define MADCTL_ML		0x10
#define MADCTL_MV		0x20
#define MADCTL_MX		0x40
#define MADCTL_MY		0x80
#define MADCTL_RGB		0x00
#define MADCTL_BGR		0x08

#define Byte8H(ByteH) ((uint8_t)(((uint16_t)(ByteH)&0xFF00)>>8))
#define Byte8L(ByteL) ((uint8_t)( (uint16_t)(ByteL)&0x00FF))

/* Basic operations */
template <typename T> static inline void
swap_coord(T& a, T& b) { T t = a; a = b; b = t;}

extern "C" char* sbrk(int incr);

constexpr float PixelAlphaGain   = 255.0;
constexpr float LoAlphaTheshold  = 64.0/255.0;
constexpr float HiAlphaTheshold  = 1.0 - LoAlphaTheshold;

inline GFXglyph *pgm_read_glyph_ptr(const GFXfont *gfxFont, uint8_t c) {
	return gfxFont->glyph + c;
}

inline uint8_t *pgm_read_bitmap_ptr(const GFXfont *gfxFont) {
	return gfxFont->bitmap;
}

typedef enum {
	ILI9341_PARALLEL	= 0x01,
	ILI9327_PARALLEL	= 0x02,
	NT35510_PARALLEL	= 0x04,
} LCD_DRIVER;

static const uint16_t default_4bit_palette[] = {
	BLACK,    //  0  ^
	BROWN,    //  1  |
	RED,      //  2  |
	ORANGE,   //  3  |
	YELLOW,   //  4  Colours 0-9 follow the resistor colour code!
	GREEN,    //  5  |
	BLUE,     //  6  |
	PURPLE,   //  7  |
	DKGREY, //  8  |
	WHITE,    //  9  v
	CYAN,     // 10  Blue+green mix
	MAGENTA,  // 11  Blue+red mix
	MAROON,   // 12  Darker red colour
	DKGREEN,// 13  Darker green colour
	NAVY,     // 14  Darker blue colour
	PINK      // 15
};

class TFTLIB_8BIT : public Print {
	friend class TFTLIB_Sprites;
	friend class ButtonWidget;
	friend class SliderWidget;
	friend class GraphWidget;

	private:
		uint8_t _rotation;
		int32_t _tx0, _tx1, _ty0, _ty1;
		uint32_t CS_PIN;
		uint32_t DC_PIN;
		uint32_t WR_PIN;
		uint32_t RD_PIN;
		uint32_t RST_PIN;
		uint32_t port_mask;
		LCD_DRIVER _type;
		uint16_t _buffer_size = 2048;
		uint16_t *_buffer = new uint16_t[_buffer_size];
		GFXfont  *gfxFont;
		uint16_t _text_fg = RED, _text_bg = BLACK;
		uint16_t SWAP_UINT16(uint16_t x) {x = (x >> 8) | (x << 8); return x;}

		uint8_t  decoderState = 0;   // UTF8 decoder state        - not for user access
		uint16_t decoderBuffer;      // Unicode code-point buffer - not for user access

		uint8_t glyph_ab,   // Smooth font glyph delta Y (height) above baseline
				glyph_bb;   // Smooth font glyph delta Y (height) below baseline

		bool     _utf8;
		bool     isDigits;   // adjust bounding box for numbers to reduce visual jiggling
		bool     textwrapX, textwrapY;  // If set, 'wrap' text at right and optionally bottom edge of display

		int32_t  cursor_x = 0, cursor_y = 0, padX = 0;       // Text cursor x,y and padding setting
		uint32_t textcolor, textbgcolor;         // Text foreground and background colours

		uint8_t textfont  = 1,  // Current selected font number
				textsize  = 1,  // Current font size multiplier
				textdatum = TL_DATUM; // Text reference datum

		int32_t _display_width  = 480;
		int32_t _display_height = 800;
		int32_t _width  = 480;
		int32_t _height = 800;

		bool     _vpOoB;
		uint32_t _lastColor;
		bool _locked = false;

		int16_t textWidth(const char *string);
		int16_t textWidth(const char *string, uint8_t font);
		int16_t fontHeight(void);
		int16_t fontHeight(int16_t font);
		uint16_t decodeUTF8(uint8_t c);
		uint16_t decodeUTF8(uint8_t *buf, uint16_t *index, uint16_t remaining);

		/*void CS_L(void) {  sio_hw->gpio_clr = (1 << CS_PIN); }
		void CS_H(void) {  sio_hw->gpio_set = (1 << CS_PIN); }

		void DC_L(void) {  sio_hw->gpio_clr = (1 << DC_PIN); }
		void DC_H(void) {  sio_hw->gpio_set = (1 << DC_PIN); }*/

		void WR_L(void) {  sio_hw->gpio_clr = (1 << WR_PIN); }
		void WR_H(void) {  sio_hw->gpio_set = (1 << WR_PIN); }

		void RD_L(void) {  sio_hw->gpio_clr = (1 << RD_PIN); }
		void RD_H(void) {  sio_hw->gpio_clr = (1 << RD_PIN); }

		void WR_STROBE(void) { WR_L(); WR_L(); WR_H(); }
		void RD_STROBE(void) { RD_L(); RD_L(); RD_L(); }
		void RD_IDLE(void)	 { RD_H(); RD_H(); RD_H(); RD_H(); RD_H(); RD_H(); }

	public:
		uint32_t bitmap_fg, bitmap_bg;
		int16_t  _xPivot;   // TFT x pivot point coordinate for rotated Sprites
		int16_t  _yPivot;   // TFT x pivot point coordinate for rotated Sprites
		int32_t  _vpX, _vpY, _vpW, _vpH;    // Note: x start, y start, x end + 1, y end + 1
		TFTLIB_8BIT(LCD_DRIVER drv = NT35510_PARALLEL, uint32_t GPIO_RD_PIN = 0, uint32_t GPIO_WR_PIN = 0, uint32_t GPIO_DC_PIN = 0, uint32_t GPIO_CS_PIN = 0, uint32_t GPIO_RST_PIN = 0);
		~TFTLIB_8BIT();

		void CS_L(void) {  sio_hw->gpio_clr = (1 << CS_PIN); }
		void CS_H(void) {  sio_hw->gpio_set = (1 << CS_PIN); }

		void DC_L(void) {  sio_hw->gpio_clr = (1 << DC_PIN); }
		void DC_H(void) {  sio_hw->gpio_set = (1 << DC_PIN); }

		uint8_t readByte(void);
		bool isLocked(void) { return _locked; };

		inline void write8(uint8_t data) {	sio_hw->gpio_clr = (1 << 0x3FC); sio_hw->gpio_togl = (sio_hw->gpio_out ^ (data << 2)) & 0x3FC; WR_STROBE(); }
		inline void write16(uint16_t data) { write8(data >> 8); write8(data); }

		void writeCommand8 (uint8_t);
		void writeCommand16 (uint16_t);
		void writeData8(uint8_t *data, uint32_t len);
		void writeData16(uint16_t *data, uint32_t len);
		inline void writeSmallData8 (uint8_t);
		inline void writeSmallData16 (uint16_t);
		inline void writeSmallData32 (uint32_t);

		void setWindow8(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
		void readWindow8(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
		void pushPixels8(const void* data_in, uint32_t len);
		void pushPixels16(const void* data_in, uint32_t len);
		void pushBlock16(uint16_t color, uint32_t len);

		void init(void);
		void setRotation(uint8_t m);
		uint16_t readID(void);
		int16_t width(void);
		int16_t height(void);

		void fillScreen(uint16_t color);

		uint16_t		readPixel(int32_t x0, int32_t y0);
		virtual void	drawPixel(int32_t x, int32_t y, uint16_t color),
						drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color),
						drawFastHLine(int32_t x, int32_t y, int32_t w, uint16_t color),
						drawFastVLine(int32_t x, int32_t y, int32_t h, uint16_t color),
						fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color);

		uint16_t	drawPixel(int32_t x, int32_t y, uint32_t color, uint8_t alpha, uint32_t bg_color = 0x00FFFFFF);
		void		drawPixelAlpha(int16_t x, int16_t y, uint16_t color, uint8_t alpha);
		void		drawSpot(float ax, float ay, float r, uint32_t color);
		void		fillSmoothCircle(int32_t x, int32_t y, int32_t r, uint32_t color, uint32_t bg_color = 0x00FFFFFF);
		void		fillSmoothRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t radius, uint32_t color, uint32_t bg_color = 0x00FFFFFF);
		void fillRectVGradient(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color1, uint32_t color2);
		void fillRectHGradient(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color1, uint32_t color2);

		inline float wedgeLineDistance(float xpax, float ypay, float bax, float bay, float dr);
		inline void drawCircleHelper( int32_t x0, int32_t y0, int32_t rr, uint8_t cornername, uint16_t color);
		inline void fillCircleHelper(int32_t x0, int32_t y0, int32_t r, uint8_t cornername, int32_t delta, uint16_t color);
		inline void fillCircleHelperAA(int32_t x0, int32_t y0, int32_t r, uint8_t cornername, int32_t delta, uint16_t color);
		inline void fillAlphaCircleHelper(int32_t x0, int32_t y0, int32_t r, uint8_t cornername, int32_t delta, uint16_t color, uint8_t alpha);

		//void drawFastHLine(int32_t x, int32_t y, int32_t w, uint16_t color);
		void drawHLineAlpha(int32_t x, int32_t y, int32_t w, uint16_t color, uint8_t alpha);
		//void drawFastVLine(int32_t x, int32_t y, int32_t w, uint16_t color);
		void drawVLineAlpha(int32_t x, int32_t y, int32_t h, uint16_t color, uint8_t alpha);
		//void drawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color);

		void drawWideLine(float ax, float ay, float bx, float by, float wd, uint16_t fg_color, uint16_t bg_color = 0xFFFF);
		void drawWedgeLine(float ax, float ay, float bx, float by, float ar, float br, uint32_t fg_color, uint32_t bg_color = 0x00FFFFFF);

		void drawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, uint16_t color);
		void drawTriangleAA(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, int32_t thickness, uint16_t color);

		void drawRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color);
		void drawRectAA(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color);

		void drawRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color);

		void drawCircle(int32_t x0, int32_t y0, int32_t r, uint16_t color);

		void drawEllipse(int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color);

		/* Text functions. */
		uint8_t getTextDatum(void);
		void getCursor(int16_t* x, int16_t* y);
		uint16_t getTextPadding(void);

		void setCursor(int16_t x, int16_t y);
		void setTextColor(uint32_t c, uint32_t b = 0xFFFFFFFF);
		void setTextWrap(bool wrapX, bool wrapY = false);
		void setTextSize(uint8_t s);
		void setFreeFont(const GFXfont *f);
		void setTextDatum(uint8_t d);
		void setTextPadding(uint16_t x_width);

		size_t write(uint8_t utf8);
		int16_t drawChar(uint16_t uniCode, int32_t x, int32_t y);
		void drawChar(int32_t x, int32_t y, uint16_t c, uint32_t color, uint32_t bg, uint8_t size);

		int16_t drawString(const char *string, int32_t poX, int32_t poY);
		int16_t drawString(const String& string, int32_t poX, int32_t poY);
		int16_t drawCentreString(const char *string, int32_t dX, int32_t poY);
		int16_t drawCentreString(const String& string, int32_t dX, int32_t poY);
		int16_t drawRightString(const char *string, int32_t dX, int32_t poY);
		int16_t drawRightString(const String& string, int32_t dX, int32_t poY);

		int16_t drawNumber(long long_num, int32_t poX, int32_t poY);
		int16_t drawFloat(float floatNumber, uint8_t dp, int32_t poX, int32_t poY);

		/* Extented Graphical functions. */
		void fillTriangle(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color);
		void fillTriangleAA( int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color);
		void fillTriangleAlpha(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t x2, int32_t y2, uint16_t color, uint8_t alpha);

		//void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color);
		void fillRectAA(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color);
		void fillRectAlpha(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color, uint8_t alpha);

		void fillRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color);
		void fillRoundRectAA(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color);
		void fillAlphaRoundRect(int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, uint16_t color, uint8_t alpha);

		virtual void fillCircle(int32_t x0, int32_t y0, int32_t r, uint16_t color);
		void fillCircleAA(float x, float y, float r, uint16_t color);
		void fillCircleAlpha(int32_t x0, int32_t y0, int32_t r, uint16_t color, uint8_t alpha);

		void fillEllipse(int16_t x0, int16_t y0, int32_t rx, int32_t ry, uint16_t color);

		void drawImage(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t *data);
		void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t  *data, bool bpp8 = true, uint16_t *cmap = nullptr);
		void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t *data, uint16_t transp);
		void pushImage(int32_t x, int32_t y, int32_t w, int32_t h, uint8_t  *data, uint8_t  transparent, bool bpp8 = true, uint16_t *cmap = nullptr);
		void drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t *bitmap, uint16_t color);
		void drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t *bitmap, uint16_t color);
		void drawBitmap(int16_t x, int16_t y, int16_t w, int16_t h, const uint8_t *bitmap, uint16_t fgcolor, uint16_t bgcolor);
        void drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t fgcolor);
        void drawXBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t fgcolor, uint16_t bgcolor);

		uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
		uint16_t color16to8(uint16_t c);
		uint16_t color8to16(uint8_t color);
		uint16_t alphaBlend(uint8_t alpha, uint16_t fgc, uint16_t bgc);

		uint32_t testFillScreen();
		uint32_t testText();
		uint32_t testLines(uint16_t color);
		uint32_t testFastLines(uint16_t color1, uint16_t color2);
		uint32_t testRects(uint16_t color);
		uint32_t testFilledRects(uint16_t color1, uint16_t color2);
		uint32_t testFilledCircles(uint8_t radius, uint16_t color);
		uint32_t testCircles(uint8_t radius, uint16_t color);
		uint32_t testTriangles();
		uint32_t testFilledTriangles();
		uint32_t testRoundRects();
		uint32_t testFilledRoundRects();
		void benchmark(void);
		void cpuConfig(void);
		int FreeRAM();
};

typedef void (*actionCallback)(void);
static void dummyButtonAction(void) { }; // In case user callback is not defined!

class ButtonWidget {

	private:
		TFTLIB_8BIT *_tft;
		int16_t  _x1, _y1;              // Coordinates of top-left corner of button
		int16_t  _xd, _yd;              // Button text datum offsets (wrt centre of button)
		uint16_t _w, _h;                // Width and height of button
		uint8_t  _textsize, _textdatum; // Text size multiplier and text datum for button
		uint16_t _outlinecolor, _fillcolor, _textcolor, _outlinewidth, _bgcolor;
		char     _label[10]; // Button text is 9 chars maximum unless long_name used
		uint32_t _pressTime, _releaseTime;
		bool  _inverted, _currstate, _laststate; // Button states

	public:

		ButtonWidget(TFTLIB_8BIT* disp);

		// "Classic" initButton() uses centre & size
		void     initButton(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t textcolor, char *label, uint8_t textsize);

		// New/alt initButton() uses upper-left corner & size
		void     initButtonUL(int16_t x1, int16_t y1, uint16_t w, uint16_t h, uint16_t outline, uint16_t fill, uint16_t textcolor, char *label, uint8_t textsize);

		// Adjust text datum and x, y deltas
		void     setLabelDatum(int16_t x_delta, int16_t y_delta, uint8_t datum = MC_DATUM);

		actionCallback pressAction   = dummyButtonAction; // Press   action callback
		actionCallback releaseAction = dummyButtonAction; // Release action callback

		void     setPressAction(actionCallback action);
		void     setReleaseAction(actionCallback action);

		void     setPressTime(uint32_t pressTime)    { _pressTime  = pressTime; }
		void     setReleaseTime(uint32_t releaseTime){ _releaseTime = releaseTime; }
		uint32_t getPressTime(void)  { return _pressTime; }
		uint32_t getReleaseTime(void){ return _releaseTime; }



		void     drawButton(bool inverted = false, String long_name = "");
		void     drawSmoothButton(bool inverted = false, int16_t outlinewidth = -1, uint32_t bgcolor = 0x00FFFFFF, String long_name = "");
		bool     contains(int16_t x, int16_t y);

		void     press(bool p);
		bool     isPressed();
		bool     justPressed();
		bool     justReleased();

		bool     getState(void) {return _inverted;}        // Get inverted state, true = inverted
};

class TFTLIB_Sprites : public TFTLIB_8BIT {

private:
	TFTLIB_8BIT *_lcd;
	void*    callocSprite(int16_t width, int16_t height, uint8_t frames = 1);

protected:
	uint8_t  _bpp;     // bits per pixel (1, 4, 8 or 16)
	uint16_t *_img;    // pointer to 16 bit sprite
	uint8_t  *_img8;   // pointer to  1 and 8 bit sprite frame 1 or frame 2
	uint8_t  *_img4;   // pointer to  4 bit sprite (uses color map)
	uint8_t  *_img8_1; // pointer to frame 1
	uint8_t  *_img8_2; // pointer to frame 2

	uint16_t *_colorMap; // color map pointer: 16 entries, used with 4 bit color map.

	int32_t  _sinra;   // Sine of rotation angle in fixed point
	int32_t  _cosra;   // Cosine of rotation angle in fixed point

	bool     _created; // A Sprite has been created and memory reserved

	int32_t  _xs, _ys, _xe, _ye, _xptr, _yptr; // for setWindow
	int32_t  _sx, _sy; // x,y for scroll zone
	uint32_t _sw, _sh; // w,h for scroll zone
	uint32_t _scolor;  // gap fill colour for scroll zone

	int32_t  _iwidth, _iheight; // Sprite memory image bit width and height (swapped during rotations)
	int32_t  _dwidth, _dheight; // Real sprite width and height (for <8bpp Sprites)
	int32_t  _bitwidth;         // Sprite image bit width for drawPixel (for <8bpp Sprites, not swapped)
	uint32_t bitmap_fg, bitmap_bg;           // Bitmap foreground (bit=1) and background (bit=0) colours
	int16_t  _xPivot;   // TFT x pivot point coordinate for rotated Sprites
	int16_t  _yPivot;   // TFT x pivot point coordinate for rotated Sprites

	// Viewport variables
	int32_t  _vpX, _vpY, _vpW, _vpH;    // Note: x start, y start, x end + 1, y end + 1
	int32_t  _xDatum = 0;
	int32_t  _yDatum = 0;
	int32_t  _xWidth = 0;
	int32_t  _yHeight = 0;
	bool     _vpDatum = 0;
	bool     _vpOoB = 0;
	uint8_t rotation = 3;

	public:
		explicit TFTLIB_Sprites(TFTLIB_8BIT* displ);
		~TFTLIB_Sprites(void);

		// Create a sprite of width x height pixels, return a pointer to the RAM area
		// Sketch can cast returned value to (uint16_t*) for 16 bit depth if needed
		void*    createSprite(int16_t width, int16_t height, uint8_t frames = 1);

		// Returns a pointer to the sprite or nullptr if not created, user must cast to pointer type
		void*    getPointer(void);

		// Returns true if sprite has been created
		bool     created(void);

		// Delete the sprite to free up the RAM
		void     deleteSprite(void);

		// Select the frame buffer for graphics write (for 2 colour ePaper and DMA toggle buffer)
		// Returns a pointer to the Sprite frame buffer
		void*    frameBuffer(int8_t f);

		// Set or get the colour depth to 1, 4, 8 or 16 bits. Can be used to change depth an existing
		// sprite, but clears it to black, returns a new pointer if sprite is re-created.
		void*    setColorDepth(int8_t b);
		int8_t   getColorDepth(void);

		// Set the palette for a 4 bit depth sprite.  Only the first 16 colours in the map are used.
		void     createPalette(uint16_t *palette = nullptr, uint8_t colors = 16);       // Palette in RAM
		void     createPalette(const uint16_t *palette = nullptr, uint8_t colors = 16); // Palette in FLASH

		// Set a single palette index to the given color
		void     setPaletteColor(uint8_t index, uint16_t color);

		// Get the color at the given palette index
		uint16_t getPaletteColor(uint8_t index);

		// Set foreground and background colours for 1 bit per pixel Sprite
		void     setBitmapColor(uint16_t fg, uint16_t bg);

        // Read the colour of a pixel at x,y and return value in 565 format
		uint16_t readPixel(int32_t x0, int32_t y0);

        // return the numerical value of the pixel at x,y (used when scrolling)
		uint16_t readPixelValue(int32_t x, int32_t y);

		// Draw a single pixel at x,y
		void     drawPixel(int32_t x, int32_t y, uint32_t color);

		// Fill Sprite with a colour
		void fillSprite(uint32_t color),

		// Define a window to push 16 bit colour pixels into in a raster order
		// Colours are converted to the set Sprite colour bit depth
		setWindow(int32_t x0, int32_t y0, int32_t x1, int32_t y1),

		// Push a color (aka singe pixel) to the sprite's set window area
		pushColor(uint16_t color),

		// Push len colors (pixels) to the sprite's set window area
		pushColor(uint16_t color, uint32_t len),

		// Push a pixel pre-formatted as a 1, 4, 8 or 16 bit colour (avoids conversion overhead)
		writeColor(uint16_t color),

		// Set the scroll zone, top left corner at x,y with defined width and height
		setScrollRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color = BLACK),

		// Scroll the defined zone dx,dy pixels. Negative values left,up, positive right,down
		scroll(int16_t dx, int16_t dy = 0),

		// Draw lines
		drawLine(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color),
		drawFastVLine(int32_t x, int32_t y, int32_t h, uint32_t color),
		drawFastHLine(int32_t x, int32_t y, int32_t w, uint32_t color),
		fillCircle(int32_t x0, int32_t y0, int32_t r, uint16_t color),

		// Fill a rectangular area with a color (aka draw a filled rectangle)
		fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);

		// Set the coordinate rotation of the Sprite (for 1bpp Sprites only)
		void	setRotation(uint8_t rotation);
		uint8_t getRotation(void);

		// Push a rotated copy of Sprite to TFT with optional transparent colour
		bool	pushRotated(int16_t angle, uint32_t transp = 0x00FFFFFF);

		// Push a rotated copy of Sprite to another different Sprite with optional transparent colour
		bool	pushRotated(TFTLIB_Sprites *spr, int16_t angle, uint32_t transp = 0x00FFFFFF);

		// Get the TFT bounding box for a rotated copy of this Sprite
		bool	getRotatedBounds(int16_t angle, int16_t *min_x, int16_t *min_y, int16_t *max_x, int16_t *max_y);

		// Get the destination Sprite bounding box for a rotated copy of this Sprite
		bool	getRotatedBounds(TFTLIB_Sprites *spr, int16_t angle, int16_t *min_x, int16_t *min_y, int16_t *max_x, int16_t *max_y);

		// Bounding box support function
		void     getRotatedBounds(int16_t angle, int16_t w, int16_t h, int16_t xp, int16_t yp,
		int16_t *min_x, int16_t *min_y, int16_t *max_x, int16_t *max_y);

		// Write an image (colour bitmap) to the sprite.
		void     pushImage(int32_t x0, int32_t y0, int32_t w, int32_t h, uint16_t *data, uint8_t sbpp = 0);
		void     pushImage(int32_t x0, int32_t y0, int32_t w, int32_t h, const uint16_t *data);

		// Push the sprite to the TFT screen, this fn calls pushImage() in the TFTLIB_8BIT class.
		void     pushSprite(int32_t x, int32_t y);
		void     pushSprite(int32_t x, int32_t y, uint16_t transparent);

		// Push a windowed area of the sprite to the TFT at tx, ty
		bool     pushSprite(int32_t tx, int32_t ty, int32_t sx, int32_t sy, int32_t sw, int32_t sh);

		// Push the sprite to another sprite at x,y. This fn calls pushImage() in the destination sprite (dspr) class.
		bool     pushToSprite(TFTLIB_Sprites *dspr, int32_t x, int32_t y);
		bool     pushToSprite(TFTLIB_Sprites *dspr, int32_t x, int32_t y, uint16_t transparent);

		// Return the width and height of the sprite
		int16_t width(void);
		int16_t height(void);

		void	setPivot(int16_t x, int16_t y);
		int16_t	getPivotX(void), // Get pivot x
				getPivotY(void); // Get pivot y

		void     setViewport(int32_t x, int32_t y, int32_t w, int32_t h, bool vpDatum = true);
		bool     checkViewport(int32_t x, int32_t y, int32_t w, int32_t h);
		int32_t  getViewportX(void);
		int32_t  getViewportY(void);
		int32_t  getViewportWidth(void);
		int32_t  getViewportHeight(void);
		bool     getViewportDatum(void);
		void     frameViewport(uint16_t color, int32_t w);
		void     resetViewport(void);
};

#define H_SLIDER true
#define V_SLIDER false

typedef struct slider_t {
  // createSlider
  uint16_t slotWidth = 5;
  uint16_t slotLength = 100;
  uint16_t slotColor = GREEN;
  uint16_t slotBgColor = BLACK;
  bool orientation = true;

  // createKnob
  uint16_t knobWidth = 21;
  uint16_t knobHeight =21;
  uint16_t knobRadius = 5;
  uint16_t knobColor = WHITE;
  uint16_t knobLineColor = RED;

  // setSliderScale
  int16_t sliderLT = 0.0;
  int16_t sliderRB = 100.0;
  int16_t startPosition = 50;
  uint16_t sliderDelay = 2000;

} slider_param;


class SliderWidget {

 public:

  SliderWidget(TFTLIB_8BIT *tft, TFTLIB_Sprites *spr);

  void drawSlider(uint16_t x, uint16_t y);
  void drawSlider(uint16_t x, uint16_t y, slider_t param);

  bool createSlider(uint16_t slotWidth, uint16_t slotLength, uint16_t slotColor, uint16_t bgColor, bool orientation);

  void createKnob(uint16_t kwidth, uint16_t kheight, uint16_t kradius, uint16_t kcolor1, uint16_t kcolor2);

  void setSliderScale(int16_t min, int16_t max, uint16_t usdelay);
  void setSliderScale(int16_t min, int16_t max);

  void setSliderPosition(int16_t val);
  int16_t getSliderPosition(void);

  void getBoundingBox(int16_t *xs, int16_t *ys, int16_t *xe, int16_t *ye);
  void getBoundingRect(int16_t *x, int16_t *y, uint16_t *w, uint16_t *h);

  bool checkTouch(uint16_t tx, uint16_t ty);

 private:
  void moveTo(int16_t val);
  void drawKnob(uint16_t kpos);

  // createSlider
  uint16_t _slotWidth;
  uint16_t _slotLength;
  uint16_t _slotColor;
  uint16_t _slotBgColor;
  bool _horiz;

  // createKnob
  uint16_t _kwidth;
  uint16_t _kheight;
  uint16_t _kradius;
  uint16_t _kcolor1;
  uint16_t _kcolor2;

  // setSliderScale
  int16_t _sliderMin = 0.0;
  int16_t _sliderMax = 100.0;
  uint16_t _usdelay = 0;

  // drawSlider
  uint16_t _xpos = 0;
  uint16_t _ypos = 0;

  bool _invert = false;

  // moveTo
  int16_t _sliderPos;
  uint16_t _kposPrev;


  // checkTouch
  uint16_t _sxs;
  uint16_t _sys;
  uint16_t _sxe;
  uint16_t _sye;

  TFTLIB_Sprites *_spr;
  TFTLIB_8BIT *_tft;
};

class GraphWidget {

 public:

  GraphWidget(TFTLIB_8BIT *tft);

  bool createGraph(uint16_t graphWidth, uint16_t graphHeight, uint16_t bgColor);
  void setGraphGrid(float xsval, float xinc, float ysval, float yinc, uint16_t gridColor);

  void setGraphPosition(uint16_t x, uint16_t y);
  void getGraphPosition(uint16_t *x, uint16_t *y);

  void drawGraph(uint16_t x, uint16_t y);

  void setGraphScale(float xmin, float xmax, float ymin, float ymax);

  void getBoundingBox(int16_t *xs, int16_t *ys, int16_t *xe, int16_t *ye);
  void getBoundingRect(int16_t *x, int16_t *y, uint16_t *w, uint16_t *h);

  int16_t getPointX(float xval);
  int16_t getPointY(float yval);

  bool addLine(float xs, float ys, float xe, float ye, uint16_t col);

  // createGraph
  uint16_t _width;
  uint16_t _height;

  // setGraphScale
  float _xMin = 0.0;
  float _xMax = 100.0;
  float _yMin = 0.0;
  float _yMax = 100.0;

  // drawGraph
  uint16_t _xpos = 0;
  uint16_t _ypos = 0;

 private:

  // Map a float to a new range
  float mapFloat(float ip, float ipmin, float ipmax, float tomin, float tomax)
  {
    return tomin + (((tomax - tomin) * (ip - ipmin))/ (ipmax - ipmin));
  }

  uint16_t regionCode(float x, float y);
  bool clipTrace(float *xs, float *ys, float *xe, float *ye);

  uint16_t _gridColor;
  uint16_t _bgColor;

  // setGraphGrid
  float _xGridStart = 0.0;
  float _xGridInc = 10.0;
  float _yGridStart = 0.0;
  float _yGridInc = 10.0;

  // Defining region codes
  const uint16_t INSIDE = 0x0; // 0000
  const uint16_t LEFT =   0x1; // 0001
  const uint16_t RIGHT =  0x2; // 0010
  const uint16_t BOTTOM = 0x4; // 0100
  const uint16_t TOP =    0x8; // 1000

  TFTLIB_8BIT *_tft;
};

class TraceWidget {

 public:

  TraceWidget(GraphWidget *gw);

  void startTrace(uint16_t ptColor);
  bool addPoint(float xval, float yval);

  uint16_t getLastPointX(void);
  uint16_t getLastPointY(void);

 private:

  // Map a float to a new range
  float mapFloat(float ip, float ipmin, float ipmax, float tomin, float tomax)
  {
    return tomin + (((tomax - tomin) * (ip - ipmin))/ (ipmax - ipmin));
  }

  uint16_t regionCode(float x, float y);
  bool clipTrace(float *xs, float *ys, float *xe, float *ye);

  // trace
  bool _newTrace = true;
  uint16_t _ptColor = WHITE;
  uint16_t _xpt = 0;
  uint16_t _ypt = 0;
  float _xval = 0;
  float _yval = 0;

  GraphWidget *_gw;
};

#pragma GCC pop_options

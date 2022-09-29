
#include "hardware/vreg.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "hardware/pio.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"

#include "FatFS/spi_driver.h"
#include "FatFS/ff.h"
#include "pio_spi/pio_spi.h"

#include "FreeRTOS.h"
#include "task.h"

#include "TFTLIB_8BIT.h"
#include "XPT2046.h"

#include "lvgl.h"
#include "demos/lv_demos.h"
#include "examples/lv_examples.h"
#include "simple_ui/ui.h"

//using namespace std;

#define FLAG 0xDEADBEEF

void core1_task(void);

extern "C" pio_spi_inst_t spi;

uint16_t fil_buff[44000];
uint32_t size = sizeof(fil_buff) / sizeof(fil_buff[0]);

uint AUDIO_PIN = 13;
int wav_position = 0;
int audio_pin_slice = pwm_gpio_to_slice_num(AUDIO_PIN);
pwm_config config;

FIL music;
UINT bw;
DIR dir;
FILINFO fno;
FRESULT fres;
FATFS fatfs;

void pwm_interrupt_handler() {
	pwm_clear_irq(pwm_gpio_to_slice_num(AUDIO_PIN));

	//Play audio file when it's not EOF.
	if(!f_eof(&music)) {
		if (wav_position < size) {
			pwm_set_gpio_level(AUDIO_PIN, fil_buff[wav_position]);
			wav_position++;
		}
		else {
			// reset to start
			wav_position = 0;
		}

		//if we sending second part of buffer than reload first part
		if(wav_position == 22000-1) {
			multicore_fifo_push_blocking(0);
		}

		//and in the other way
		if(wav_position == 0) {
			multicore_fifo_push_blocking(1);
		}
	}

	else {
		//When EOF is true than close current music file, find next and open
		f_close(&music);
		f_findnext(&dir, &fno);

		// If all files were played then run searching routing again in root directory
		if(fno.fattrib & AM_DIR) {
			f_closedir(&dir);
		}

		f_findfirst(&dir, &fno, "/", "*.mus");
		f_open(&music, fno.fname, FA_READ | FA_OPEN_EXISTING);
		f_lseek(&music, 0);
		f_read(&music, fil_buff, 88000, &bw);
	}
}

#define DISP_HOR_RES 800

using namespace std;

static lv_disp_drv_t disp_drv;
static lv_indev_drv_t touch_drv;

static lv_color_t disp_buf1[DISP_HOR_RES * 24];
static lv_color_t disp_buf2[DISP_HOR_RES * 24];

void flush_buff(lv_disp_drv_t * drv, lv_area_t * area, lv_color_t * color_p);

TFTLIB_8BIT tft(NT35510_PARALLEL, 10, 11, 21, 22);
XPT2046_Touchscreen ts(spi0, 17, 20);

void disp_init(void) {
	tft.init();
	tft.setRotation(3);

	static lv_disp_draw_buf_t buf;

	lv_disp_draw_buf_init(&buf, disp_buf1, disp_buf2, DISP_HOR_RES * 24);
	lv_disp_drv_init(&disp_drv);

	disp_drv.draw_buf = &buf;
	disp_drv.flush_cb = flush_buff;
	disp_drv.hor_res = 480;
	disp_drv.ver_res = 800;
	disp_drv.rotated = 3;
	lv_disp_drv_register(&disp_drv);
}

uint16_t last_x = 0;
uint16_t last_y = 0;

void touchpad_read(lv_indev_drv_t * drv, lv_indev_data_t *data)
{
	uint16_t x = 0;
	uint16_t y = 0;

	if (ts.pressed()) {
		ts.getTouch(&y, &x);
		data->point.x = 480 - x;
		data->point.y = y;
		last_x = data->point.x;
		last_y = data->point.y;
		data->state = LV_INDEV_STATE_PRESSED;
	}

	else
	{
		data->point.x = last_x;
		data->point.y = last_y;
		data->state = LV_INDEV_STATE_RELEASED;
	}
}

void touchpad_init(void)
{
	ts.init();
	ts.setRotation(3);
	static lv_indev_drv_t indev_drv;
	lv_indev_drv_init(&indev_drv);
	indev_drv.read_cb = touchpad_read;
	indev_drv.type = LV_INDEV_TYPE_POINTER;
	lv_indev_drv_register(&indev_drv);
}

void flush_buff(lv_disp_drv_t * drv, lv_area_t * area, lv_color_t * color_p)
{
	tft.drawImage(area->x1, area->y1, area->x2 - area->x1 + 1, area->y2 - area->y1 + 1, (uint16_t*)color_p);
	lv_disp_flush_ready(drv);
}

void lvgl_task(void* args) {
	while(1) {
		lv_task_handler();
		vTaskDelay(4);
	}
}

int main() {
	// Overclock core0 & core 1 to 352MHz!
	vreg_set_voltage(VREG_VOLTAGE_1_25);
	stdio_init_all();
	set_sys_clock_khz(352000, true);

	// Initialize AUDIO_PIN as PWM and CS pin for SDC
	gpio_init(AUDIO_PIN);
	gpio_set_function(AUDIO_PIN, GPIO_FUNC_PWM);

	gpio_init(SD_CS);
	gpio_set_dir(SD_CS, GPIO_OUT);
    gpio_put(SD_CS, 1);

	// Initialize PIO driven SPI
	uint offset = pio_add_program(spi.pio, &spi_cpha0_program);

	pio_spi_init(spi.pio, spi.sm, offset,
				8,				// 8 bits per SPI frame
				22000 * 1000,	// 32 MHz @ 256MHz clk_sys
				false,			// CPHA = 0
				false,			// CPOL = 0
				SD_MOSI,
				SD_MISO,
				SD_CLK
    );

	//Init SPI0 for touchscreen
	const uint sck_pin = 18;
	const uint mosi_pin = 19;
	const uint miso_pin = 16;

	spi_init(spi0, 1000 * 1000);

	spi_set_format(spi0, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_LSB_FIRST);

	gpio_set_function(sck_pin, GPIO_FUNC_SPI);
	gpio_set_function(mosi_pin, GPIO_FUNC_SPI);
	gpio_set_function(miso_pin, GPIO_FUNC_SPI);

	// Setup PWM interrupt to fire when PWM cycle is complete
	pwm_clear_irq(AUDIO_PIN);
	pwm_set_irq_enabled(audio_pin_slice, true);

	// Set the interrupt handler function
	irq_set_exclusive_handler(PWM_IRQ_WRAP, pwm_interrupt_handler); 
	irq_set_enabled(PWM_IRQ_WRAP, true);

	// Setup PWM for audio output
	config = pwm_get_default_config();

	// SYSCLK formula --> SAMPLING_FREQUENCY * MAX_PWM_VALUE
	pwm_config_set_clkdiv(&config, 2.0f); 
	pwm_config_set_wrap(&config, 4000);

	// Mount SDC
	f_mount(&fatfs, "0:", 1);

	// Initialize LVGL and run demo
	lv_init();
	disp_init();
	touchpad_init();
	ui_init();

	// Run core1 code
	multicore_launch_core1(core1_task);

    // Wait for core1 response
    multicore_fifo_pop_blocking();

	//Find first audio file and open
	f_findfirst(&dir, &fno, "/", "*.mus");
	f_open(&music, fno.fname, FA_READ | FA_OPEN_EXISTING);
	f_read(&music, &fil_buff, size * 2, &bw);

	// If we finally get some data in buffer than run timer
	pwm_init(audio_pin_slice, &config, true);

	// Create task for FreeRTOS and run scheduler
	xTaskCreate(lvgl_task, "LVGL Task", 512, NULL, 1, NULL);
	vTaskStartScheduler();

	while(1) {
		// All audio routines are running in interrupts.
	}
}

void core1_task(void) {
	// If CORE1 started properly than send FLAG to CORE0
	multicore_fifo_push_blocking(FLAG);

	while(1) {
        //wait until we get flag to reload buffer
		uint16_t data = multicore_fifo_pop_blocking();
		f_read(&music, fil_buff + (22000*data), 44000, &bw);
	}
}
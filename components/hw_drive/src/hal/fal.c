//
// Created by Thomas Abplanalp on 27.11.24.
//

#include "fal.h"

#include <portmacro.h>
#include <robokit_log.h>
#include <driver/gpio.h>
#include <driver/adc.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "scheduler.h"

#define ROBOKIT_FB_LEFT ADC_CHANNEL_0
#define ROBOKIT_FB_MIDDLE_LEFT ADC_CHANNEL_1
#define ROBOKIT_FB_MIDDLE ADC_CHANNEL_2
#define ROBOKIT_FB_MIDDLE_RIGHT ADC_CHANNEL_3
#define ROBOKIT_FB_RIGHT ADC_CHANNEL_4

#define ROBOKIT_READ_INTERVAL_MS 3

typedef struct {
	uint16_t red;
	uint16_t green;
	uint16_t blue;
} S_color;

static volatile S_color my_colors[5];

static uint16_t fal_get_sensor_left(void) {
	return adc1_get_raw(ROBOKIT_FB_LEFT);
}

static uint16_t fal_get_sensor_middle_left(void) {
	return adc1_get_raw(ROBOKIT_FB_MIDDLE_LEFT);
}

static uint16_t fal_get_sensor_middle(void) {
	return adc1_get_raw(ROBOKIT_FB_MIDDLE);
}

static uint16_t fal_get_sensor_middle_right(void) {
	return adc1_get_raw(ROBOKIT_FB_MIDDLE_RIGHT);
}

static uint16_t fal_get_sensor_right(void) {
	return adc1_get_raw(ROBOKIT_FB_RIGHT);
}

void fal_init(void) {
	gpio_config_t io_conf;
	io_conf.intr_type = GPIO_INTR_DISABLE;          // Keine Interrupts
	io_conf.mode = GPIO_MODE_OUTPUT;                // Als Output konfigurieren
	io_conf.pin_bit_mask = (1ULL << GPIO_RED) |     // Mask für die gewünschten Pins
						   (1ULL << GPIO_GREEN) |
						   (1ULL << GPIO_BLUE);
	io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;   // Kein Pull-Down-Widerstand
	io_conf.pull_up_en = GPIO_PULLUP_DISABLE;       // Kein Pull-Up-Widerstand
	gpio_config(&io_conf);

	io_conf.mode = GPIO_MODE_INPUT;
	io_conf.pin_bit_mask = (1ULL << 13);
	io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
	gpio_config(&io_conf);

	adc1_config_width(ADC_WIDTH_BIT_12);

	adc1_config_channel_atten(ROBOKIT_FB_LEFT, ADC_ATTEN_DB_0);
	adc1_config_channel_atten(ROBOKIT_FB_MIDDLE_LEFT, ADC_ATTEN_DB_0);
	adc1_config_channel_atten(ROBOKIT_FB_MIDDLE, ADC_ATTEN_DB_0);
	adc1_config_channel_atten(ROBOKIT_FB_MIDDLE_RIGHT, ADC_ATTEN_DB_0);
	adc1_config_channel_atten(ROBOKIT_FB_RIGHT, ADC_ATTEN_DB_0);
}


static void fal_read_red(void) {
	gpio_set_level(GPIO_RED, 1);
	gpio_set_level(GPIO_GREEN, 0);
	gpio_set_level(GPIO_BLUE, 0);

	vTaskDelay(ROBOKIT_READ_INTERVAL_MS / portTICK_PERIOD_MS);
	my_colors[0].red = fal_get_sensor_left();
	my_colors[1].red = fal_get_sensor_middle_left();
	my_colors[2].red = fal_get_sensor_middle();
	my_colors[3].red = fal_get_sensor_middle_right();
	my_colors[4].red = fal_get_sensor_right();

	gpio_set_level(GPIO_RED, 0);
	gpio_set_level(GPIO_GREEN, 0);
	gpio_set_level(GPIO_BLUE, 0);
}

static void fal_read_green(void) {
	gpio_set_level(GPIO_RED, 0);
	gpio_set_level(GPIO_GREEN, 1);
	gpio_set_level(GPIO_BLUE, 0);

	vTaskDelay(ROBOKIT_READ_INTERVAL_MS / portTICK_PERIOD_MS);
	my_colors[0].green = fal_get_sensor_left();
	my_colors[1].green = fal_get_sensor_middle_left();
	my_colors[2].green = fal_get_sensor_middle();
	my_colors[3].green = fal_get_sensor_middle_right();
	my_colors[4].green = fal_get_sensor_right();

	gpio_set_level(GPIO_RED, 0);
	gpio_set_level(GPIO_GREEN, 0);
	gpio_set_level(GPIO_BLUE, 0);
}

static void fal_read_blue(void) {
	gpio_set_level(GPIO_RED, 0);
	gpio_set_level(GPIO_GREEN, 0);
	gpio_set_level(GPIO_BLUE, 1);

	vTaskDelay(ROBOKIT_READ_INTERVAL_MS / portTICK_PERIOD_MS);
	my_colors[0].blue = fal_get_sensor_left();
	my_colors[1].blue = fal_get_sensor_middle_left();
	my_colors[2].blue = fal_get_sensor_middle();
	my_colors[3].blue = fal_get_sensor_middle_right();
	my_colors[4].blue = fal_get_sensor_right();

	gpio_set_level(GPIO_RED, 0);
	gpio_set_level(GPIO_GREEN, 0);
	gpio_set_level(GPIO_BLUE, 0);
}

static void print_fal(void) {
	ROBOKIT_LOGI("CD | POS | RED  | GRN  | BLUE");
	ROBOKIT_LOGI("----+------+------+------|");
	ROBOKIT_LOGI("01 | LFT | %04d | %04d | %04d", my_colors[0].red, my_colors[0].green, my_colors[0].blue);
	ROBOKIT_LOGI("02 | LFM | %04d | %04d | %04d", my_colors[1].red, my_colors[1].green, my_colors[1].blue);
	ROBOKIT_LOGI("03 | MID | %04d | %04d | %04d", my_colors[2].red, my_colors[2].green, my_colors[2].blue);
	ROBOKIT_LOGI("04 | RTM | %04d | %04d | %04d", my_colors[3].red, my_colors[3].green, my_colors[3].blue);
	ROBOKIT_LOGI("05 | RGT | %04d | %04d | %04d", my_colors[4].red, my_colors[4].green, my_colors[4].blue);
	ROBOKIT_LOGI("----+------+------+------|");
}

void fal_update(void) {
	static uint8_t tester = 1;
	if(gpio_get_level(13) == 0) {
		tester = 0;
		return;
	}

	if(tester == 0) {
		tester = 1;
		fal_read_red();
		fal_read_green();
		fal_read_blue();
		print_fal();
	}
	return;

	tester++;

	static uint8_t status = 0;
	switch (status) {
		case 0:
			status = 1;
			fal_read_red();
		break;
		case 1:
			status = 2;
			fal_read_green();
		break;
		default:
			status = 0;
			fal_read_blue();

		print_fal();
		break;
	}
}
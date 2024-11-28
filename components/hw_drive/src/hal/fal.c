//
// Created by Thomas Abplanalp on 27.11.24.
//

#include "fal.h"

#include <led_command.h>
#include <portmacro.h>
#include <robokit_log.h>
#include <driver/gpio.h>
#include <driver/adc.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "scheduler.h"
#include "command_data_containers.h"

#define ROBOKIT_FB_LEFT ADC_CHANNEL_0
#define ROBOKIT_FB_MIDDLE_LEFT ADC_CHANNEL_1
#define ROBOKIT_FB_MIDDLE ADC_CHANNEL_2
#define ROBOKIT_FB_MIDDLE_RIGHT ADC_CHANNEL_3
#define ROBOKIT_FB_RIGHT ADC_CHANNEL_4

#define ROBOKIT_READ_INTERVAL_MS 5


static volatile uint16_t my_colors[3][5];
static volatile uint16_t result_colors[5];
static volatile uint16_t my_colors_reference[5] = {0};
static volatile uint8_t my_status = 0;

static uint8_t fal_calibration = 15;
static int8_t fal_result[5] = {0};

uint8_t fal_is_calibrated(void) {
	return fal_calibration < 1;
}

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

static void _robokit_cmd_handler(_S_Command_Fal *cmd, uint8_t mode, uint8_t *flags) {
	if (mode == E_SCHEDULE_MODE_PRECHECK) {
		if(cmd->flags & 2) {
			if(!fal_is_calibrated()) {
				ROBOKIT_LOGE("Calibration not yet done.");
				*flags = 0xFF;
				return;
			}
		}
		*flags = 0;
	}

	if(mode == E_SCHEDULE_MODE_PERFORM) {
		if(cmd->flags & 1) {
			// Kalibrieren
			my_status = 1;
			fal_calibration = 15;
		} else if(cmd->flags & 2) {
			my_status |= 2;
		} else {
			my_status &= ~2;
		}
	}
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

	adc1_config_width(ADC_WIDTH_BIT_12);

	adc1_config_channel_atten(ROBOKIT_FB_LEFT, ADC_ATTEN_DB_0);
	adc1_config_channel_atten(ROBOKIT_FB_MIDDLE_LEFT, ADC_ATTEN_DB_0);
	adc1_config_channel_atten(ROBOKIT_FB_MIDDLE, ADC_ATTEN_DB_0);
	adc1_config_channel_atten(ROBOKIT_FB_MIDDLE_RIGHT, ADC_ATTEN_DB_0);
	adc1_config_channel_atten(ROBOKIT_FB_RIGHT, ADC_ATTEN_DB_0);

	robokit_register_command_fn(E_COMMAND_FAL, _robokit_cmd_handler);
}

static void calculate_fal_result(void) {
	for(int e=0;e<5;e++) {
		fal_result[e] = (my_colors_reference[e] > result_colors[e]) ? (my_colors_reference[e] - result_colors[e]) : 0;
	}
}

static void fal_read_red(void) {
	static uint8_t counter = 0;

	gpio_set_level(GPIO_RED, 1);
	gpio_set_level(GPIO_GREEN, 1);
	gpio_set_level(GPIO_BLUE, 1);

	vTaskDelay(ROBOKIT_READ_INTERVAL_MS / portTICK_PERIOD_MS);
	my_colors[counter][0] = fal_get_sensor_left();
	my_colors[counter][1] = fal_get_sensor_middle_left();
	my_colors[counter][2] = fal_get_sensor_middle();
	my_colors[counter][3] = fal_get_sensor_middle_right();
	my_colors[counter][4] = fal_get_sensor_right();

	if(++counter > 2) {
		counter = 0;
		for(int e=0;e<5;e++) {
			result_colors[e] = (result_colors[e] + my_colors[0][e] + my_colors[1][e] + my_colors[2][e])/4;
		}
	}

	gpio_set_level(GPIO_RED, 0);
	gpio_set_level(GPIO_GREEN, 0);
	gpio_set_level(GPIO_BLUE, 0);
}

static void print_fal(void) {
	ROBOKIT_LOGI("FAL[L-R, 1..5]: %04d %04d %04d %04d %04d",
		fal_result[0],
		fal_result[1],
		fal_result[2],
		fal_result[3],
		fal_result[4]
	);
}

static void _copy_reference(uint8_t abs) {
	if(abs) {
		my_colors_reference[0] = result_colors[0];
		my_colors_reference[1] = result_colors[1];
		my_colors_reference[2] = result_colors[2];
		my_colors_reference[3] = result_colors[3];
		my_colors_reference[4] = result_colors[4];
	}
}

void fal_update(void) {
	if(my_status < 1)
		return;

	fal_read_red();

	static uint8_t counter = 0;

	if(fal_calibration > 1) {
		fal_calibration--;
		ROBOKIT_LOGI("RUN_CAL[L-R, 1..5]: %04d %04d %04d %04d %04d",
			result_colors[0],
			result_colors[1],
			result_colors[2],
			result_colors[3],
			result_colors[4]
		);
		return;
	}

	if(fal_calibration == 1) {
		_copy_reference(1);
		fal_calibration = 0;
		ROBOKIT_LOGI("KALIBRATION[L-R, 1..5]: %04d %04d %04d %04d %04d",
			my_colors_reference[0],
			my_colors_reference[1],
			my_colors_reference[2],
			my_colors_reference[3],
			my_colors_reference[4]
		);
		my_status = 0;
		return;
	}

	if(counter++>2) {
		counter = 0;
		calculate_fal_result();

		print_fal();
	}
}
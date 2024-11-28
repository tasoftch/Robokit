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

#define ROBOKIT_READ_INTERVAL_US 1000

typedef struct {
	uint16_t red;
	uint16_t green;
	uint16_t blue;
} S_color;

static volatile S_color my_colors[5];
static volatile S_color my_colors_reference[5] = {0};
static volatile uint8_t my_status = 0;

static S_Fal_Result fal_result = {0};

static char my_color_refs[] = {'S', 'B', 'G', 'C', 'R', 'M', 'Y', 'W'};

uint8_t fal_is_calibrated(void) {
	return my_status & 128;
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

	gpio_pullup_dis(GPIO_NUM_0);
	gpio_pulldown_dis(GPIO_NUM_0);
	gpio_pullup_dis(GPIO_NUM_1);
	gpio_pulldown_dis(GPIO_NUM_1);

	adc1_config_channel_atten(ROBOKIT_FB_LEFT, ADC_ATTEN_DB_0);
	adc1_config_channel_atten(ROBOKIT_FB_MIDDLE_LEFT, ADC_ATTEN_DB_0);
	adc1_config_channel_atten(ROBOKIT_FB_MIDDLE, ADC_ATTEN_DB_0);
	adc1_config_channel_atten(ROBOKIT_FB_MIDDLE_RIGHT, ADC_ATTEN_DB_0);
	adc1_config_channel_atten(ROBOKIT_FB_RIGHT, ADC_ATTEN_DB_0);

	robokit_register_command_fn(E_COMMAND_FAL, _robokit_cmd_handler);
}


static void fal_read_red(void) {
	gpio_set_level(GPIO_RED, 1);
	gpio_set_level(GPIO_GREEN, 0);
	gpio_set_level(GPIO_BLUE, 0);

	esp_rom_delay_us(ROBOKIT_READ_INTERVAL_US);

	my_colors[4].red = fal_get_sensor_right();
	my_colors[3].red = fal_get_sensor_middle_right();
	my_colors[2].red = fal_get_sensor_middle();
	my_colors[1].red = fal_get_sensor_middle_left();
	my_colors[0].red = fal_get_sensor_left();

	gpio_set_level(GPIO_RED, 0);
	gpio_set_level(GPIO_GREEN, 0);
	gpio_set_level(GPIO_BLUE, 0);
}

static void fal_read_green(void) {
	gpio_set_level(GPIO_RED, 0);
	gpio_set_level(GPIO_GREEN, 1);
	gpio_set_level(GPIO_BLUE, 0);

	esp_rom_delay_us(ROBOKIT_READ_INTERVAL_US);

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

	esp_rom_delay_us(ROBOKIT_READ_INTERVAL_US);

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

static uint8_t _calculate_color(uint8_t index) {
	uint8_t color = ROBOKIT_FAL_WHITE;
	if(my_colors[index].red < my_colors_reference[index].red *2 / 3) {
		color &= ~ ROBOKIT_FAL_RED;
	}
	if(my_colors[index].green < my_colors_reference[index].green *2 / 3) {
		color &= ~ ROBOKIT_FAL_GREEN;
	}
	if(my_colors[index].blue < my_colors_reference[index].blue *2 / 3) {
		color &= ~ ROBOKIT_FAL_BLUE;
	}
	return color;
}

static void calculate_result() {
	fal_result.fb_1_left = _calculate_color(0);
	fal_result.fb_2_middle_left = _calculate_color(1);
	fal_result.fb_3_middle = _calculate_color(2);
	fal_result.fb_4_middle_right = _calculate_color(3);
	fal_result.fb_5_right = _calculate_color(4);

	ROBOKIT_LOGI("FAL[L-R, 1..5]: %c %c %c %c %c",
		my_color_refs[ fal_result.fb_1_left ],
		my_color_refs[ fal_result.fb_2_middle_left ],
		my_color_refs[ fal_result.fb_3_middle ],
		my_color_refs[ fal_result.fb_4_middle_right ],
		my_color_refs[ fal_result.fb_5_right ]
	);
}

void fal_update(void) {
	if(my_status & 1) {
		ROBOKIT_LOGI("KAL START");
		fal_read_red();
		fal_read_green();
		fal_read_blue();
		ROBOKIT_LOGI("KAL STOP");
		my_colors_reference[0] = my_colors[0];
		my_colors_reference[1] = my_colors[1];
		my_colors_reference[2] = my_colors[2];
		my_colors_reference[3] = my_colors[3];
		my_colors_reference[4] = my_colors[4];
		ROBOKIT_LOGI("KALIBRATION");
		print_fal();

		my_status |= 128;
		my_status &= ~1;
		return;
	}

	if(!fal_is_calibrated())
		return;

	if(!(my_status & 2))
		return;

	fal_read_red();
	fal_read_green();
	fal_read_blue();

	print_fal();

	return;
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
	}
}
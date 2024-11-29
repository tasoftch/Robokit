//
// Created by Thomas Abplanalp on 27.11.24.
//

#include "fal.h"

#include <device.h>
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
static volatile S_color my_color_minimums[5] = {0};
static volatile S_color my_color_maximums[5] = {0};

static void (*my_fal_callback_calibration)(uint8_t);
static void (*my_fal_line_interpreter)(S_Fal_Result *) = fal_default_line_result_interpreter;

static uint8_t is_running=0;
static uint8_t is_calibrated=0;
static uint8_t make_drive_command=0;
static uint8_t drive_counter=0;

uint8_t fal_is_calibrated(void) {
	return is_calibrated;
}

static char my_color_refs[] = {'S', 'B', 'G', 'C', 'R', 'M', 'Y', 'W'};


void fal_set_line_result_interpreter(void (*interpreter) (S_Fal_Result *)) {
	my_fal_line_interpreter = interpreter;
}


unsigned char fal_name_of_color(uint8_t color) {
	if (color < 8) {
		return my_color_refs[color];
	}

	return '-';
}


void fal_default_line_result_interpreter(S_Fal_Result *result) {
	S_command cmd;
	S_vector vector;

	uint8_t flags = 0;
	static int8_t lut[] = {
		0, 		    // -----
		-45, 		// ----S
		-30, 		// ---S-
		-45, 		// ---SS
		0, 		    // --S--
		-30, 		// --S-S
		-12, 		// --SS-
		-30, 		// --SSS
		30, 		// -S---
		-3, 		// -S--S
		0, 		    // -S-S-
		-3, 		// -S-SS
		12, 		    // -SS--
		12, 		    // -SS-S
		0, 		    // -SSS-
		-3, 		// -SSSS
		45, 		// S----
		45, 		    // S---S
		30, 		    // S--S-
		-3, 		// S--SS
		12, 		    // S-S--
		0, 		    // S-S-S
		0, 		    // S-SS-
		0, 		    // S-SSS
		30, 		// SS---
		30, 		// SS--S
		30, 		    // SS-S-
		0, 		    // SS-SS
		30, 		// SSS--
		30, 		// SSS-S
		0, 		    // SSSS-
		0 		    // SSSSS
	};
	flags |= (result->fb_1_left == ROBOKIT_FAL_BLACK) ? (1<<4) : 0;
	flags |= (result->fb_2_middle_left == ROBOKIT_FAL_BLACK) ? (1<<3) : 0;
	flags |= (result->fb_3_middle == ROBOKIT_FAL_BLACK) ? (1<<2) : 0;
	flags |= (result->fb_4_middle_right == ROBOKIT_FAL_BLACK) ? (1<<1) : 0;
	flags |= (result->fb_5_right == ROBOKIT_FAL_BLACK) ? (1<<0) : 0;

	vector = robokit_make_vector_polar(lut[flags], 50);

	robokit_make_drive_command_vector(&cmd, vector);
	robokit_push_command(&cmd, 0);

	ROBOKIT_LOGI("FAL [1..5] %c %c %c %c %c %d",
		fal_name_of_color( result->fb_1_left ) ,
		fal_name_of_color( result->fb_2_middle_left ) ,
		fal_name_of_color( result->fb_3_middle ) ,
		fal_name_of_color( result->fb_4_middle_right ) ,
		fal_name_of_color( result->fb_5_right ),
		lut[flags]
		);
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

static void _fal_reset_calibration(void) {
	for(int e=0; e<5; e++) {
		my_color_maximums[e] = (S_color){0};
		my_color_minimums[e] = (S_color){
			.red = 0xFFFF,
			.green = 0xFFFF,
			.blue = 0xFFFF
		};
	}
	is_calibrated = 0;
}

static void _robokit_cmd_handler(_S_Command_Fal *cmd, uint8_t mode, uint8_t *flags) {
	if (mode == E_SCHEDULE_MODE_PRECHECK) {
		if(cmd->flags == E_FAL_OPTION_ENABLE) {
			if(!fal_is_calibrated()) {
				ROBOKIT_LOGE("Calibration not yet done.");
				*flags = 0xFF;
				return;
			}
		}
		*flags = 0;
	}

	if(mode == E_SCHEDULE_MODE_PERFORM) {
		if(cmd->flags == E_FAL_OPTION_CALIBRATE) {
			_fal_reset_calibration();
			make_drive_command = 1;
			drive_counter = 500;
			is_running = 1;
			my_fal_callback_calibration = cmd->callback;
		} else if(cmd->flags == E_FAL_OPTION_ENABLE) {
			is_running = 1;
		} else if(cmd->flags == E_FAL_OPTION_DISABLE) {
			is_running = 0;
			S_command cmd;
			robokit_make_drive_command_fwd(&cmd, 0);
			robokit_push_command(&cmd, 0);
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


static void fal_read_red(void) {
	gpio_set_level(GPIO_RED, 1);
	gpio_set_level(GPIO_GREEN, 0);
	gpio_set_level(GPIO_BLUE, 0);

	esp_rom_delay_us(ROBOKIT_READ_INTERVAL_US);

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

static void print_fal(S_color *the_colors) {
	ROBOKIT_LOGI("CD | POS | RED  | GRN  | BLUE");
	ROBOKIT_LOGI("----+------+------+------|");
	ROBOKIT_LOGI("01 | LFT | %04d | %04d | %04d", the_colors[0].red, the_colors[0].green, the_colors[0].blue);
	ROBOKIT_LOGI("02 | LFM | %04d | %04d | %04d", the_colors[1].red, the_colors[1].green, the_colors[1].blue);
	ROBOKIT_LOGI("03 | MID | %04d | %04d | %04d", the_colors[2].red, the_colors[2].green, the_colors[2].blue);
	ROBOKIT_LOGI("04 | RTM | %04d | %04d | %04d", the_colors[3].red, the_colors[3].green, the_colors[3].blue);
	ROBOKIT_LOGI("05 | RGT | %04d | %04d | %04d", the_colors[4].red, the_colors[4].green, the_colors[4].blue);
	ROBOKIT_LOGI("----+------+------+------|");
}


void _fal_calibration_done(void) {
	for(int e=0;e<5;e++) {
		my_color_minimums[e].red = (my_color_minimums[e].red + my_color_maximums[e].red) / 2;
		my_color_minimums[e].green = (my_color_minimums[e].green + my_color_maximums[e].green) / 2;
		my_color_minimums[e].blue = (my_color_minimums[e].blue + my_color_maximums[e].blue) / 2;
	}

	is_calibrated = 1;
	is_running = 0;
	make_drive_command = 2;

	if(my_fal_callback_calibration)
		my_fal_callback_calibration(1);
}


void _fal_calibration_init(void) {
	static uint8_t status = 0;

	if(--drive_counter < 1) {
		print_fal(my_color_minimums);
		print_fal(my_color_maximums);

		is_calibrated = 0;
		is_running = 0;
		make_drive_command = 2;
		if(my_fal_callback_calibration)
			my_fal_callback_calibration(0);
		return;
	}

	for(int e=0;e<5;e++) {
		my_color_minimums[e].red = MIN( my_color_minimums[e].red, my_colors[e].red );
		my_color_minimums[e].green = MIN( my_color_minimums[e].green, my_colors[e].green );
		my_color_minimums[e].blue = MIN( my_color_minimums[e].blue, my_colors[e].blue );

		my_color_maximums[e].red = MAX( my_color_maximums[e].red, my_colors[e].red );
		my_color_maximums[e].green = MAX( my_color_maximums[e].green, my_colors[e].green );
		my_color_maximums[e].blue = MAX( my_color_maximums[e].blue, my_colors[e].blue );
	}

	if(status == 1) {
		if(my_color_maximums[3].red - my_colors[3].red < 50) {
			_fal_calibration_done();
			status = 0;
		}
	}

	if(status == 0) {
		if(my_color_maximums[3].red - my_color_minimums[3].red > 100) {
			status = 1;
		}
	}
}


static void _fal_calculate_result(void) {
	if(!my_fal_line_interpreter)
		return;

	int e = 0;
	S_Fal_Result my_fal_result = {0};

	my_fal_result.fb_1_left |= my_colors[e].red > my_color_minimums[e].red ? ROBOKIT_FAL_RED : 0;
	my_fal_result.fb_1_left |= my_colors[e].green > my_color_minimums[e].green ? ROBOKIT_FAL_GREEN : 0;
	my_fal_result.fb_1_left |= my_colors[e].blue > my_color_minimums[e].blue ? ROBOKIT_FAL_BLUE : 0;

	e=1;
	my_fal_result.fb_2_middle_left |= my_colors[e].red > my_color_minimums[e].red ? ROBOKIT_FAL_RED : 0;
	my_fal_result.fb_2_middle_left |= my_colors[e].green > my_color_minimums[e].green ? ROBOKIT_FAL_GREEN : 0;
	my_fal_result.fb_2_middle_left |= my_colors[e].blue > my_color_minimums[e].blue ? ROBOKIT_FAL_BLUE : 0;

	e=2;
	my_fal_result.fb_3_middle |= my_colors[e].red > my_color_minimums[e].red ? ROBOKIT_FAL_RED : 0;
	my_fal_result.fb_3_middle |= my_colors[e].green > my_color_minimums[e].green ? ROBOKIT_FAL_GREEN : 0;
	my_fal_result.fb_3_middle |= my_colors[e].blue > my_color_minimums[e].blue ? ROBOKIT_FAL_BLUE : 0;

	e=3;
	my_fal_result.fb_4_middle_right |= my_colors[e].red > my_color_minimums[e].red ? ROBOKIT_FAL_RED : 0;
	my_fal_result.fb_4_middle_right |= my_colors[e].green > my_color_minimums[e].green ? ROBOKIT_FAL_GREEN : 0;
	my_fal_result.fb_4_middle_right |= my_colors[e].blue > my_color_minimums[e].blue ? ROBOKIT_FAL_BLUE : 0;

	e=4;
	my_fal_result.fb_5_right |= my_colors[e].red > my_color_minimums[e].red ? ROBOKIT_FAL_RED : 0;
	my_fal_result.fb_5_right |= my_colors[e].green > my_color_minimums[e].green ? ROBOKIT_FAL_GREEN : 0;
	my_fal_result.fb_5_right |= my_colors[e].blue > my_color_minimums[e].blue ? ROBOKIT_FAL_BLUE : 0;

	my_fal_line_interpreter(&my_fal_result);
}


void fal_render_result(void) {
	if(!is_calibrated) {
		_fal_calibration_init();
	} else {
		_fal_calculate_result();
	}
}

void fal_update(void) {
	if(make_drive_command == 1) {
		make_drive_command = 0;
		S_command cmd;
		robokit_make_drive_command_fwd(&cmd, 50);
		robokit_push_command(&cmd, 0);
	}

	if(make_drive_command == 2) {
		make_drive_command = 0;
		S_command cmd;
		robokit_make_drive_command_fwd(&cmd, 0);
		robokit_push_command(&cmd, 0);
	}

	if(!is_running)
		return;

	static uint8_t status = 0;
	switch (status) {
		case 0:
			status = 1;
			fal_read_red();
		//break;
		case 1:
			status = 2;
			fal_read_green();
		break;
		case 2:
			status = 3;
			fal_read_blue();
		//break;
		default:
			status = 0;
			fal_render_result();
	}
}
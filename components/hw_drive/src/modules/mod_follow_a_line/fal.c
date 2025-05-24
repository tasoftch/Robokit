/*
 * MIT License
 *
 * Copyright (c) 2024 Th. Abplanalp, F. Romer
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "hal/fal.h"

#include <device.h>
#include <portmacro.h>
#include <private/robokit_log.h>
#include <driver/gpio.h>
#include <driver/adc.h>

#include <modules/robokit_module.h>
#include <values.h>
#include "private/median.h"

#define ROBOKIT_FB_LEFT ADC_CHANNEL_0
#define ROBOKIT_FB_MIDDLE_LEFT ADC_CHANNEL_1
#define ROBOKIT_FB_MIDDLE ADC_CHANNEL_2
#define ROBOKIT_FB_MIDDLE_RIGHT ADC_CHANNEL_3
#define ROBOKIT_FB_RIGHT ADC_CHANNEL_4

#define ROBOKIT_READ_INTERVAL_US 1000

#define ROBOKIT_FAL_USE_MEDIAN 1

#if ROBOKIT_FAL_USE_MEDIAN
static S_robokit_median_filter_t median_filters[15] = {0};
#endif
static portMUX_TYPE lock_10_us = portMUX_INITIALIZER_UNLOCKED;


static S_color my_colors[5];
static volatile S_color my_color_minimums[5] = {0};
static volatile S_color my_color_maximums[5] = {0};

static void (*my_fal_callback_calibration)(uint8_t);
static void (*my_fal_callback_one_shot)(void);
static robokit_fal_interpreter_result_t (*my_fal_line_interpreter)(S_Fal_Result *, T_Speed) = fal_default_line_result_interpreter;

static S_Fal_Result my_global_fal_result;

// Marks the FAL system as running.
// The sensor loop will be active and reads the cells.
static uint8_t is_running=0;

// Triggers exactly one measurement of all sensors.
// Reports when done.
static uint8_t is_one_shot=0;

// Marks the FAL system as calibrated.
// That means, the threshold is set and the system is ready to
// proj3E: distinguish between black and white
// proj4E: distinguish between black, white, red, green and blue.
//         If possible also combinations yellow, magenta and cyan.
static uint8_t calibration_status = E_CALIBRATION_STATUS_UNCALIBRATED;

// Task separation.
// The command handler must only set configurations, it is not allowed to
// push commands into the system.
static uint8_t make_drive_command=0;
static T_Speed specified_drive_speed=0;
static uint8_t my_drive_timeout_counter = 0;
// Calibration counter for 500ms.
static uint16_t drive_counter=0;

/**
 * @inheritDoc
 */
uint8_t robokit_fal_get_calibration_status(void) {
	return calibration_status;
}

uint8_t fal_is_calibrated(void) {
	return calibration_status == E_CALIBRATION_STATUS_CALIBRATION_DONE ? 1 : 0;
}

uint8_t robokit_fal_is_running(void) {
	return is_running;
}

uint8_t robokit_fal_is_one_shot_complete(void) {
	if(is_one_shot == 2) {
		is_one_shot = 0;
		return 1;
	}
	return 0;
}

S_color robokit_fal_get_color_left(void) { return my_colors[0]; }
S_color robokit_fal_get_color_middle_left(void) { return my_colors[1]; }
S_color robokit_fal_get_color_middle(void) { return my_colors[2]; }
S_color robokit_fal_get_color_middle_right(void) { return my_colors[3]; }
S_color robokit_fal_get_color_right(void) { return my_colors[4]; }
S_Fal_Result robokit_fal_get_last_result(void) {
	ROBOKIT_LOGI("FAL [1..5] %c %c %c %c %c",
			fal_name_of_color( my_global_fal_result.fb_1_left ) ,
			fal_name_of_color( my_global_fal_result.fb_2_middle_left ) ,
			fal_name_of_color( my_global_fal_result.fb_3_middle ) ,
			fal_name_of_color( my_global_fal_result.fb_4_middle_right ) ,
			fal_name_of_color( my_global_fal_result.fb_5_right )
			);
	return my_global_fal_result;
}

/**
 * @inheritDoc
 */
void fal_set_line_result_interpreter(uint8_t (*interpreter) (S_Fal_Result *, T_Speed)) {
	my_fal_line_interpreter = interpreter;
}

/**
 * @inheritDoc
 */
unsigned char fal_name_of_color(uint8_t color) {
	if (color < 8) {
		static char my_color_refs[] = {'S', 'B', 'G', 'C', 'R', 'M', 'Y', 'W'};
		return my_color_refs[color];
	}

	return '-';
}

/**
 * @brief Reads the raw sensor value of the left sensor cell
 * @return The raw sensor value
 */
static uint16_t fal_get_sensor_left(void) {
	return adc1_get_raw(ROBOKIT_FB_LEFT);
}

/**
 * @brief Reads the raw sensor value of the middle left sensor cell
 * @return The raw sensor value
 */
static uint16_t fal_get_sensor_middle_left(void) {
	return adc1_get_raw(ROBOKIT_FB_MIDDLE_LEFT);
}

/**
 * @brief Reads the raw sensor value of the middle sensor cell
 * @return The raw sensor value
 */
static uint16_t fal_get_sensor_middle(void) {
	return adc1_get_raw(ROBOKIT_FB_MIDDLE);
}

/**
 * @brief Reads the raw sensor value of the middle right sensor cell
 * @return The raw sensor value
 */
static uint16_t fal_get_sensor_middle_right(void) {
	return adc1_get_raw(ROBOKIT_FB_MIDDLE_RIGHT);
}

/**
 * @brief Reads the raw sensor value of the right sensor cell
 * @return The raw sensor value
 */
static uint16_t fal_get_sensor_right(void) {
	return adc1_get_raw(ROBOKIT_FB_RIGHT);
}

/**
 * @brief Resets the calibration values
 *
 * Resets the minimum and maximum color values to calculate a new threshold
 * Within the next calibration loop.
 */
static void _fal_reset_calibration(void) {
	for(int e=0; e<5; e++) {
		my_color_maximums[e] = (S_color){0};
		my_color_minimums[e] = (S_color){
			.red = 0xFFFF,
			.green = 0xFFFF,
			.blue = 0xFFFF
		};
	}
	calibration_status = E_CALIBRATION_STATUS_UNCALIBRATED;
}

/**
 * @brief Initializes the necessary peripherals and configurations for FAL (Follow A Line) operation.
 *
 * This function sets up the GPIO pins for RGB output and configures the ADC channels
 * for the FAL sensors.
 */
ROBOKIT_MODULE_INIT() {
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
}


/**
 * @brief Helper function to capture the values of all single sensor cells for
 * a specific color
 *
 * @param color The specified color Red, Green or Blue to be captured
 * @param data_field_1 Data field of cell 1
 * @param data_field_2 Data field of cell 2
 * @param data_field_3 Data field of cell 3
 * @param data_field_4 Data field of cell 4
 * @param data_field_5 Data field of cell 5
 */
static void fal_read_sensor_cells(uint8_t color, uint16_t *data_field_1, uint16_t *data_field_2, uint16_t *data_field_3, uint16_t *data_field_4, uint16_t *data_field_5) {
	gpio_set_level(GPIO_RED, color & ROBOKIT_FAL_RED ? 1 : 0);
	gpio_set_level(GPIO_GREEN, color & ROBOKIT_FAL_GREEN ? 1 : 0);
	gpio_set_level(GPIO_BLUE, color & ROBOKIT_FAL_BLUE ? 1 : 0);

	portENTER_CRITICAL(&lock_10_us);
	esp_rom_delay_us(ROBOKIT_READ_INTERVAL_US);

	*data_field_1 = fal_get_sensor_left();
	*data_field_2 = fal_get_sensor_middle_left();
	*data_field_3 = fal_get_sensor_middle();
	*data_field_4 = fal_get_sensor_middle_right();
	*data_field_5 = fal_get_sensor_right();
	*data_field_1 = fal_get_sensor_left();		// Better measurements when repeating first again.
	portEXIT_CRITICAL(&lock_10_us);

	gpio_set_level(GPIO_RED, 0);
	gpio_set_level(GPIO_GREEN, 0);
	gpio_set_level(GPIO_BLUE, 0);
}

#if ROBOKIT_FAL_USE_MEDIAN
static void fal_filter_results_with_median(S_robokit_median_filter_t *filter, uint16_t *median) {
	*median = robokit_median_filter_shift(filter, *median);
}
#endif

/**
 * @brief Capture values of all sensor cells for Red color
 */
static void fal_read_red(void) {
	fal_read_sensor_cells(ROBOKIT_FAL_RED,
		&my_colors[0].red,
		&my_colors[1].red,
		&my_colors[2].red,
		&my_colors[3].red,
		&my_colors[4].red
	);
#if ROBOKIT_FAL_USE_MEDIAN
	fal_filter_results_with_median(&median_filters[0], &my_colors[0].red);
	fal_filter_results_with_median(&median_filters[1], &my_colors[1].red);
	fal_filter_results_with_median(&median_filters[2], &my_colors[2].red);
	fal_filter_results_with_median(&median_filters[3], &my_colors[3].red);
	fal_filter_results_with_median(&median_filters[4], &my_colors[4].red);
#endif
}

/**
 * @brief Capture values of all sensor cells for Green color
 */
static void fal_read_green(void) {
	fal_read_sensor_cells(ROBOKIT_FAL_GREEN,
		&my_colors[0].green,
		&my_colors[1].green,
		&my_colors[2].green,
		&my_colors[3].green,
		&my_colors[4].green
	);
#if ROBOKIT_FAL_USE_MEDIAN
	fal_filter_results_with_median(&median_filters[5], &my_colors[0].green);
	fal_filter_results_with_median(&median_filters[6], &my_colors[1].green);
	fal_filter_results_with_median(&median_filters[7], &my_colors[2].green);
	fal_filter_results_with_median(&median_filters[8], &my_colors[3].green);
	fal_filter_results_with_median(&median_filters[9], &my_colors[4].green);
#endif
}

/**
 * @brief Capture values of all sensor cells for Blue color
 */
static void fal_read_blue(void) {
	fal_read_sensor_cells(ROBOKIT_FAL_BLUE,
		&my_colors[0].blue,
		&my_colors[1].blue,
		&my_colors[2].blue,
		&my_colors[3].blue,
		&my_colors[4].blue
	);
#if ROBOKIT_FAL_USE_MEDIAN
	fal_filter_results_with_median(&median_filters[10], &my_colors[0].blue);
	fal_filter_results_with_median(&median_filters[11], &my_colors[1].blue);
	fal_filter_results_with_median(&median_filters[12], &my_colors[2].blue);
	fal_filter_results_with_median(&median_filters[13], &my_colors[3].blue);
	fal_filter_results_with_median(&median_filters[14], &my_colors[4].blue);
#endif
}

#if ROBOKIT_DEBUG
/**
 * @brief Debug function to display a list of colors
 * @param the_colors A list of colors
 */
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
#endif

/**
 * @brief Helper function to calculate threshold of each sensor cell
 *
 * It does also call the callback to the software with success flag.
 */
static void _fal_calibration_done(void) {
#if ROBOKIT_DEBUG
	print_fal(my_color_minimums);
	print_fal(my_color_maximums);
#endif
	for(int e=0;e<5;e++) {
		my_color_minimums[e].red = (my_color_minimums[e].red + my_color_maximums[e].red) / 2;
		my_color_minimums[e].green = (my_color_minimums[e].green + my_color_maximums[e].green) / 2;
		my_color_minimums[e].blue = (my_color_minimums[e].blue + my_color_maximums[e].blue) / 2;
	}

	calibration_status = E_CALIBRATION_STATUS_CALIBRATION_DONE;
	is_running = 0;
	make_drive_command = 2;

	if(my_fal_callback_calibration)
		my_fal_callback_calibration(1);
}

/**
 * @brief
 */
static void _fal_calibration_init(void) {
	static uint8_t status = 0;

	if(--drive_counter < 1) {
#if ROBOKIT_DEBUG
		print_fal(my_color_minimums);
		print_fal(my_color_maximums);
#endif
		calibration_status = E_CALIBRATION_STATUS_CALIBRATION_FAILED;
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

ROBOKIT_MODULE_COMMAND_HANDLER(E_COMMAND_FAL, _S_Command_Fal *cmd, uint8_t mode, uint8_t *flags) {
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
			specified_drive_speed = cmd->speed;
			make_drive_command = 1;
			drive_counter = cmd->timeout;
			is_running = 1;
			calibration_status = E_CALIBRATION_STATUS_CALIBRATING;
			my_fal_callback_calibration = cmd->callback;
		} else if(cmd->flags == E_FAL_OPTION_ENABLE) {
			is_running = 1;
			specified_drive_speed = cmd->speed;
			my_drive_timeout_counter = drive_counter = cmd->timeout;
		} else if(cmd->flags == E_FAL_OPTION_DISABLE) {
			is_running = 0;
			make_drive_command = 2;
		} else if(cmd->flags == E_FAL_OPTION_SHOT) {
			my_fal_callback_one_shot = cmd->callback;
			is_running = 1;
			is_one_shot = 1;
		}
	}
}

// Helper macro to assign color values to the FAL_Result
#define _ROBOKIT_FAL_WRITE_CELL_RESULT(CELL, COLOR_ID) \
CELL |= my_colors[COLOR_ID].red > my_color_minimums[COLOR_ID].red ? ROBOKIT_FAL_RED : 0;\
CELL |= my_colors[COLOR_ID].green > my_color_minimums[COLOR_ID].green ? ROBOKIT_FAL_RED : 0;\
CELL |= my_colors[COLOR_ID].blue > my_color_minimums[COLOR_ID].blue ? ROBOKIT_FAL_RED : 0

/**
 * @brief Helper function to assign color values to a FAL result.
 *
 * This function also calls the result interpreter if available to transform
 * the follow a line sensor results into Robokit commands.
 */
static void _fal_calculate_result(void) {
	if(!my_fal_line_interpreter)
		return;

	S_Fal_Result my_fal_result = {0};

	_ROBOKIT_FAL_WRITE_CELL_RESULT(my_fal_result.fb_1_left,		0);
	_ROBOKIT_FAL_WRITE_CELL_RESULT(my_fal_result.fb_2_middle_left,	1);
	_ROBOKIT_FAL_WRITE_CELL_RESULT(my_fal_result.fb_3_middle,	2);
	_ROBOKIT_FAL_WRITE_CELL_RESULT(my_fal_result.fb_4_middle_right, 3);
	_ROBOKIT_FAL_WRITE_CELL_RESULT(my_fal_result.fb_5_right,	4);

	robokit_fal_interpreter_result_t result = my_fal_line_interpreter(&my_fal_result, specified_drive_speed);

	if(!result) {
		if(--my_drive_timeout_counter < 1) {
			make_drive_command = 2;
			is_running = 0;
		}
	} else {
		my_global_fal_result = my_fal_result;
		my_drive_timeout_counter = drive_counter;
	}
}

/**
 * @brief Helper function to handle the end of sensor cell readings.
 */
static void fal_render_result(void) {
	if(is_one_shot) {
		is_running = 0;
		is_one_shot = 2;
		if(my_fal_callback_one_shot)
			my_fal_callback_one_shot();
	} else if(calibration_status == E_CALIBRATION_STATUS_CALIBRATING) {
		_fal_calibration_init();
	} else {
		_fal_calculate_result();
	}
}

/**
 * @brief Updates the status of the fal (feedback and light) system and handles related commands.
 *
 * The function controls the color reading sequence, checks the drive command status,
 * and initiates commands accordingly if the system is running.
 * It alternates between reading red, green, and blue using internal sensor functions
 * and renders the result once the cycle is complete.
 */
ROBOKIT_MODULE_SENSOR_LOOP() {
	static uint8_t old_running = 0;

	// The command handler must not push commands itself.
	// It is only allowed to make configurations.
	if(make_drive_command == 1) {
		S_command cmd;
		robokit_make_drive_command_fwd(&cmd, specified_drive_speed);
		make_drive_command = 0;
		robokit_push_command(&cmd, 0);
	}

	if(make_drive_command == 2) {
		make_drive_command = 0;
		S_command cmd;
		robokit_make_drive_command_fwd(&cmd, 0);
		robokit_push_command(&cmd, 0);
	}

	if(!is_running) {
		old_running = 0;
		return;
	}

	// 100Hz interval:
	// 1 cycle to read red and green,
	// next cycle to read blue and calculate the next commands.
	static uint8_t status = 0;



	if(old_running != is_running) {
		// Make sure that on a start it will begin by status 0
		old_running = is_running;
		status = 0;
	}

	switch (status) {
		case 0:
			status = 1;
			fal_read_red();
		break;
		case 1:
			status = 2;
			fal_read_green();
		break;
		case 2:
			status = 3;
			fal_read_blue();
		break;
		case 3:
			fal_render_result();
			status = 0;
		break;
		// It must pass.
		default:
			status = 0;
	}
}
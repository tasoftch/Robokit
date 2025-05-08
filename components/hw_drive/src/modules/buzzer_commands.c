//
// Created by Thomas Abplanalp on 05.05.25.
//

#include "buzzer_commands.h"

#include <FreeRTOSConfig.h>
#include <driver/ledc.h>
#include <freertos/FreeRTOS.h>
#include <freertos/projdefs.h>
#include <modules/robokit_module.h>
#include <private/robokit_log.h>

#define BUZZER_GPIO 18
#define BUZZER_SPEED_MODE LEDC_LOW_SPEED_MODE
#define BUZZER_LEDC_CHANNEL LEDC_CHANNEL_4

typedef struct {
	T_cmd cmd;
	uint8_t reserve0;
	uint8_t reserve1;
	uint8_t reserve2;
	uint16_t frequency;
	uint16_t period_ms;
} _S_buzzer_cmd;

ROBOKIT_CHECK_COMMAND_STRUCT(_S_buzzer_cmd);

uint8_t robokit_make_command_buzzer_beep(S_command *cmd, uint16_t freq, uint16_t duration_ms) {
	if(!cmd)
		return 0;

	ROBOKIT_COMMAND_RESET_P(cmd);
	_S_buzzer_cmd *buzzer_cmd = (_S_buzzer_cmd *)cmd;
	buzzer_cmd->cmd = E_COMMAND_BUZZER;
	buzzer_cmd->frequency = freq;
	buzzer_cmd->period_ms = duration_ms;

	return 1;
}

uint8_t robokit_make_command_buzzer_mute(S_command *cmd) {
	if(!cmd)
		return 0;

	ROBOKIT_COMMAND_RESET_P(cmd);
	_S_buzzer_cmd *buzzer_cmd = (_S_buzzer_cmd *)cmd;
	buzzer_cmd->cmd = E_COMMAND_BUZZER;
	buzzer_cmd->frequency = 0;
	buzzer_cmd->period_ms = 0;

	return 1;
}

ROBOKIT_MODULE_INIT() {
	ledc_timer_config_t ledc_timer = {
		.duty_resolution = LEDC_TIMER_8_BIT, // Auflösung 8-bit
		.freq_hz = 1000,                     // Startfrequenz 1 kHz
		.speed_mode = BUZZER_SPEED_MODE,   // Low speed mode
		.timer_num = LEDC_TIMER_2,
		.clk_cfg = LEDC_AUTO_CLK
	};
	ledc_timer_config(&ledc_timer);

	ledc_channel_config_t ledc_channel = {
		.channel    = BUZZER_LEDC_CHANNEL,
		.duty       = 0,                   // 50% duty cycle
		.gpio_num   = BUZZER_GPIO,
		.speed_mode = BUZZER_SPEED_MODE,
		.hpoint     = 0,
		.timer_sel  = LEDC_TIMER_2
	};
	ledc_channel_config(&ledc_channel);
	ROBOKIT_LOGI("buzzer initialized");
}

ROBOKIT_MODULE_COMMAND_HANDLER(E_COMMAND_BUZZER, _S_buzzer_cmd *cmd, uint8_t mode, uint8_t *flags) {
	if(mode == E_SCHEDULE_MODE_PERFORM) {
		ROBOKIT_LOGI("BUZZER %u", cmd->frequency);

		if(cmd->frequency > 250) {
			ledc_timer_config_t ledc_timer = {
				.duty_resolution = LEDC_TIMER_8_BIT,
				.freq_hz = cmd->frequency,
				.speed_mode = BUZZER_SPEED_MODE,
				.timer_num = LEDC_TIMER_2,
				.clk_cfg = LEDC_AUTO_CLK
			};
			ledc_timer_config(&ledc_timer);

			ledc_set_duty(BUZZER_SPEED_MODE, BUZZER_LEDC_CHANNEL, 0x80);
			ledc_update_duty(BUZZER_SPEED_MODE, BUZZER_LEDC_CHANNEL);
		} else {
			ledc_set_duty(BUZZER_SPEED_MODE, BUZZER_LEDC_CHANNEL, 0x0);
			ledc_stop(BUZZER_SPEED_MODE, BUZZER_LEDC_CHANNEL, 0);
		}
	}
}

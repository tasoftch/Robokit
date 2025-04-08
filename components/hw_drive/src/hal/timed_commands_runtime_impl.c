//
// Created by Thomas Abplanalp on 08.04.25.
//

#include "hal/timed_commands_impl.h"
#include "private/thread_safety.h"

static void (*_intern_handler)(S_command *, uint8_t);

static S_T_chain _runtime_buffer[ROBOKIT_TIMED_COMMAND_LISTS_COUNT] = {0};
static uint8_t _runtime_buffer_index = 0;

static S_T_chain *_current_chain = NULL;
static S_T_cmd *_current_command = NULL;

void tc_runtime_command_handler(void (*handler)(S_command *command, uint8_t flags)) {
	_intern_handler = handler;
}

S_T_chain *tc_runtime_current_chain(void) {
	if(_runtime_buffer_index>0)
		return &_runtime_buffer[0];
	return NULL;
}

void tc_runtime_make_current_chain(S_T_chain *chain) {
	if(chain && _current_chain == NULL) {
		ROBOKIT_THREAD_SAFE_BLOCK_START()
		_current_chain = chain;
		_current_command = chain->first_command;
		ROBOKIT_THREAD_SAFE_BLOCK_END();
	}
}

void tc_runtime_abort_current_chain(void) {
	S_T_chain *chain = tc_runtime_current_chain();
	if(chain) {

	}
}

uint8_t tc_runtime_free_buffer_count(void) {
	return ROBOKIT_TIMED_COMMAND_LISTS_COUNT - _runtime_buffer_index;
}

robokit_err_t tc_runtime_replace_chain(S_T_chain *chain);

uint8_t tc_runtime_free_buffer_count(void);
robokit_err_t tc_runtime_append_command_list(S_T_chain *chain);

// Moves the time forward by ms
void tc_runtime_update(uint8_t ms);

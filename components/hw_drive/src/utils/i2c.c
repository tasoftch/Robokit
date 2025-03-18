//
// Created by Thomas Abplanalp on 11.03.25.
//

#include "hal/i2c.h"

static i2c_cmd_handle_t _i2c_prepare_command(i2c_addr_t chip_addr, uint8_t reg_addr) {
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (chip_addr) | I2C_MASTER_WRITE, true);
	i2c_master_write_byte(cmd, reg_addr, true);

	return cmd;
}

esp_err_t i2c_write_register(i2c_addr_t chip_addr, uint8_t reg_addr) {
	i2c_cmd_handle_t cmd = _i2c_prepare_command(chip_addr, reg_addr);
	i2c_master_stop(cmd);
	esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(1000));
	i2c_cmd_link_delete(cmd);
	return ret;
}

esp_err_t i2c_write_register_byte(i2c_addr_t chip_addr, uint8_t reg_addr, uint8_t data) {
	i2c_cmd_handle_t cmd = _i2c_prepare_command(chip_addr, reg_addr);
	i2c_master_write_byte(cmd, data, true);
	i2c_master_stop(cmd);
	esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(1000));
	i2c_cmd_link_delete(cmd);
	return ret;
}

esp_err_t i2c_write_register_bytes(i2c_addr_t chip_addr, uint8_t reg_addr, uint8_t *data, uint8_t length) {
	i2c_cmd_handle_t cmd = _i2c_prepare_command(chip_addr, reg_addr);
	for(int e=0;e<length;e++)
		i2c_master_write_byte(cmd, data[e], true);
	i2c_master_stop(cmd);
	esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(1000));
	i2c_cmd_link_delete(cmd);
	return ret;
}

esp_err_t i2c_read_register_bytes(i2c_addr_t chip_addr, uint8_t reg_addr, uint8_t *data, uint8_t length) {
	esp_err_t ret = i2c_write_register(chip_addr, reg_addr);
	if(ret != ESP_OK)
		return ret;

	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (chip_addr) | I2C_MASTER_READ, true);

	for(int e=0;e<length;e++)
		i2c_master_read_byte(cmd, &data[e], I2C_MASTER_NACK);

	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(1000));
	i2c_cmd_link_delete(cmd);
	return ret;
}

uint8_t i2c_read_register_byte(i2c_addr_t chip_addr, uint8_t reg_addr, esp_err_t *error) {
	uint8_t data = 0;
	esp_err_t ret = i2c_read_register_bytes(chip_addr, reg_addr, &data, 1);
	if(*error)
		*error = ret;
	return data;
}

uint16_t i2c_read_register_word(i2c_addr_t chip_addr, uint8_t reg_addr, esp_err_t *error) {
	uint16_t data = 0;
	esp_err_t ret = i2c_read_register_bytes(chip_addr, reg_addr, (uint8_t*) &data, 2);
	if(*error)
		*error = ret;
	return data;
}

uint32_t i2c_read_register_word2(i2c_addr_t chip_addr, uint8_t reg_addr, esp_err_t *error) {
	uint32_t data = 0;
	esp_err_t ret = i2c_read_register_bytes(chip_addr, reg_addr, (uint8_t*) &data, 4);
	if(*error)
		*error = ret;
	return data;
}
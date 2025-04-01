//
// Created by Thomas Abplanalp on 11.03.25.
//

#include "hal/i2c.h"


#define I2C_MASTER_SDA 6   // Dein SDA-Pin (ändern je nach ESP32-C6 Board)
#define I2C_MASTER_SCL 7   // Dein SCL-Pin (ändern je nach Board)
#define I2C_MASTER_FREQ_HZ 100000  // 100 kHz für schnellen Zugriff
#define I2C_PORT I2C_NUM_0

void i2c_init() {
	static uint8_t did_init = 0;

	if(!did_init) {
		did_init = 1;

		i2c_config_t config = {
			.mode = I2C_MODE_MASTER,
			.sda_io_num = I2C_MASTER_SDA,
			.scl_io_num = I2C_MASTER_SCL,
			.sda_pullup_en = GPIO_PULLUP_ENABLE,
			.scl_pullup_en = GPIO_PULLUP_ENABLE,
			.master.clk_speed = I2C_MASTER_FREQ_HZ,
		};

		ESP_ERROR_CHECK(i2c_param_config(I2C_PORT, &config));
		ESP_ERROR_CHECK(i2c_driver_install(I2C_PORT, config.mode, 0, 0, 0));
	}
}

void i2c_scan() {
	printf("     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
	printf("----------------------------------------------------\n");
	for (uint8_t row = 0; row < 8; row++) {
		printf("%X0 |", row);

		for (uint8_t col = 0; col < 16; col++) {
			uint8_t addr = (row << 4) | col;

			if (addr < 3 || addr > 119) {  // I2C Adressen nur von 0x03 bis 0x77
				printf(" --");
				continue;
			}

			i2c_cmd_handle_t cmd = i2c_cmd_link_create();
			i2c_master_start(cmd);
			i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
			i2c_master_stop(cmd);

			esp_err_t err = i2c_master_cmd_begin(I2C_PORT, cmd, 100 / portTICK_PERIOD_MS);
			i2c_cmd_link_delete(cmd);

			if (err == ESP_OK) {
				printf(" %02X", addr);  // Gerät gefunden
			} else {
				printf(" --");  // Kein Gerät
			}
		}
		printf("\n");
	}
	printf("----------------------------------------------------\n");
}

esp_err_t i2c_write_register(i2c_addr_t chip_addr, uint8_t reg_addr) {
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (chip_addr << 1) | I2C_MASTER_WRITE, true);
	i2c_master_write_byte(cmd, reg_addr, true);

	i2c_master_stop(cmd);
	esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(1000));
	i2c_cmd_link_delete(cmd);
	return ret;
}

esp_err_t i2c_write_register_byte(i2c_addr_t chip_addr, uint8_t reg_addr, uint8_t data) {
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (chip_addr << 1) | I2C_MASTER_WRITE, true);
	i2c_master_write_byte(cmd, reg_addr, true);

	i2c_master_write_byte(cmd, data, true);
	i2c_master_stop(cmd);
	esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(1000));
	i2c_cmd_link_delete(cmd);
	return ret;
}

esp_err_t i2c_write_register_bytes(i2c_addr_t chip_addr, uint8_t reg_addr, uint8_t *data, uint8_t length) {
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (chip_addr << 1) | I2C_MASTER_WRITE, true);
	i2c_master_write_byte(cmd, reg_addr, true);

	for(int e=0;e<length;e++)
		i2c_master_write_byte(cmd, data[e], true);
	i2c_master_stop(cmd);
	esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(1000));
	i2c_cmd_link_delete(cmd);
	return ret;
}

esp_err_t i2c_read_register_bytes(i2c_addr_t chip_addr, uint8_t reg_addr, uint8_t *data, uint8_t length) {
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (chip_addr << 1) | I2C_MASTER_WRITE, true);
	i2c_master_write_byte(cmd, reg_addr, true);

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, (chip_addr << 1) | I2C_MASTER_READ, true);  // Leseoperation starten

	for (int i = 0; i < length - 1; i++) {
		i2c_master_read_byte(cmd, &data[i], I2C_MASTER_ACK);
	}
	i2c_master_read_byte(cmd, &data[length - 1], I2C_MASTER_NACK);

	i2c_master_stop(cmd);
	esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, pdMS_TO_TICKS(1000));
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
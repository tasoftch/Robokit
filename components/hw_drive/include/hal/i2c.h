//
// Created by Thomas Abplanalp on 11.03.25.
//

#ifndef I2C_H
#define I2C_H

#include <driver/i2c.h>

typedef uint8_t i2c_addr_t;

void i2c_init();
void i2c_scan();

esp_err_t i2c_write_register(i2c_addr_t chip_addr, uint8_t reg_addr);
esp_err_t i2c_write_register_byte(i2c_addr_t chip_addr, uint8_t reg_addr, uint8_t data);
esp_err_t i2c_write_register_bytes(i2c_addr_t chip_addr, uint8_t reg_addr, uint8_t *data, uint8_t length);

esp_err_t i2c_read_register_bytes(i2c_addr_t chip_addr, uint8_t reg_addr, uint8_t *data, uint8_t length);

uint8_t i2c_read_register_byte(i2c_addr_t chip_addr, uint8_t reg_addr, esp_err_t *error);
uint16_t i2c_read_register_word(i2c_addr_t chip_addr, uint8_t reg_addr, esp_err_t *error);
uint32_t i2c_read_register_word2(i2c_addr_t chip_addr, uint8_t reg_addr, esp_err_t *error);
#endif //I2C_H

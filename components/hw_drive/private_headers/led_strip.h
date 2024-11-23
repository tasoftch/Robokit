//
// Created by Thomas Abplanalp on 21.11.24.
//

#ifndef LED_STRIP_H
#define LED_STRIP_H

#include <config.h>
#include <esp_err.h>

/**
 * @brief LED strip pixel format
 */
typedef enum {
	LED_PIXEL_FORMAT_GRB,    /*!< Pixel format: GRB */
	LED_PIXEL_FORMAT_GRBW,   /*!< Pixel format: GRBW */
	LED_PIXEL_FORMAT_INVALID /*!< Invalid pixel format */
} led_pixel_format_t;

/**
	* @brief LED strip model
	* @note Different led model may have different timing parameters, so we need to distinguish them.
	*/
typedef enum {
	LED_MODEL_WS2812, /*!< LED strip model: WS2812 */
	LED_MODEL_SK6812, /*!< LED strip model: SK6812 */
	LED_MODEL_INVALID /*!< Invalid LED strip model */
} led_model_t;

/**
	* @brief LED strip handle
	*/
typedef struct led_strip_t *led_strip_handle_t;

/**
	* @brief LED Strip Configuration
	*/
typedef struct {
	int strip_gpio_num;      /*!< GPIO number that used by LED strip */
	uint32_t max_leds;       /*!< Maximum LEDs in a single strip */
	led_pixel_format_t led_pixel_format; /*!< LED pixel format */
	led_model_t led_model;   /*!< LED model */

	struct {
		uint32_t invert_out: 1; /*!< Invert output signal */
	} flags;                    /*!< Extra driver flags */
} led_strip_config_t;

/**
	* @brief Set RGB for a specific pixel
	*
	* @param strip: LED strip
	* @param index: index of pixel to set
	* @param red: red part of color
	* @param green: green part of color
	* @param blue: blue part of color
	*
	* @return
	*      - ESP_OK: Set RGB for a specific pixel successfully
	*      - ESP_ERR_INVALID_ARG: Set RGB for a specific pixel failed because of invalid parameters
	*      - ESP_FAIL: Set RGB for a specific pixel failed because other error occurred
	*/
esp_err_t led_strip_set_pixel(led_strip_handle_t strip, uint32_t index, uint32_t red, uint32_t green, uint32_t blue);

/**
	* @brief Set RGBW for a specific pixel
	*
	* @note Only call this function if your led strip does have the white component (e.g. SK6812-RGBW)
	* @note Also see `led_strip_set_pixel` if you only want to specify the RGB part of the color and bypass the white component
	*
	* @param strip: LED strip
	* @param index: index of pixel to set
	* @param red: red part of color
	* @param green: green part of color
	* @param blue: blue part of color
	* @param white: separate white component
	*
	* @return
	*      - ESP_OK: Set RGBW color for a specific pixel successfully
	*      - ESP_ERR_INVALID_ARG: Set RGBW color for a specific pixel failed because of an invalid argument
	*      - ESP_FAIL: Set RGBW color for a specific pixel failed because other error occurred
	*/
esp_err_t led_strip_set_pixel_rgbw(led_strip_handle_t strip, uint32_t index, uint32_t red, uint32_t green, uint32_t blue, uint32_t white);

/**
	* @brief Refresh memory colors to LEDs
	*
	* @param strip: LED strip
	*
	* @return
	*      - ESP_OK: Refresh successfully
	*      - ESP_FAIL: Refresh failed because some other error occurred
	*
	* @note:
	*      After updating the LED colors in the memory, a following invocation of this API is needed to flush colors to strip.
	*/
esp_err_t led_strip_refresh(led_strip_handle_t strip);

/**
	* @brief Clear LED strip (turn off all LEDs)
	*
	* @param strip: LED strip
	*
	* @return
	*      - ESP_OK: Clear LEDs successfully
	*      - ESP_FAIL: Clear LEDs failed because some other error occurred
	*/
esp_err_t led_strip_clear(led_strip_handle_t strip);

#endif //LED_STRIP_H

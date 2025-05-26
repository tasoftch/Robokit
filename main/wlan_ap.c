//
// Created by Thomas Abplanalp on 08.04.25.
//

#include "wlan_ap.h"

#include <esp_err.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_netif.h>
#include <esp_wifi.h>
#include <nvs_flash.h>
#include <string.h>
#include <values.h>
#include <private/parameters.h>

static const char *TAG = "wifi_ap";
extern void _robokit_internal_set_parameter(robokit_parameter_name_t name, robokit_parameter_type_t type, uint32_t value);

void wifi_init_softap(void) {
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());

	esp_netif_create_default_wifi_ap();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	char buffer_ssid[32] = {0x0};
	sprintf(buffer_ssid, "%s%s", WIFI_SSID, robokit_device_get_serial());

	printf("SSID (%d):%s\n", strlen(buffer_ssid), buffer_ssid);

	wifi_config_t wifi_config = {
		.ap = {
			.ssid_len = strlen(buffer_ssid),
			.password = WIFI_PASS,
			.max_connection = MAX_STA_CONN,
			.authmode = WIFI_AUTH_WPA_WPA2_PSK
		},
	};
	strcpy((char *)wifi_config.ap.ssid, buffer_ssid);

	if (strlen(WIFI_PASS) == 0) {
		wifi_config.ap.authmode = WIFI_AUTH_OPEN;
	}

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());

	ESP_LOGI(TAG, "Access Point gestartet: SSID=%s, PASS=%s", buffer_ssid, WIFI_PASS);

	int8_t pwr=0;
	esp_wifi_get_max_tx_power(&pwr);

	_robokit_internal_set_parameter(
	E_ROBOKIT_PARAM_WLAN_TX_POWER,
		E_ROBOKIT_PARAM_TYPE_INTEGER_8 | E_ROBOKIT_PARAM_TYPE_SIGNED,
		pwr
	);
}

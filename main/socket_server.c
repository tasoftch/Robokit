//
// Created by Thomas Abplanalp on 08.04.25.
//

#include "socket_server.h"

#include <esp_log.h>
#include <lwip/sockets.h>

static const char *TCP_TAG = "tcp_server";

static server_interpreter_t _main_interpreter = NULL;

void socket_server_interpreter(server_interpreter_t interpreter) {
	_main_interpreter = interpreter;
}

void tcp_server_task(void *pvParameters) {
	uint8_t rx_buffer[RECV_BUF_SIZE];
	struct sockaddr_in server_addr, client_addr;
	socklen_t addr_len = sizeof(client_addr);

	int listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (listen_sock < 0) {
		ESP_LOGE(TCP_TAG, "Fehler beim Erstellen des Sockets");
		vTaskDelete(NULL);
		return;
	}

	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);


	int err = bind(listen_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (err != 0) {
		ESP_LOGE(TCP_TAG, "Bind failed: errno %d", errno);
	}
	err = listen(listen_sock, 1);
	if (err != 0) {
		ESP_LOGE(TCP_TAG, "Listen failed: errno %d", errno);
	}

	ESP_LOGI(TCP_TAG, "TCP Server läuft auf Port %d", PORT);

	while (1) {
		int client_sock = accept(listen_sock, (struct sockaddr *)&client_addr, &addr_len);
		if (client_sock < 0) {
			ESP_LOGE(TCP_TAG, "Fehler beim Verbinden");
			continue;
		}

		ESP_LOGI(TCP_TAG, "Client verbunden");

		while (1) {
			int len = recv(client_sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
			if (len <= 0) {
				ESP_LOGI(TCP_TAG, "Client getrennt");
				break;
			}

			rx_buffer[len] = '\0'; // Null-terminieren
			if(_main_interpreter) {
				uint8_t snd = _main_interpreter(rx_buffer, len);
				if (snd > 0) {
					if(snd < RECV_BUF_SIZE-1) {
						rx_buffer[snd] = '\n';
						rx_buffer[snd+1] = '\0';
						send(client_sock, rx_buffer, snd+1, 0);
					} else {
						send(client_sock, "NOK\n", 4, 0);
					}
				} else {
					send(client_sock, "OK\n", 3, 0);
				}
			} else {
				ESP_LOGI(TCP_TAG, "Empfangen: %s", rx_buffer);
				send(client_sock, "OK\n", 3, 0);
			}
		}

		close(client_sock);
	}

	close(listen_sock);
	vTaskDelete(NULL);
}

void socket_server_init(void) {
	xTaskCreate(tcp_server_task, "tcp_server", 4096, NULL, 5, NULL);
}
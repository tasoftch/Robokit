#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include <esp_http_server.h>
#include "nvs_flash.h"
#include "esp_spiffs.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "rom/gpio.h"
#include <dirent.h>       // For opendir(), readdir(), closedir()
#include "esp_vfs.h"
#include "driver.h"
#include "cJSON.h"
#include "connect_wifi.h"
#include <device.h>

#define LED_PIN 2
httpd_handle_t server = NULL;
struct async_resp_arg {
    httpd_handle_t hd;
    int fd;
};

static const char *TAG = "WebSocket Server"; // TAG for debug
int led_state = 0;

#define INDEX_HTML_PATH "/spiffs/index.html"
char index_html[4096];
char response_data[4096];

static void initi_web_page_buffer(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true};

    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&conf));

      DIR* dir = opendir("/spiffs");
    if (!dir) {
        ESP_LOGE(TAG, "Failed to open /spiffs directory");
        return;
    }

    struct dirent* entry;
    // Iterate over all files and directories in /spiffs
    ESP_LOGI(TAG, "Listing files in /spiffs:");
    while ((entry = readdir(dir)) != NULL) {
        ESP_LOGI(TAG, "File: %s", entry->d_name);  // Log the filename
    }


    

    memset((void *)index_html, 0, sizeof(index_html));
    struct stat st;
    
    if (stat(INDEX_HTML_PATH, &st))
    {
        ESP_LOGE(TAG, "index.html not found");
        return;
    }

    FILE *fp = fopen(INDEX_HTML_PATH, "r");
    if (fread(index_html, st.st_size, 1, fp) == 0)
    {
        ESP_LOGE(TAG, "fread failed");
    }
    fclose(fp);
}

esp_err_t get_req_handler(httpd_req_t *req)
{
    int response;
    if(led_state)
    {
        sprintf(response_data, index_html, "ON");
    }
    else
    {
        sprintf(response_data, index_html, "OFF");
    }
    response = httpd_resp_send(req, response_data, HTTPD_RESP_USE_STRLEN);
    return response;
}

static void ws_async_send(void *arg)
{
    httpd_ws_frame_t ws_pkt;
    struct async_resp_arg *resp_arg = arg;
    httpd_handle_t hd = resp_arg->hd;
    int fd = resp_arg->fd;
    ESP_LOGI(TAG, "joooo im in send");

    //led_state = !led_state;
    //gpio_set_level(LED_PIN, led_state);
    
    char buff[4];
    memset(buff, 0, sizeof(buff));
    sprintf(buff, "%d",led_state);
    
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.payload = (uint8_t *)buff;
    ws_pkt.len = strlen(buff);
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;
    
    static size_t max_clients = CONFIG_LWIP_MAX_LISTENING_TCP;
    size_t fds = max_clients;
    int client_fds[max_clients];

    esp_err_t ret = httpd_get_client_list(server, &fds, client_fds);

    if (ret != ESP_OK) {
        return;
    }

    for (int i = 0; i < fds; i++) {
        int client_info = httpd_ws_get_fd_info(server, client_fds[i]);
        if (client_info == HTTPD_WS_CLIENT_WEBSOCKET) {
            httpd_ws_send_frame_async(hd, client_fds[i], &ws_pkt);
        }
    }
    free(resp_arg);
}

static esp_err_t trigger_async_send(httpd_handle_t handle, httpd_req_t *req)
{
    struct async_resp_arg *resp_arg = malloc(sizeof(struct async_resp_arg));
    resp_arg->hd = req->handle;
    resp_arg->fd = httpd_req_to_sockfd(req);
    return httpd_queue_work(handle, ws_async_send, resp_arg);
}

static esp_err_t handle_ws_req(httpd_req_t *req)
{
    if (req->method == HTTP_GET)
    {
        ESP_LOGI(TAG, "Handshake done, the new connection was opened");
        return ESP_OK;
    }

    httpd_ws_frame_t ws_pkt;
    uint8_t *buf = NULL;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;
    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "httpd_ws_recv_frame failed to get frame len with %d", ret);
        return ret;
    }

    if (ws_pkt.len)
    {
        buf = calloc(1, ws_pkt.len + 1);
        if (buf == NULL)
        {
            ESP_LOGE(TAG, "Failed to calloc memory for buf");
            return ESP_ERR_NO_MEM;
        }
        ws_pkt.payload = buf;
        ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", ret);
            free(buf);
            return ret;
        }
        ESP_LOGI(TAG, "Got packet with message: %s", ws_pkt.payload);
    }

    ESP_LOGI(TAG, "frame len is %d", ws_pkt.len);

    ESP_LOGW(TAG,"payload:%s",ws_pkt.payload);

    //get first to char of the payload
//_____________________________________________________________________________________


ESP_LOGI(TAG, "Third character: %c", (char)ws_pkt.payload[0]);
 





cJSON *json = cJSON_ParseWithLength((char*)ws_pkt.payload, ws_pkt.len);


char *json_string = cJSON_Print(json);
if (json_string) 
{
    ESP_LOGI("TAG", "Parsed JSON:\n%s", json_string);
}
ESP_LOGI(TAG, "1111111111");
const char *key = cJSON_GetStringValue(cJSON_GetObjectItem(json, "key"));
 ESP_LOGI(TAG, "222222222222");
const char *value = cJSON_GetStringValue(cJSON_GetObjectItem(json, "value"));
if(!value)
{
    ESP_LOGI(TAG,"isNULLLL");
}
 ESP_LOGI(TAG, "33333333333333333");
S_command cmd;
ESP_LOGW(TAG,"payload:%s",ws_pkt.payload);
ESP_LOGW(TAG,"key:%s",key);
ESP_LOGW(TAG,"value:%s", value);
ESP_LOGI(TAG, "44444444444444");


int num = atoi(key);
switch (num) // Note: No cast is necessary here
{
    case 1: // Assuming you want to match 't' (a single character)
        
        robokit_make_test_command(&cmd);
        robokit_push_command(&cmd, 0);
        break;

    case 2: // Assuming you want to match 'd' (a single character)
        // Handle 'd'
        //driver 100
        createrampcmdf(100);
       

        
        break;
    case 3:

    createrampcmdb(100);

    break;
    
    case 4:

    robokit_make_drive_command_fwd(&cmd,0);
    robokit_push_command(&cmd,0);
    break;

    default:
        // Handle unknown character
        break;
}
 cJSON_Delete(json);

//_____________________________________________________________________________________________________
   if (ws_pkt.type == HTTPD_WS_TYPE_TEXT &&
        strcmp((char *)ws_pkt.payload, "toggle") == 0)
    {
        free(buf);
        return trigger_async_send(req->handle, req);
    }
    return ESP_OK;
}

httpd_handle_t setup_websocket_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    httpd_uri_t uri_get = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = get_req_handler,
        .user_ctx = NULL};

    httpd_uri_t ws = {
        .uri = "/ws",
        .method = HTTP_GET,
        .handler = handle_ws_req,
        .user_ctx = NULL,
        .is_websocket = true};

    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &ws);
    }

    return server;
}



    

void app_main()
{
    device_init();

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    connect_wifi();

    if (wifi_connect_status)
    {
        //gpio_pad_select_gpio(LED_PIN);
        //gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

        //led_state = 0;
        ESP_LOGI(TAG, "ESP32 ESP-IDF WebSocket Web Server is running ... ...\n");
        initi_web_page_buffer();
        setup_websocket_server();
    }
    ESP_LOGI(TAG, "in TEST cmd");

    
}
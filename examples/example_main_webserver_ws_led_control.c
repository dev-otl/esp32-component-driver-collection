#include <stdio.h>

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "esp_http_server.h"
#include "esp_log.h"

#include "led.h"
#include "wifi.h"
#include "webserver.h"

/******************************************
 *
 */
/* A simple example that demonstrates using websocket echo server
 */
static const char *TAG = "ws_server_led";
led_handle_t led_handle;
/*
 * This handler echos back the received ws data
 * and triggers an async send if certain message received
 */
static esp_err_t echo_handler(httpd_req_t *req)
{
    esp_err_t ret;
    if (req->method == HTTP_GET)
    {
        ESP_LOGI(TAG, "Handshake done, the new connection was opened");
        return ESP_OK;
    }
    httpd_ws_frame_t ws_pkt;
    ret = webserver_ws_receive_frame(req, &ws_pkt);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "webserver_ws_receive failed with %d", ret);
        return ret;
    }
    ESP_LOGI(TAG, "Packet type: %d", ws_pkt.type);

    if (ws_pkt.type == HTTPD_WS_TYPE_TEXT)
    {
        if (ws_pkt.payload == NULL)
        {
            ret = webserver_ws_send_message(req, &ws_pkt, "SERVER: Empty string received");
        }
        else if (strcmp((char *)ws_pkt.payload, "Trigger async") == 0)
        {
            // free(buf);
            ret = webserver_ws_async_send_message(req, "my async message");
        }
        else if (strcmp((char *)ws_pkt.payload, "LED OFF") == 0)
        {
            led_write(led_handle, 0);
            goto sendstate;
        }
        else if (strcmp((char *)ws_pkt.payload, "LED ON") == 0)
        {
            led_write(led_handle, 1);
            goto sendstate;
        }
        else if (strcmp((char *)ws_pkt.payload, "LED STATE") == 0)
        {
        sendstate:
            uint8_t level = -1;
            led_read(led_handle, &level);
            switch (level)
            {
            case 0:
                ret = webserver_ws_send_message(req, &ws_pkt, "LED OFF");
                break;
            case 1:
                ret = webserver_ws_send_message(req, &ws_pkt, "LED ON");
                break;
            case 2:
            default:
                ret = webserver_ws_send_message(req, &ws_pkt, "LED ?");
                break;
            }
        }
        else
        {
            ret = webserver_ws_send_message(req, &ws_pkt, "my message");
        }
    }
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "webserver_ws_send_message failed with %d", ret);
    }
    // free(buf);
    return ret;
}
/*******************************************/

void app_main(void)
{
    led_config_t led_conf = {
        .pin = GPIO_NUM_23,
    };
    led_handle = NULL;
    led_new(&led_conf, &led_handle);
    wifi_connect();
    webserver_ws_start(80, "/wsecho", echo_handler);
}

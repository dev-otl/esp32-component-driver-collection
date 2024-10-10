#include <esp_log.h>
#include <esp_http_server.h>
#include "esp_wifi.h"

#include "webserver.h"

static const char *TAG = "webserver";
static httpd_handle_t ws_server;
static httpd_uri_t ws;
static uint16_t ws_port;
static const char *ws_uri;
static void *ws_handler;

esp_err_t start_webserver_ws()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    if (ws_port > 0)
    {
        config.server_port = ws_port;
    }

    // Start the httpd server
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    esp_err_t ret = httpd_start(&ws_server, &config);
    if (ret != ESP_OK)
    {
        ESP_LOGI(TAG, "Error starting server!");
        ws_server = NULL;
        return ret;
    }
    // Registering the ws handler
    ws.uri = ws_uri;
    ws.method = HTTP_GET;
    ws.handler = ws_handler; // add socket handler here
    ws.user_ctx = NULL;
    ws.is_websocket = true;

    ESP_LOGI(TAG, "Registering URI handlers");
    ret = httpd_register_uri_handler(ws_server, &ws);
    if (ret != ESP_OK)
    {
        ESP_LOGI(TAG, "Registering URI handlers");
    }
    return ret;
}

esp_err_t webserver_ws_stop()
{
    // Stop the httpd server
    esp_err_t ret = httpd_stop(ws_server);
    if (ret == ESP_OK)
    {
        ws_server = NULL;
        return ret;
    }
    ESP_LOGI(TAG, "Error stopping server!");
    return ret;
}

static void connect_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (ws_server == NULL)
    {
        ESP_LOGI(TAG, "Starting webserver");
        start_webserver_ws();
    }
}

static void disconnect_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (ws_server)
    {
        ESP_LOGI(TAG, "Stopping webserver");
        webserver_ws_stop();
    }
}

esp_err_t webserver_ws_start(uint16_t port, const char *uri, void *handler)
{
    ws_server = NULL;
    ws_port = port;
    ws_uri = uri;
    ws_handler = handler;

    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, NULL));

    return start_webserver_ws();
}

esp_err_t webserver_ws_receive_frame(httpd_req_t *req, httpd_ws_frame_t *frame)
{
    httpd_ws_frame_t ws_pkt;
    uint8_t *buf = NULL;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;
    /* Set max_len = 0 to get the frame len */
    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, 0);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "httpd_ws_recv_frame failed to get frame len with %d", ret);
        return ret;
    }
    ESP_LOGI(TAG, "frame len is %d", ws_pkt.len);
    if (ws_pkt.len)
    {
        /* ws_pkt.len + 1 is for NULL termination as we are expecting a string */
        buf = calloc(1, ws_pkt.len + 1);
        if (buf == NULL)
        {
            ESP_LOGE(TAG, "Failed to calloc memory for buf");
            return ESP_ERR_NO_MEM;
        }
        ws_pkt.payload = buf;
        /* Set max_len = ws_pkt.len to get the frame payload */
        ret = httpd_ws_recv_frame(req, &ws_pkt, ws_pkt.len);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", ret);
            free(buf);
            return ret;
        }
        ESP_LOGI(TAG, "Got packet with message: %s", ws_pkt.payload);
    }
    *frame = ws_pkt;
    return ESP_OK;
}

esp_err_t webserver_ws_send_message(httpd_req_t *req, httpd_ws_frame_t *ws_pkt, char *msg)
{
    ws_pkt->payload = (uint8_t *)msg;
    ws_pkt->len = strlen(msg);
    ws_pkt->type = HTTPD_WS_TYPE_TEXT;
    return httpd_ws_send_frame(req, ws_pkt);
}

typedef struct ws_async_send_message_arg_s
{
    httpd_handle_t hd;
    int fd;
    const char *message;
} ws_async_send_arg_t;

static void ws_async_send(void *arg)
{
    ws_async_send_arg_t *resp_arg = arg;
    httpd_handle_t hd = resp_arg->hd;
    int fd = resp_arg->fd; // socket descriptor
    const char *data = resp_arg->message;

    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.payload = (uint8_t *)data;
    ws_pkt.len = strlen(data);
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;

    httpd_ws_send_frame_async(hd, fd, &ws_pkt);
    free(resp_arg);
}

esp_err_t webserver_ws_async_send_message(httpd_req_t *req, const char *msg)
{
    // trigger_async_send(req->handle, req);
    ws_async_send_arg_t *resp_arg = malloc(sizeof(ws_async_send_arg_t));
    if (resp_arg == NULL)
    {
        return ESP_ERR_NO_MEM;
    }
    resp_arg->hd = req->handle;
    resp_arg->fd = httpd_req_to_sockfd(req);
    resp_arg->message = msg;
    esp_err_t ret = httpd_queue_work(req->handle, ws_async_send, resp_arg);
    if (ret != ESP_OK)
    {
        free(resp_arg);
    }
    return ret;
}

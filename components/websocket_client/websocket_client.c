#include <stdio.h>
#include "esp_log.h"
#include "esp_websocket_client.h"

#include "websocket_client.h"

#define ESP_CHECK_GOTO_ON_ERROR(func, label)                                         \
    {                                                                                \
        ret = func;                                                                  \
        if (ret != ESP_OK)                                                           \
        {                                                                            \
            ESP_LOGE(TAG, "websocket_client_start error: %s", esp_err_to_name(ret)); \
            goto label;                                                              \
        }                                                                            \
    }

static const char *TAG = "WS_CLIENT";

esp_err_t websocket_client_start(const char *ws_uri_with_port, esp_event_handler_t ws_event_handler, void *ws_events_handler_arg, ws_client_handle_t *ws_client_handle)
{
    esp_err_t ret = ESP_OK;

    esp_websocket_client_config_t config = {.uri = ws_uri_with_port};
    esp_websocket_client_handle_t handle = esp_websocket_client_init(&config);
    if (handle == NULL)
    {
        ESP_LOGE(TAG, "Could not create handle");
        return ESP_FAIL;
    }
    ESP_CHECK_GOTO_ON_ERROR(esp_websocket_register_events(handle, WEBSOCKET_EVENT_ANY, ws_event_handler, (void *)handle), error);

    ESP_CHECK_GOTO_ON_ERROR(esp_websocket_client_start(handle), error);

    *ws_client_handle = handle;

    return ret;

error:
    if (handle)
    {
        esp_websocket_client_close(handle, portMAX_DELAY);
        esp_websocket_client_destroy(handle);
    }
    return ret;
}

int websocket_client_send_message(ws_client_handle_t *handle, const char *msg, int len)
{
    if (esp_websocket_client_is_connected(*handle))
        return esp_websocket_client_send_text(*(esp_websocket_client_handle_t *)handle, msg, len, portMAX_DELAY);
    return -1;
}

esp_err_t websocket_client_stop(ws_client_handle_t *handle)
{
    return esp_websocket_client_close(*(esp_websocket_client_handle_t *)handle, portMAX_DELAY);
}
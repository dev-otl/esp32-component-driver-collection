#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "esp_event_base.h"
#include "esp_websocket_client.h"

    typedef esp_websocket_client_handle_t ws_client_handle_s;

    typedef ws_client_handle_s ws_client_handle_t;

    esp_err_t websocket_client_start(const char *ws_uri_with_port, esp_event_handler_t ws_event_handler, void *ws_events_handler_arg, ws_client_handle_t *ws_client_handle);
    int websocket_client_send_message(ws_client_handle_t *handle, const char *msg, int len);
    esp_err_t websocket_client_stop(ws_client_handle_t *handle);

#ifdef __cplusplus
    extern "C"
}
#endif
#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

    esp_err_t webserver_ws_start(uint16_t port, const char *uri, void *handler);
    esp_err_t webserver_ws_stop(void);
    esp_err_t webserver_ws_receive_frame(httpd_req_t *req, httpd_ws_frame_t *frame);
    esp_err_t webserver_ws_send_message(httpd_req_t *req, httpd_ws_frame_t *ws_pkt, char *msg);
    esp_err_t webserver_ws_async_send_message(httpd_req_t *req, const char *msg);

#ifdef __cplusplus
}
#endif
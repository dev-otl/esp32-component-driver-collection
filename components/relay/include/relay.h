#pragma once

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        uint8_t pin;
    } relay_config_t;

    typedef struct relay_impl_s *relay_handle_t;
    typedef uint8_t relay_data_t;

    esp_err_t relay_new(relay_config_t *relay_conf, relay_handle_t *relay_handle);
    esp_err_t relay_del(relay_handle_t relay_handle);
    esp_err_t relay_write(relay_handle_t relay_handle, uint8_t level);
    esp_err_t relay_read(relay_handle_t relay_handle, uint8_t *level);
    esp_err_t relay_init(relay_config_t *relay_conf, relay_handle_t *relay_handle);
    esp_err_t relay_deinit(relay_handle_t relay_handle);

#ifdef __cplusplus
}
#endif
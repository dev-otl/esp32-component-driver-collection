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
    } led_config_t;

    typedef struct led_impl_s *led_handle_t;

    esp_err_t led_new(led_config_t *led_conf, led_handle_t *led_handle);
    esp_err_t led_del(led_handle_t led_handle);
    esp_err_t led_write(led_handle_t led_handle, uint8_t level);
    esp_err_t led_read(led_handle_t led_handle, uint8_t *level);

#ifdef __cplusplus
}
#endif
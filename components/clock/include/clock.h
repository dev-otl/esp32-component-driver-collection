#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "esp_err.h"

#define CLOCK_DEFAULT_NTP_SERVER CONFIG_ESP_CLOCK_NTP_SERVER

    struct clock_data_s
    {
        long int unixtime;
        struct timeval tv;
    };
    typedef struct clock_data_s clock_data_t;

    esp_err_t clock_init(void);
    esp_err_t clock_get_unixtime(clock_data_t *);

#ifdef __cplusplus
}
#endif
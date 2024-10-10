#pragma once

#include "esp_err.h"
#include "esp_adc\adc_oneshot.h"

#ifdef __cplusplus
extern "C"
{
#endif

    struct wls_config_s
    {
        uint8_t pin;
    };

    struct wls_handle_s
    {
        adc_oneshot_unit_handle_t adc_unit_handle;
        adc_channel_t adc_channel;
        bool is_calibrated;
        adc_cali_handle_t adc_cali_handle;
    };
    struct wls_data_s
    {
        int raw;
        int calibrated;
        double moisture;
    };

    typedef struct wls_config_s wls_config_t;
    typedef struct wls_handle_s wls_handle_t;
    typedef struct wls_data_s wls_data_t;

    esp_err_t wls_init(wls_config_t *, wls_handle_t *);
    esp_err_t wls_read(wls_handle_t *, wls_data_t *);
    esp_err_t wls_deinit(wls_handle_t *);

#ifdef __cplusplus
}
#endif

#pragma once

#include "esp_err.h"
#include "esp_adc\adc_oneshot.h"

#ifdef __cplusplus
extern "C"
{
#endif

    struct csms_config_s
    {
        uint8_t pin;
    };

    struct csms_handle_s
    {
        adc_oneshot_unit_handle_t adc_unit_handle;
        adc_channel_t adc_channel;
        bool is_calibrated;
        adc_cali_handle_t adc_cali_handle;
    };
    struct csms_data_s
    {
        int raw;
        int calibrated;
        double moisture;
    };

    typedef struct csms_config_s csms_config_t;
    typedef struct csms_handle_s csms_handle_t;
    typedef struct csms_data_s csms_data_t;

    esp_err_t csms_init(csms_config_t *, csms_handle_t *);
    esp_err_t csms_read(csms_handle_t *, csms_data_t *);
    esp_err_t csms_deinit(csms_handle_t *);

#ifdef __cplusplus
}
#endif

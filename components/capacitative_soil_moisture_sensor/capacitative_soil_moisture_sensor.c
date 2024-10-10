#include <stdio.h>
#include "esp_log.h"
#include "capacitative_soil_moisture_sensor.h"

#include "shareables.h"
// shared in shareables
//adc_oneshot_unit_handle_t adc_oneshot_unit_handle_array[NUM_ADC_UNIT];

#define ESP_ERR_CHECK_LOG_GOTO(func, message, label) \
    {                                                \
        ret = func;                                  \
        if (ret != ESP_OK)                           \
        {                                            \
            ESP_LOGE(TAG, message);                  \
            goto label;                              \
        }                                            \
    }

static const char *TAG = "CSMS";

esp_err_t csms_init(csms_config_t *csms_config, csms_handle_t *csms_handle)
{
    esp_err_t ret = ESP_OK;

    int io_num = csms_config->pin;
    adc_unit_t unit_id;
    adc_channel_t channel;
    ESP_ERR_CHECK_LOG_GOTO(adc_oneshot_io_to_channel(io_num, &unit_id, &channel), "gpio not valid adc pin", error);

    adc_oneshot_unit_init_cfg_t init_cfg = {
        .unit_id = unit_id,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };

    adc_oneshot_unit_handle_t adc_handle;

    ret = adc_oneshot_new_unit(&init_cfg, &adc_handle);
    if (ret != ESP_OK)
    {
        if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGW(TAG, "adc unit was previously initialized!. Skipping to channel configuration");
            adc_handle = adc_oneshot_unit_handle_array[unit_id];
        }
        else
            ESP_ERR_CHECK_LOG_GOTO(ret, "adc_oneshot_new_unit", error);
    }
    else
    {
        adc_oneshot_unit_handle_array[unit_id] = adc_handle;
    }

    // ESP_LOGI(TAG,|);
    adc_oneshot_chan_cfg_t chan_cfg = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_11,
    };
    ESP_ERR_CHECK_LOG_GOTO(adc_oneshot_config_channel(adc_handle, channel, &chan_cfg), "adc_oneshot_config_channel", error);

    bool calibrated = false;
    adc_cali_handle_t adc_cali_handle;
    ret = ESP_FAIL;

    adc_cali_scheme_ver_t scheme;
    scheme = ADC_CALI_SCHEME_VER_LINE_FITTING;
    ESP_LOGI(
        TAG,
        "scheme check:: line: %s",
        esp_err_to_name(adc_cali_check_scheme(&scheme)));
    scheme = ADC_CALI_SCHEME_VER_CURVE_FITTING;
    ESP_LOGI(
        TAG,
        "scheme check:: curve: %s",
        esp_err_to_name(adc_cali_check_scheme(&scheme)));

#if ADC_CALI_SCHEME_CURVE_FITTING_SUPPORTED
    ESP_LOGI(TAG, "calibration scheme used: %s", "Curve Fitting");
    adc_cali_curve_fitting_config_t cali_config = {
        .unit_id = unit_id,
        .chan = channel,
        .atten = chan_cfg.atten,
        .bitwidth = chan_cfg.bitwidth,
    };
    ret = adc_cali_create_scheme_curve_fitting(&cali_config, &adc_cali_handle);
#elif ADC_CALI_SCHEME_LINE_FITTING_SUPPORTED
    ESP_LOGI(TAG, "calibration scheme used: %s", "Line Fitting");
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = unit_id,
        .atten = chan_cfg.atten,
        .bitwidth = chan_cfg.bitwidth,
    };
    adc_cali_line_fitting_efuse_val_t cali_val;
    if (adc_cali_scheme_line_fitting_check_efuse(&cali_val) == ESP_OK)
    {
        if (cali_val == ADC_CALI_LINE_FITTING_EFUSE_VAL_DEFAULT_VREF)
        {
            ESP_LOGI(TAG, "eFuse not burnt, using nominal 1100mV");
            cali_config.default_vref = 1100;
        }
    }
    ret = adc_cali_create_scheme_line_fitting(&cali_config, &adc_cali_handle);
#endif
    if (ret == ESP_OK)
    {
        calibrated = true;
        ESP_LOGI(TAG, "calibrated successfully");
    }
    else
    {
        ESP_LOGW(TAG, "uncalibrated, calibration failed with code : %s", esp_err_to_name(ret));
    }

    csms_handle->adc_unit_handle = adc_handle;
    csms_handle->adc_channel = channel;
    csms_handle->is_calibrated = calibrated;
    csms_handle->adc_cali_handle = adc_cali_handle;

    ESP_LOGI(TAG, "initialized");
    return ESP_OK;
error:
    ESP_LOGE(TAG, "could not initialize with error : %s", esp_err_to_name(ret));
    return ret;
}

esp_err_t csms_read(csms_handle_t *csms_handle, csms_data_t *csms_data)
{
    esp_err_t ret = ESP_OK;

    adc_oneshot_unit_handle_t handle = csms_handle->adc_unit_handle;
    adc_channel_t chan = csms_handle->adc_channel;
    int out_raw;
    ESP_ERR_CHECK_LOG_GOTO(adc_oneshot_read(handle, chan, &out_raw), "adc_oneshot_read", error);
    csms_data->raw = out_raw;

    csms_data->calibrated = -1;
    if (csms_handle->is_calibrated)
    {
        adc_cali_handle_t cali_handle = csms_handle->adc_cali_handle;
        int cali_result;
        ESP_ERR_CHECK_LOG_GOTO(
            adc_cali_raw_to_voltage(cali_handle, out_raw, &cali_result),
            "adc_cali_raw_to_voltage",
            error);
        csms_data->calibrated = cali_result;
    }
    return ret;
error:
    ESP_LOGE(TAG, "could not read with error : %s", esp_err_to_name(ret));
    return ret;
}

esp_err_t csms_deinit(csms_handle_t *csms_handle)
{
    esp_err_t ret = ESP_OK;
    /*struct csms_handle_s
    {
        adc_oneshot_unit_handle_t adc_unit_handle;
        adc_channel_t adc_channel;
        bool is_calibrated;
        adc_cali_handle_t adc_cali_handle;
    };*/
    ESP_ERR_CHECK_LOG_GOTO(adc_oneshot_del_unit(csms_handle->adc_unit_handle) | adc_cali_delete_scheme_line_fitting(csms_handle->adc_cali_handle), "something went wrong", error);
    return ret;
error:
    ESP_LOGE(TAG, "something went wrong in csms_deinit() : %s", esp_err_to_name(ret));
    return ret;
}
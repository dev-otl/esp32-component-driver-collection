#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include "esp_adc\adc_oneshot.h"

#define NUM_ADC_UNIT 2

    extern adc_oneshot_unit_handle_t adc_oneshot_unit_handle_array[NUM_ADC_UNIT];

#ifdef __cplusplus
    extern "C"
}
#endif

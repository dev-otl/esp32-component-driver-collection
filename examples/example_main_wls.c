#include <stdio.h>

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

#include "water_level_sensor.h"

void app_main(void)
{
    esp_err_t ret = ESP_OK;
    wls_config_t wls_conf = {GPIO_NUM_33};
    wls_handle_t wls;
    if (wls_init(&wls_conf, &wls) != ESP_OK)
    {
        return;
    }
    wls_data_t data;
    while (1)
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        if (wls_read(&wls, &data) != ESP_OK)
        {
            return;
        }
        printf("reading raw:%d, converted:%d isCalibrated: %d, calibrated: %d\n", data.raw, data.raw * 3300 / 4096, wls.is_calibrated, data.calibrated);
    }
}

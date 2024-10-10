#include <stdio.h>

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

#include "capacitative_soil_moisture_sensor.h"

void app_main(void)
{
    esp_err_t ret = ESP_OK;
    csms_config_t csms_conf = {GPIO_NUM_34};
    csms_handle_t csms;
    if (csms_init(&csms_conf, &csms) != ESP_OK)
    {
        return;
    }
    csms_data_t data;
    while (1)
    {
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        if (csms_read(&csms, &data) != ESP_OK)
        {
            return;
        }
        printf("reading raw:%d, converted:%d isCalibrated: %d, calibrated: %d\n", data.raw, data.raw * 3300 / 4096, csms.is_calibrated, data.calibrated);
    }
}

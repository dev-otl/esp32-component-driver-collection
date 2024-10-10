#include <stdio.h>

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

#include "dht22.h"

void app_main(void)
{
    dht22_config_t dht22_conf = {GPIO_NUM_18};
    dht22_handle_t dht22 = {-1};
    dht22_init(&dht22_conf, &dht22);
    dht22_data_t dht22_data = {0, 0};
    while (1)
    {
        vTaskDelay(3000 / portTICK_PERIOD_MS);

        printf("status: %s, humidity: %f, temperature: %f\n", esp_err_to_name(dht22_read(&dht22, &dht22_data)), dht22_data.humidity, dht22_data.temperature);
    }
}

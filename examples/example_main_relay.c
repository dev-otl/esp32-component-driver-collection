#include <stdio.h>
#include "relay.h"

#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"


void app_main(void)
{
    relay_config_t relay_conf = {
        .pin = GPIO_NUM_23,
    };
    relay_handle_t relay_handle = NULL;
    relay_new(&relay_conf, &relay_handle);
    while (1)
    {
        relay_write(relay_handle, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        relay_write(relay_handle, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
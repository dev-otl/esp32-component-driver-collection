#include <stdio.h>
#include "led.h"

#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"


void app_main(void)
{
    led_config_t led_conf = {
        .pin = GPIO_NUM_23,
    };
    led_handle_t led_handle = NULL;
    led_new(&led_conf, &led_handle);
    while (1)
    {
        led_write(led_handle, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        led_write(led_handle, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
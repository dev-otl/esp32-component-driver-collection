#include <stdio.h>

#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

#include "led.h"
#include "wifi.h"

void app_main(void)
{
    led_config_t led_conf = {
        .pin = GPIO_NUM_23,
    };
    led_handle_t led_handle = NULL;
    led_new(&led_conf, &led_handle);
    wifi_init();
}
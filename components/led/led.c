#include "led.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_check.h"

#define ERR_CHECK_OR_GOTO(ret, label) \
    {                                 \
        if (ret != ESP_OK)            \
            goto label;               \
    }

#define TAG_LED "LED"

struct led_impl_s
{
    uint8_t pin;
};

esp_err_t led_new(led_config_t *led_conf, led_handle_t *led_handle)
{
    esp_err_t ret;
    struct led_impl_s *led_impl = NULL;
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT_OUTPUT;
    io_conf.pin_bit_mask = 1ULL << led_conf->pin;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    ret = gpio_config(&io_conf);
    ERR_CHECK_OR_GOTO(ret, error);
    led_impl = malloc(sizeof(struct led_impl_s));
    ESP_GOTO_ON_FALSE(led_impl != NULL, ESP_ERR_NO_MEM, error, TAG_LED, "Could not allocate memory");
    led_impl->pin = led_conf->pin;
    *led_handle = led_impl;
    return ret;
error:
    if (led_impl != NULL)
    {
        free(led_impl);
    }
    return ret;
}

esp_err_t led_del(led_handle_t led_handle)
{
    esp_err_t ret;
    ESP_GOTO_ON_FALSE(led_handle != NULL, ESP_ERR_INVALID_ARG, error, TAG_LED, "Bus must not be NULL");
    free(led_handle);
error:
    return ret;
}

esp_err_t led_write(led_handle_t led_handle, uint8_t level)
{
    esp_err_t ret;
    ret = gpio_set_level(led_handle->pin, level);
    return ret;
}

esp_err_t led_read(led_handle_t led_handle, uint8_t *level)
{
    esp_err_t ret = ESP_OK;
    *level = gpio_get_level(led_handle->pin);
    return ret;
}

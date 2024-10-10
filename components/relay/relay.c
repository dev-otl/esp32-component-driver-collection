#include "relay.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_check.h"

#define ERR_CHECK_OR_GOTO(ret, label) \
    {                                 \
        if (ret != ESP_OK)            \
            goto label;               \
    }

#define TAG_RELAY "RELAY"

struct relay_impl_s
{
    uint8_t pin;
};

esp_err_t relay_new(relay_config_t *relay_conf, relay_handle_t *relay_handle)
{
    esp_err_t ret;
    struct relay_impl_s *relay_impl = NULL;
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT_OUTPUT;
    io_conf.pin_bit_mask = 1ULL << relay_conf->pin;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    ret = gpio_config(&io_conf);
    ERR_CHECK_OR_GOTO(ret, error);
    relay_impl = malloc(sizeof(struct relay_impl_s));
    ESP_GOTO_ON_FALSE(relay_impl != NULL, ESP_ERR_NO_MEM, error, TAG_RELAY, "Could not allocate memory");
    relay_impl->pin = relay_conf->pin;
    *relay_handle = relay_impl;
    return ret;
error:
    if (relay_impl != NULL)
    {
        free(relay_impl);
    }
    return ret;
}

esp_err_t relay_del(relay_handle_t relay_handle)
{
    esp_err_t ret;
    ESP_GOTO_ON_FALSE(relay_handle != NULL, ESP_ERR_INVALID_ARG, error, TAG_RELAY, "Bus must not be NULL");
    free(relay_handle);
error:
    return ret;
}

esp_err_t relay_write(relay_handle_t relay_handle, uint8_t level)
{
    esp_err_t ret;
    ret = gpio_set_level(relay_handle->pin, level);
    return ret;
}

esp_err_t relay_read(relay_handle_t relay_handle, uint8_t *level)
{
    esp_err_t ret = ESP_OK;
    *level = gpio_get_level(relay_handle->pin);
    return ret;
}

esp_err_t relay_init(relay_config_t *relay_conf, relay_handle_t *relay_handle)
{
    return relay_new(relay_conf, relay_handle);
}
esp_err_t relay_deinit(relay_handle_t relay_handle)
{
    return relay_del(relay_handle);
}

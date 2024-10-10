#include <stdio.h>
#include "driver/gpio.h"
#include "rom/ets_sys.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"

#include "dht22.h"

#define DHT_OUT gpio_set_direction(pin, GPIO_MODE_INPUT_OUTPUT_OD)
#define DHT_IN gpio_set_direction(pin, GPIO_MODE_INPUT)
#define DHT_LOW gpio_set_level(pin, 0)
#define DHT_HIGH gpio_set_level(pin, 1)
#define DHT_VAL gpio_get_level(pin)
#define DHT_WAIT_US(x) ets_delay_us(x)
#define DHT_EXPECT(x) ERR_CHECK_OR_GOTO(dht22_expect_signal(handle, x, &elapsed_us), error)

#define ERR_CHECK_OR_GOTO(ret, label) \
    {                                 \
        if (ret != ESP_OK)            \
            goto label;               \
    }

#define EXPECT(x)                       \
    {                                   \
        int counter = 0;                \
        while (DHT_VAL != x)            \
        {                               \
            if (counter++ > max_count)  \
            {                           \
                return ESP_ERR_TIMEOUT; \
            }                           \
        }                               \
    }
static const char *TAG = "dht22";
int max_count = 100000;
int64_t lastReadingTimeUs = -2 * 1000 * 1000; // never read
dht22_data_t lastReading = {0, 0};

esp_err_t dht22_init(dht22_config_t *conf, dht22_handle_t *handle)
{
    esp_err_t ret;

    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = 1ULL << conf->pin;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    ret = gpio_config(&io_conf);
    ERR_CHECK_OR_GOTO(ret, error);

    handle->pin = conf->pin;
    return ret;

error:
    return ret;
}

esp_err_t dht22_read(dht22_handle_t *handle, dht22_data_t *data)
{
    // if reading too early send the previous result
    int64_t currentTime = esp_timer_get_time();
    if (currentTime - lastReadingTimeUs < 2 * 1000 * 1000)
    {
        *data = lastReading;
        ESP_LOGI("dht22", "sending last reading");
        return ESP_OK;
    }

    uint8_t pin = handle->pin; // dht pin

    // send start signal
    DHT_OUT;
    DHT_LOW;
    DHT_WAIT_US(2 * 1000);
    DHT_IN; // release pin control (pulled up by resistor)

    // start receiving signals //
    // receive response signal
    EXPECT(0);
    EXPECT(1);
    EXPECT(0);

    // start receiving 40 bits
    uint64_t data_raw = 0; // store 40 bits DHT data
    for (int iBit = 39; iBit >= 0; --iBit)
    {
        EXPECT(1);
        // monitor the duration signal stays 1
        int startTime = esp_timer_get_time();
        EXPECT(0);
        int endTime = esp_timer_get_time();

        // decide bit based on duration
        if (endTime - startTime > 50)
        {
            data_raw |= 1ULL << iBit;
        }
    }
    EXPECT(1);

    // parse data
    uint8_t h_i = (data_raw >> (8 * 4)) & 0xFF;
    uint8_t h_d = (data_raw >> (8 * 3)) & 0xFF;
    uint8_t t_i = (data_raw >> (8 * 2)) & 0xFF;
    uint8_t t_d = (data_raw >> (8 * 1)) & 0xFF;
    uint8_t crc = (data_raw >> (8 * 0)) & 0xFF;
    ESP_LOGI(TAG, "Raw bits %010llx", data_raw);
    if ((h_i + h_d + t_i + t_d) & 0xFF & !crc)
    {
        return ESP_ERR_INVALID_CRC;
    }

    data->humidity = ((h_i << 8) | h_d) / 10.0;
    data->temperature = (((t_i & 0b01111111) << 8) | t_d) / 10.0 * ((t_i & 0b10000000) ? -1.0 : 1.0);

    lastReading = *data;
    lastReadingTimeUs = esp_timer_get_time();

    return ESP_OK;
}

/**
 * 10k pull up resistor on the sensor input needed
 * SEQUENCE::
 * Start signal: MCU pulls low, waits >1 ms, pulls up, (waits 20-40 us)
 * Response signal: DHT pulls low for 80 us, DHT pulls high for 80 us,
 * Data signal: DHL pulls low for 50 us, pulls high for 26-27 us if 0, 70 us if 1
 * last bit: DHT pulls down for 50 us, resistor pulls up
 *
 * DATA FORMAT::
 * 40 bits
 * 5 bytes: humidity(integral,decimal), temperature(integral, decimal), checksum
 * |RH_i|RH_d|T_i|T_d|CS| where checksum, CS  = (RH_i+RH_d+T_i+T_d) | 0xFF(last 8 LSB)
 *
 **/

#pragma once

#include <stdint.h>
#include "esp_err.h"

#define DHT_PERIOD_SAMPLING_US 1     // us
#define DHT_PERIOD_TIMEOUT_US 100000 // us
#define DHT_N_SAMPLES_TIMEOUT (PERIOD_TIMEOUT / PERIOD_SAMPLING)

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct dht22_config_s
    {
        uint8_t pin;
    } dht22_config_t;

    typedef struct dht22_handle_s
    {
        uint8_t pin;
    } dht22_handle_t;

    typedef struct dht22_data_s
    {
        double humidity;
        double temperature;
    } dht22_data_t;

    esp_err_t dht22_init(dht22_config_t *, dht22_handle_t *);
    esp_err_t dht22_read(dht22_handle_t *, dht22_data_t *);

#ifdef __cplusplus
}
#endif

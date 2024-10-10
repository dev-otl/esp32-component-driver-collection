#include <stdio.h>
#include <sys/time.h>
#include "esp_log.h"
#include "esp_netif_sntp.h"

#include "clock.h"

#define ESP_SET_CHECK_LOG_GOTO(ret, func, msg, label) \
    {                                                 \
        ret = func;                                   \
        if (ret != ESP_OK)                            \
        {                                             \
            ESP_LOGE(TAG, msg);                       \
            goto label;                               \
        }                                             \
    }

#define ESP_SETRET_CHECK_LOG_GOTOERROR(func, msg) \
    {                                             \
        ret = func;                               \
        if (ret != ESP_OK)                        \
        {                                         \
            ESP_LOGE(TAG, msg);                   \
            goto error;                           \
        }                                         \
    }

static const char *TAG = "CLOCK";

esp_err_t clock_init(void)
{
    esp_err_t ret;
    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG(CLOCK_DEFAULT_NTP_SERVER);
    ESP_SETRET_CHECK_LOG_GOTOERROR(esp_netif_sntp_init(&config), "sntp init failed");
    ESP_SETRET_CHECK_LOG_GOTOERROR(esp_netif_sntp_sync_wait(pdMS_TO_TICKS(10000)), "sntp sync wait failed, failed update within 10s timeout");

error:
    return ret;
}

esp_err_t clock_get_unixtime(clock_data_t *clock_data)
{
    esp_err_t ret = ESP_OK;

    if (gettimeofday(&(clock_data->tv), NULL) == -1)
    {
        ESP_LOGE(TAG, "gettimeofday failed");
        ret = ESP_FAIL;
        goto error;
    }

    clock_data->unixtime = (clock_data->tv).tv_sec;

error:
    return ret;
}

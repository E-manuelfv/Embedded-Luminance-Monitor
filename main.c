#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define TAG "SENSOR_MONITOR"

// Ajuste para o pino que você estiver usando
#define SENSOR_GPIO GPIO_NUM_4

// Período de leitura (ms)
#define READ_INTERVAL_MS 1000

static void sensor_gpio_init(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << SENSOR_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    gpio_config(&io_conf);
}

void app_main(void)
{
    ESP_LOGI(TAG, "Embedded Sensor Monitor starting...");

    sensor_gpio_init();

    TickType_t last_wake_time = xTaskGetTickCount();

    while (true)
    {
        int sensor_value = gpio_get_level(SENSOR_GPIO);

        ESP_LOGI(TAG, "Sensor value: %d", sensor_value);

        vTaskDelayUntil(
            &last_wake_time,
            pdMS_TO_TICKS(READ_INTERVAL_MS)
        );
    }
}

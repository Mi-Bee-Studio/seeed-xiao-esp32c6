/*
 * blink — Seeed XIAO ESP32C6 基线工程（测试固件）。
 *
 * 板载 USER LED（GPIO15）每秒翻转；BOOT 键（GPIO9）按住常亮（交互
 * 自检），松开恢复闪烁；每 10 秒一条心跳日志（uptime/heap），供 serialtap
 * 持续采集验证。同构拷贝自 esp32-s3-zero/blink（共性先拷贝规范）。
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_task_wdt.h"
#include "driver/gpio.h"
#include "app_web.h"

static const char *TAG = "BLINK";

#define LED_GPIO  GPIO_NUM_15
#define BOOT_GPIO GPIO_NUM_9

static void led_set(bool on)
{
    gpio_set_level(LED_GPIO, on ? 1 : 0);
}

static bool boot_pressed(void)
{
    return gpio_get_level(BOOT_GPIO) == 0; // 按下接地
}

void app_main(void)
{
    const gpio_config_t led = {
        .pin_bit_mask = 1ULL << LED_GPIO,
        .mode = GPIO_MODE_OUTPUT,
    };
    ESP_ERROR_CHECK(gpio_config(&led));
    led_set(false);

    const gpio_config_t btn = {
        .pin_bit_mask = 1ULL << BOOT_GPIO,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&btn));

    ESP_LOGI(TAG, "blink ready: led=GPIO%d boot=GPIO%d heap=%uK",
             LED_GPIO, BOOT_GPIO,
             (unsigned)(esp_get_free_heap_size() / 1024));

    /* 板端维护页 :80（WiFi 配网 / OTA 刷机 / 状态），自带 APSTA 热点兜底 */
    app_web_init();

    /* 主循环看门狗：1s 一拍喂狗，卡死 >5s 触发 panic 重启自恢复 */
    esp_task_wdt_config_t wdt_cfg = {
        .timeout_ms = 5000,
        .idle_core_mask = 0,
        .trigger_panic = true,
    };
    esp_err_t werr = esp_task_wdt_init(&wdt_cfg);
    if (werr != ESP_OK && werr != ESP_ERR_INVALID_STATE) {
        ESP_ERROR_CHECK(werr);
    }
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));

    int beat = 0;
    while (true) {
        esp_task_wdt_reset();
        if (boot_pressed()) {
            led_set(true);
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
        static bool on;
        on = !on;
        led_set(on);
        if (++beat >= 10) {
            beat = 0;
            ESP_LOGI(TAG, "heartbeat uptime=%llds heap=%uK",
                     (long long)(esp_timer_get_time() / 1000000),
                     (unsigned)(esp_get_free_heap_size() / 1024));
        }
    }
}

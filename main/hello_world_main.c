#include <stdio.h>
#include "driver/gpio.h"
#include "led_strip.h"
#include "esp_log.h"
#include "math.h"

#define LED_GPIO_PIN  8
#define TAG "BLINK"

static led_strip_handle_t led_strip;

// Helper function to convert HSV to RGB
void hsv_to_rgb(uint16_t h, uint8_t s, uint8_t v, uint8_t *r, uint8_t *g, uint8_t *b) {
    float hh = h / 60.0f;
    int i = (int)hh;
    float ff = hh - i;
    float p = v * (1.0f - s / 255.0f);
    float q = v * (1.0f - (s / 255.0f) * ff);
    float t = v * (1.0f - (s / 255.0f) * (1.0f - ff));

    switch (i) {
        case 0: *r = v; *g = t; *b = p; break;
        case 1: *r = q; *g = v; *b = p; break;
        case 2: *r = p; *g = v; *b = t; break;
        case 3: *r = p; *g = q; *b = v; break;
        case 4: *r = t; *g = p; *b = v; break;
        case 5:
        default: *r = v; *g = p; *b = q; break;
    }
}

void app_main(void) {
    // Configure WS2812B LED    
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_GPIO_PIN,
        .max_leds = 1  // Only 1 LED
    };
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000  // 10MHz
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));

    uint16_t hue = 0;
    while (1) {
        uint8_t r, g, b;
        hsv_to_rgb(hue, 255, 255, &r, &g, &b);
        led_strip_set_pixel(led_strip, 0, r, g, b);
        led_strip_refresh(led_strip);
        ESP_LOGI(TAG, "Hue: %d, RGB: (%d, %d, %d)", hue, r, g, b);

        hue = (hue + 10) % 360;
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}
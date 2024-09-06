/*
 * Copyright (c) 2024 Charles Dias
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define SLEEP_TIME_MS   2000

#define LED_NODE DT_ALIAS(led0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED_NODE, gpios);

int main(void)
{
	int ret;
	bool led_state = true;

	if (!gpio_is_ready_dt(&led)) {
		return 0;
	}

	/* Teste também a configuração GPIO_OUTPUT_INACTIVE | GPIO_ACTIVE_LOW).*/
	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
	if (ret < 0) {
		return 0;
	}

	while (1) {
        
        led_state = true;
		ret = gpio_pin_set_dt(&led, led_state);
		if (ret < 0) {
			return 0;
		}
        
        printf("%6lld ms - LED state: %s\n", k_uptime_get(), led_state ? "ON" : "OFF");
		k_msleep(SLEEP_TIME_MS);

        led_state = false;
		ret = gpio_pin_set_dt(&led, led_state);
		if (ret < 0) {
			return 0;
		}
        
        printf("%6lld ms - LED state: %s\n", k_uptime_get(), led_state ? "ON" : "OFF");
		k_msleep(SLEEP_TIME_MS);
	}
	return 0;
}

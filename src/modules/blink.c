#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include "modules.h"

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

void module_blink_run(void)
{
	if (!gpio_is_ready_dt(&led)) {
		printk("led0 non pronto\n");
		return;
	}
	if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE) < 0) {
		return;
	}
	while (true) {
		gpio_pin_toggle_dt(&led);
		k_msleep(500);
	}
}


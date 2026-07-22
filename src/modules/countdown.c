#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include "modules.h"

#define LED0_NODE       DT_ALIAS(led0)
#define SW0_NODE        DT_ALIAS(sw0)
#define RED_LED_NODE    DT_NODELABEL(red_led_1)
#define GREEN_LED_NODE  DT_NODELABEL(green_led_1)

static const struct gpio_dt_spec button    = GPIO_DT_SPEC_GET(SW0_NODE, gpios);
static const struct gpio_dt_spec blue_led  = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec red_led   = GPIO_DT_SPEC_GET(RED_LED_NODE, gpios);
static const struct gpio_dt_spec green_led = GPIO_DT_SPEC_GET(GREEN_LED_NODE, gpios);

void module_countdown_run(void)
{
	int n = CONFIG_APP_MODULE_COUNTDOWN_START;
	if (!gpio_is_ready_dt(&blue_led) || !gpio_is_ready_dt(&green_led) ||
	    !gpio_is_ready_dt(&red_led) || !gpio_is_ready_dt(&button)) {
		printk("dispositivi non pronti\n");
		return;
	}

	if (gpio_pin_configure_dt(&button, GPIO_INPUT) < 0) {
		return;
	}
	if (gpio_pin_configure_dt(&green_led, GPIO_OUTPUT_ACTIVE) < 0) {
		return;
	}
	printk("Test countdown\n");
	printk("Premi il pulsante user per iniziare il test\n");
	while (gpio_pin_get_dt(&button) != 1){
		k_msleep(10);
	}
	while (n) {
		printk("T Minus %d\n", n);
		gpio_pin_toggle_dt(&green_led);
		k_msleep(1000);
		n--;
	}
	gpio_pin_configure_dt(&green_led, GPIO_OUTPUT_INACTIVE);
	gpio_pin_configure_dt(&blue_led, GPIO_OUTPUT_ACTIVE);
	gpio_pin_configure_dt(&red_led, GPIO_OUTPUT_INACTIVE);
	while (true) {
		gpio_pin_toggle_dt(&blue_led);
		gpio_pin_toggle_dt(&red_led);
		k_msleep(500);
	}
}

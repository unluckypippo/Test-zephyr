#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include "modules.h"

static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
static const struct gpio_dt_spec led =	GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
static struct gpio_callback button_cb;
static K_SEM_DEFINE(press_sem, 0, 1);

/* Questa funzione gira in contesto interrupt: si limita a segnalare il semaforo. */

static void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	static int64_t last;
	int64_t now = k_uptime_get();
	if (now - last < 50) {   /* debounce software 50 ms */
		return;
	}
	last = now;
	k_sem_give(&press_sem);
}
void module_button_i_run(void)
{
	if (!gpio_is_ready_dt(&button) || !gpio_is_ready_dt(&led)) {
		printk("dispositivi non pronti\n");
		return;
	}
	gpio_pin_configure_dt(&button, GPIO_INPUT);
	gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE);
	gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
	gpio_init_callback(&button_cb, button_pressed, BIT(button.pin));
	gpio_add_callback_dt(&button, &button_cb);
	printk("Premi il pulsante per accendere/spegnere il led\n");
	while (true) {
		k_sem_take(&press_sem, K_FOREVER);
		gpio_pin_toggle_dt(&led);
	}
}

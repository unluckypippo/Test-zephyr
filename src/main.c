#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include "modules.h"

int main(void)
{
	printk("=== Test Zephyr: avvio ===\n");

#ifdef CONFIG_APP_MODULE_COUNTDOWN
	printk("Modulo attivo: countdown\n");
	module_countdown_run();
#endif

#ifdef CONFIG_APP_MODULE_BLINK
	printk("Modulo attivo: blink\n");
	module_blink_run();
#endif

#ifdef CONFIG_APP_MODULE_BUTTON_I
	printk("Modulo attivo: button_i\n");
	module_button_i_run();
#endif

	printk("Nessun modulo attivo\n");
	return 0;
}

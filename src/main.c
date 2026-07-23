#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include "modules.h"

int main(void)
{
	printk("=== Test Zephyr: avvio ===\n");

	#if defined(CONFIG_APP_MODULE_COUNTDOWN)
		printk("Modulo attivo: countdown\n");
		module_countdown_run();
	#elif defined(CONFIG_APP_MODULE_BLINK)
		printk("Modulo attivo: blink\n");
		module_blink_run();
	#elif defined(CONFIG_APP_MODULE_BUTTON_I)
		printk("Modulo attivo: button_i\n");
		module_button_i_run();
	#elif defined(CONFIG_APP_MODULE_TEMP)
		printk("Modulo attivo: temp_read\n");
		module_temp_read_run();
	#elif defined(CONFIG_APP_MODULE_THREAD)
		printk("Modulo attivo: thread_mg\n");
		module_thread_run();
	#else
		printk("Nessun modulo attivo\n");
	#endif
		return 0;

}

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include "modules.h"

//Esercizio sulla gestione della memoria 
/*
static const uint8_t buffer_const[1024] = {1};   // atteso: flash (.rodata)
static uint8_t buffer_bss[1024];                  // atteso: RAM (.bss)
static uint8_t buffer_data[1024] = {1};           // atteso: RAM (.data) + flash
*/

int main(void)
{
/*
	buffer_bss[0] = buffer_const[0];
    buffer_data[0] = buffer_bss[0];
    printk("const=%p bss=%p data=%p\n",
           (void *)buffer_const, (void *)buffer_bss, (void *)buffer_data);
*/
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
	#elif defined(CONFIG_APP_MODULE_LOG)
		printk("Modulo attivo: logging_system\n");
		module_log_run();
	#else
		printk("Nessun modulo attivo\n");
	#endif

	
		return 0;

}

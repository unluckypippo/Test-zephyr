#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include "modules.h"

static const int32_t blink_sleep = 500;

#define BLINK_THREAD_STACK_SIZE 512
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

void blink_thread_start(void *p1, void *p2, void *p3){
    ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);
    uint32_t count=0;
    if (!gpio_is_ready_dt(&led)) {
        printk("led0 non pronto\n");
        return;
    }
    if (gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE) < 0) {
        return;
    }

    printk("thread di monitoraggio blink iniziato\n");
    while(true){
        gpio_pin_toggle_dt(&led);
        k_msleep(blink_sleep);
        printk("Contatore: %u\n", count++);
    }

}

K_THREAD_DEFINE(blink_thread, BLINK_THREAD_STACK_SIZE, blink_thread_start, NULL, NULL, NULL, 7, 0, 0);

void module_thread_run(void){
	printk("modulo thread attivo: il thread blink gira gia'\n");
	k_sleep(K_FOREVER);
}
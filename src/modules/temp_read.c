#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/sensor.h>
#include "modules.h"

#define SW0_NODE DT_ALIAS(sw0)

static const struct device *const die_temp = DEVICE_DT_GET(DT_NODELABEL(die_temp));
static const struct gpio_dt_spec button  = GPIO_DT_SPEC_GET(SW0_NODE, gpios);

void module_temp_read_run(void){
	struct sensor_value temp;
	int ret;
	if(!device_is_ready(die_temp)){
		printk("Il sensore di temperatura non è pronto\n");
		return;
	}
	if(!gpio_is_ready_dt(&button)){
                printk("Il bottone non è pronto\n");
                return;
	}
	gpio_pin_configure_dt(&button, GPIO_INPUT);
	printk("Premi il button per leggere la temperatura\n");
	while(true){
	        while(gpio_pin_get_dt(&button)!= 1){
		        k_msleep(10);
	        }
	        ret = sensor_sample_fetch(die_temp);
	        if(ret<0){return;}
	        else{
		        ret = sensor_channel_get(die_temp, SENSOR_CHAN_DIE_TEMP, &temp);
		        if(ret<0){return;}
		        printk("Temperatura = %d.%06d C\n",temp.val1,temp.val2);
	        }
	        while (gpio_pin_get_dt(&button) == 1){
	                k_msleep(20);
	        }
	}
	
}

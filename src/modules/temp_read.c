#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/sensor.h>
#include "modules.h"

#define SW0_NODE DT_ALIAS(sw0)

static const struct device *const die_temp = DEVICE_DT_GET(DT_NODELABEL(die_temp));
static const struct device *const vref = DEVICE_DT_GET(DT_NODELABEL(vref));
static const struct device *const vbat = DEVICE_DT_GET(DT_NODELABEL(vbat));
static const struct gpio_dt_spec button  = GPIO_DT_SPEC_GET(SW0_NODE, gpios);

void module_temp_read_run(void){
	struct sensor_value temp;
	struct sensor_value tref;
	struct sensor_value tbat;

	int ret_T, ret_Vr, ret_Vb;
	if(!device_is_ready(die_temp) && !device_is_ready(vref) && !device_is_ready(vbat)){
		printk("I sensori non sono pronti\n");
		return;
	}
	if(!gpio_is_ready_dt(&button)){
                printk("Il bottone non è pronto\n");
                return;
	}
	gpio_pin_configure_dt(&button, GPIO_INPUT);
	printk("Premi il button per leggere i sensori \n");
	while(true){
	        while(gpio_pin_get_dt(&button)!= 1){
		        k_msleep(10);
	        }
	        ret_T = sensor_sample_fetch(die_temp);
			ret_Vb = sensor_sample_fetch(vbat);
			ret_Vr = sensor_sample_fetch(vref);
	        if(ret_T<0 && ret_Vb<0 && ret_Vr<0){return;}
	        else{
		        ret_T = sensor_channel_get(die_temp, SENSOR_CHAN_DIE_TEMP, &temp);
				ret_Vb = sensor_channel_get(vbat, SENSOR_CHAN_VOLTAGE, &tbat);
				ret_Vr = sensor_channel_get(vref, SENSOR_CHAN_VOLTAGE, &tref);
		        if(ret_T<0 && ret_Vb<0 && ret_Vr<0){return;}
		        printk("Temperatura del silicio del chip = %d.%06d C\n",temp.val1,temp.val2);
				printk("Tensione di riferimento interna = %d.%06d V\n",tref.val1,tref.val2);
				printk("Tensione della batteria di backup (VBAT) = %d.%06d V\n",tbat.val1,tbat.val2);

	        }
	        while (gpio_pin_get_dt(&button) == 1){
	                k_msleep(20);
	        }
	}
	
}

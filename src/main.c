#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define LED0_NODE DT_ALIAS(led0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

int main(void)
{
	int n = 10, l = 11;
	int ret;
	bool led_state = true;
	printf("Test countdown \n");
	
	while (n) {
		printf("T Minus %d \n", n);
		k_msleep(1000);
		n --;
	}
	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
	while (l){
		ret = gpio_pin_toggle_dt(&led);
		if(ret<0){
			return 0;
		}
		led_state = !led_state;
                printf("LED state: %s\n", led_state ? "ON" : "OFF");
                k_msleep(500);
		l --;
	}

	return 0;
}

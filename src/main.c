#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define LED0_NODE DT_ALIAS(led0)
#define SW0_NODE DT_ALIAS(sw0)
#define RED_LED_NODE DT_NODELABEL(red_led_1)

static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(SW0_NODE, gpios);
static const struct gpio_dt_spec blue_led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec red_led = GPIO_DT_SPEC_GET(RED_LED_NODE, gpios);

int main(void)
{
        int n = 10, l = 11;
        int ret_bl, ret_rl, ret_b;
        bool led_state = true;
	if (!gpio_is_ready_dt(&blue_led) || !gpio_is_ready_dt(&red_led) || !gpio_is_ready_dt(&button) ){
		printf("dispositivi non pronti \n");
		return 0;
	}

        ret_b = gpio_pin_configure_dt(&button, GPIO_INPUT);
        printf("Test countdown \n");
        printf("Premi il pulsante user per iniziare il test\n");
        if (ret_b<0){
                return 0;
                }
        while (true){
		bool inpt=0;
                inpt = gpio_pin_get_dt(&button);
                if (inpt==1){
                        while (n) {
                                printf("T Minus %d \n", n);
                                k_msleep(1000);
                                n --;
                        }
                        ret_bl = gpio_pin_configure_dt(&blue_led, GPIO_OUTPUT_ACTIVE);
			ret_rl = gpio_pin_configure_dt(&red_led, GPIO_OUTPUT_INACTIVE);
                        while (l){
                                ret_bl = gpio_pin_toggle_dt(&blue_led);
				ret_rl = gpio_pin_toggle_dt(&red_led);
                                if(ret_bl<0 || ret_rl<0){
                                        return 0;
                                }
                                led_state = !led_state;
                                k_msleep(500);
                                l --;
                        }
                }
        }
        return 0;
}



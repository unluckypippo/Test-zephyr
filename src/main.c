#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define LED0_NODE DT_ALIAS(led0)
#define SW0_NODE DT_ALIAS(sw0)


static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(SW0_NODE, gpios);
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

int main(void)
{
        int n = 10, l = 11;
        int ret_l, ret_b;
        bool led_state = true;
        ret_b = gpio_pin_configure_dt(&button, GPIO_INPUT);
        printf("Test countdown \n");
        printf("Premi il pulsante user per iniziare il test\n");
        if (ret_b<0){
                return 0;
                }
        while (true){
                int inpt = gpio_pin_get_dt(&button);
                if (inpt==1){
                        while (n) {
                                printf("T Minus %d \n", n);
                                k_msleep(1000);
                                n --;
                        }
                        ret_l = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
                        while (l){
                                ret_l = gpio_pin_toggle_dt(&led);
                                if(ret_l<0){
                                        return 0;
                                }
                                led_state = !led_state;
                                printf("LED state: %s\n", led_state ? "ON" : "OFF");
                                k_msleep(500);
                                l --;
                        }
                }
        }
        return 0;
}



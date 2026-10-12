#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define LED0_NODE DT_ALIAS(led0)

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);


void led_timer_function(struct k_timer *timer_id)
{
        gpio_pin_toggle_dt(&led);
}

K_TIMER_DEFINE(my_timer, led_timer_function, NULL);

int main(void)
{
        if (!device_is_ready(led.port)) {
                return -1;
        }

        gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);

        k_timer_start(&my_timer, K_MSEC(500), K_MSEC(1000));

        return 0;
}

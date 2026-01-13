#include "app_main.h"

swt_device_t swt_device[DEVICE_SWITCH_MAX];

device_switch_t device_type = DEVICE_SWITCH_NONE;

swt_device_t *cur_device = &swt_device[DEVICE_SWITCH_1];

static void device_gpio_init(device_gpio_t *device_gpio) {
    drv_gpio_func_set(device_gpio->gpio);
    drv_gpio_output_en(device_gpio->gpio, device_gpio->output);
    drv_gpio_input_en(device_gpio->gpio, device_gpio->input);
    if (device_gpio->pull) drv_gpio_up_down_resistor(device_gpio->gpio, device_gpio->pull);

}

void device_init() {
    uint8_t devi = DEVICE_SWITCH_NONE;

    memset(&swt_device, 0, sizeof(swt_device));

    /* None device */
    swt_device[devi++].device_en = OFF;

    /* jr_zds01 Gerier _TZ3000_6axxqqi2 */
    swt_device[devi].device_en = ON;
    swt_device[devi].button_max = 1;
    swt_device[devi].button_gpio.gpio = GPIO_PC2;
    swt_device[devi].button_gpio.input = ON;
    swt_device[devi].button_gpio.output = OFF;
    swt_device[devi].button_gpio.func = AS_GPIO;
    swt_device[devi].button_gpio.pull = PM_PIN_PULLUP_10K;
    device_gpio_init(&swt_device[devi].button_gpio);
    swt_device[devi].led_gpio.gpio = GPIO_PB5;
    swt_device[devi].led_gpio.input = OFF;
    swt_device[devi].led_gpio.output = ON;
    swt_device[devi].led_gpio.func = AS_GPIO;
    swt_device[devi].len_on = 0;
    swt_device[devi].len_off = 1;
    device_gpio_init(&swt_device[devi].led_gpio);
    swt_device[devi].switch_gpio.gpio = GPIO_PB4;
    swt_device[devi].switch_gpio.input = ON;
    swt_device[devi].switch_gpio.output = OFF;
    swt_device[devi].switch_gpio.func = AS_GPIO;
    swt_device[devi].switch_gpio.pull = PM_PIN_PULLDOWN_100K;
    swt_device[devi].switch_debounce = 64;
    device_gpio_init(&swt_device[devi].switch_gpio);
    swt_device[devi].relay_gpio.gpio = GPIO_PD2;
    swt_device[devi].relay_gpio.input = ON;
    swt_device[devi].relay_gpio.output = ON;
    swt_device[devi].relay_on = 1;
    swt_device[devi].relay_off = 0;
    device_gpio_init(&swt_device[devi].relay_gpio);
    swt_device[devi].uart_debug_gpio.gpio = GPIO_PD3;
    swt_device[devi].uart_debug_en = ON;

    devi++;

    /* zg301z Hobean */
    swt_device[devi].device_en = ON;
    swt_device[devi].button_max = 1;
    swt_device[devi].button_gpio.gpio = GPIO_PB1;
    swt_device[devi].button_gpio.input = ON;
    swt_device[devi].button_gpio.output = OFF;
    swt_device[devi].button_gpio.func = AS_GPIO;
    swt_device[devi].button_gpio.pull = PM_PIN_PULLUP_10K;
    device_gpio_init(&swt_device[devi].button_gpio);
    swt_device[devi].led_gpio.gpio = GPIO_PD4;
    swt_device[devi].led_gpio.input = OFF;
    swt_device[devi].led_gpio.output = ON;
    swt_device[devi].led_gpio.func = AS_GPIO;
    swt_device[devi].len_on = 0;
    swt_device[devi].len_off = 1;
    device_gpio_init(&swt_device[devi].led_gpio);
    swt_device[devi].switch_gpio.gpio = GPIO_PB6;
    swt_device[devi].switch_gpio.input = ON;
    swt_device[devi].switch_gpio.output = OFF;
    swt_device[devi].switch_gpio.func = AS_GPIO;
    swt_device[devi].switch_gpio.pull = PM_PIN_PULLDOWN_100K;
    swt_device[devi].switch_debounce = 128;
    device_gpio_init(&swt_device[devi].switch_gpio);
    swt_device[devi].relay_gpio.gpio = GPIO_PA1;
    swt_device[devi].relay_gpio.input = ON;
    swt_device[devi].relay_gpio.output = ON;;
    swt_device[devi].relay_on = 1;
    swt_device[devi].relay_off = 0;
    device_gpio_init(&swt_device[devi].relay_gpio);
    swt_device[devi].uart_debug_gpio.gpio = GPIO_PC2;
    swt_device[devi].uart_debug_en = ON;

}

#include "app_main.h"

swt_device_t swt_device[DEVICE_SWITCH_MAX];

swt_device_t *cur_device = &swt_device[1];

void device_init() {
    uint8_t devi = DEVICE_SWITCH_NONE;

    swt_device[devi++].device_en = false;

    swt_device[devi].device_en = true;
    swt_device[devi].button_max = 1;
    swt_device[devi].button_gpio.gpio = GPIO_PC2;
    swt_device[devi].button_gpio.input = true;
    swt_device[devi].button_gpio.output = false;
    swt_device[devi].button_gpio.func = AS_GPIO;
    swt_device[devi].button_gpio.pull = PM_PIN_PULLUP_10K;
    swt_device[devi].led_gpio.gpio = GPIO_PB5;
    swt_device[devi].led_gpio.input = false;
    swt_device[devi].led_gpio.output = true;
    swt_device[devi].led_gpio.func = AS_GPIO;
    swt_device[devi].len_on = 0;
    swt_device[devi].len_off = 1;
    swt_device[devi].switch_gpio.gpio = GPIO_PB4;
    swt_device[devi].switch_gpio.input = true;
    swt_device[devi].switch_gpio.output = false;
    swt_device[devi].switch_gpio.func = AS_GPIO;
    swt_device[devi].switch_gpio.pull = PM_PIN_PULLDOWN_100K;
    swt_device[devi].switch_debounce = 64;
    swt_device[devi].relay_gpio.gpio = GPIO_PD2;
    swt_device[devi].relay_gpio.input = true;
    swt_device[devi].relay_gpio.output = true;;
    swt_device[devi].relay_on = 1;
    swt_device[devi].relay_off = 0;
    swt_device[devi].uart_debug_gpio.gpio = GPIO_PD3;
    swt_device[devi++].uart_debug_en = true;

    swt_device[devi].device_en = true;
    swt_device[devi].button_max = 1;
    swt_device[devi].button_gpio.gpio = GPIO_PB1;
    swt_device[devi].button_gpio.input = true;
    swt_device[devi].button_gpio.output = false;
    swt_device[devi].button_gpio.func = AS_GPIO;
    swt_device[devi].button_gpio.pull = PM_PIN_PULLUP_10K;
    swt_device[devi].led_gpio.gpio = GPIO_PD4;
    swt_device[devi].led_gpio.input = false;
    swt_device[devi].led_gpio.output = true;
    swt_device[devi].led_gpio.func = AS_GPIO;
    swt_device[devi].len_on = 0;
    swt_device[devi].len_off = 1;
    swt_device[devi].switch_gpio.gpio = GPIO_PB6;
    swt_device[devi].switch_gpio.input = true;
    swt_device[devi].switch_gpio.output = false;
    swt_device[devi].switch_gpio.func = AS_GPIO;
    swt_device[devi].switch_gpio.pull = PM_PIN_PULLDOWN_100K;
    swt_device[devi].switch_debounce = 128;
    swt_device[devi].relay_gpio.gpio = GPIO_PA1;
    swt_device[devi].relay_gpio.input = true;
    swt_device[devi].relay_gpio.output = true;;
    swt_device[devi].relay_on = 1;
    swt_device[devi].relay_off = 0;
    swt_device[devi].uart_debug_gpio.gpio = GPIO_PC2;
    swt_device[devi++].uart_debug_en = true;

}

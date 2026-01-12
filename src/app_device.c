#include "app_main.h"

swt_device_t swt_device[DEVICE_SWITCH_MAX];
//                      = {
//        {
//                .device_en = false,
//        },
//        {
//                .device_en = true,
//                .button_max = 1,
//                .button_gpio.gpio = GPIO_PC2,
//                .button_gpio.input = true,
//                .button_gpio.output = false,
//                .button_gpio.gpio_func = AS_GPIO,
//                .button_gpio.pull = PM_PIN_PULLUP_10K,
//        },
//};

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
}

#include "app_main.h"

#define ADDR_DEVICE_SWITCH_CFG  0x75000
#define ID_DEVICE_SWITCH_CFG    0x1465

typedef struct {
    uint16_t id;
    uint8_t  device_model;
    uint8_t  crc;
} config_switch_model_t;

switch_device_t switch_device[DEVICE_SWITCH_MAX];
device_switch_model_t device_switch_model = DEVICE_MODEL;
switch_device_t *device = &switch_device[DEVICE_SWITCH_NONE];

static void device_gpio_init(device_gpio_t *device_gpio) {

    drv_gpio_func_set(device_gpio->gpio);
    drv_gpio_output_en(device_gpio->gpio, device_gpio->output);
    drv_gpio_input_en(device_gpio->gpio, device_gpio->input);
    if (device_gpio->pull) drv_gpio_up_down_resistor(device_gpio->gpio, device_gpio->pull);
}

static void device_model_init() {

    device = &switch_device[device_switch_model];
    gpio_init(false);
    if (device_switch_model != DEVICE_SWITCH_NONE) {
        device_gpio_init(&device->button_gpio);
        device_gpio_init(&device->led_gpio);
        device_gpio_init(&device->relay_gpio);
        if (device_switch_model == DEVICE_SWITCH_2) {
            drv_adc_init();
            drv_adc_mode_pin_set(DRV_ADC_BASE_MODE, device->switch_gpio.gpio);
            drv_gpio_up_down_resistor(device->switch_gpio.gpio, PM_PIN_PULLUP_10K);
        } else
            device_gpio_init(&device->switch_gpio);
#if UART_PRINTF_MODE
        device_gpio_init(&device->debug_gpio);
        gpio_write(device->debug_gpio.gpio, 1);
#endif
        light_init();
//        light_off();
        dev_relay_init();
    }
    kb_drv_init();
    switch_init();
}

void device_model_restore() {

    config_switch_model_t model_cfg;

    flash_read_page(ADDR_DEVICE_SWITCH_CFG, sizeof(config_switch_model_t), (uint8_t*)&model_cfg);

    if (model_cfg.id == ID_DEVICE_SWITCH_CFG && model_cfg.crc == checksum((uint8_t*)&model_cfg, sizeof(config_switch_model_t)-1)) {
        device_switch_model = model_cfg.device_model;
    } else {
        device_switch_model = DEVICE_MODEL;
    }

    device_model_init();
}

void device_model_save(uint8_t model) {
    config_switch_model_t model_cfg;

    model_cfg.id = ID_DEVICE_SWITCH_CFG;
    device_switch_model = model_cfg.device_model = model;

    flash_erase(ADDR_DEVICE_SWITCH_CFG);
    model_cfg.crc = checksum((uint8_t*)&(model_cfg), sizeof(config_switch_model_t)-1);
    flash_write(ADDR_DEVICE_SWITCH_CFG, sizeof(config_switch_model_t), (uint8_t*)&(model_cfg));

    device_model_init();
}

void device_init() {
    uint8_t devi = DEVICE_SWITCH_NONE;

    memset(&switch_device, 0, sizeof(switch_device));

    /* None device */
    switch_device[devi++].device_en = OFF;

    /* jr_zds01 Gerier _TZ3000_6axxqqi2 */
    switch_device[devi].device_en = ON;
    switch_device[devi].button_max = 1;
    switch_device[devi].button_gpio.gpio = GPIO_PC2;
    switch_device[devi].button_gpio.input = ON;
    switch_device[devi].button_gpio.output = OFF;
    switch_device[devi].button_gpio.func = AS_GPIO;
    switch_device[devi].button_gpio.pull = PM_PIN_PULLUP_10K;
    switch_device[devi].led_gpio.gpio = GPIO_PB5;
    switch_device[devi].led_gpio.input = OFF;
    switch_device[devi].led_gpio.output = ON;
    switch_device[devi].led_gpio.func = AS_GPIO;
    switch_device[devi].len_on = 0;
    switch_device[devi].len_off = 1;
    switch_device[devi].switch_gpio.gpio = GPIO_PB4;
    switch_device[devi].switch_gpio.input = ON;
    switch_device[devi].switch_gpio.output = OFF;
    switch_device[devi].switch_gpio.func = AS_GPIO;
    switch_device[devi].switch_gpio.pull = PM_PIN_PULLDOWN_100K;
    switch_device[devi].switch_debounce = 64;
    switch_device[devi].relay_gpio.gpio = GPIO_PD2;
    switch_device[devi].relay_gpio.input = ON;
    switch_device[devi].relay_gpio.output = ON;
    switch_device[devi].relay_gpio.func = AS_GPIO;
    switch_device[devi].relay_on = 1;
    switch_device[devi].relay_off = 0;
    switch_device[devi].debug_gpio.gpio = GPIO_PD3;
    switch_device[devi].debug_gpio.input = OFF;
    switch_device[devi].debug_gpio.output = ON;
    switch_device[devi].debug_gpio.func = AS_GPIO;
    switch_device[devi].debug_gpio.pull = PM_PIN_PULLUP_1M;

    devi++;

    /* zg301z Hobeian */
    switch_device[devi].device_en = ON;
    switch_device[devi].button_max = 1;
    switch_device[devi].button_gpio.gpio = GPIO_PB1;
    switch_device[devi].button_gpio.input = ON;
    switch_device[devi].button_gpio.output = OFF;
    switch_device[devi].button_gpio.func = AS_GPIO;
    switch_device[devi].button_gpio.pull = PM_PIN_PULLUP_10K;
    switch_device[devi].led_gpio.gpio = GPIO_PD4;
    switch_device[devi].led_gpio.input = OFF;
    switch_device[devi].led_gpio.output = ON;
    switch_device[devi].led_gpio.func = AS_GPIO;
    switch_device[devi].len_on = 0;
    switch_device[devi].len_off = 1;
    switch_device[devi].switch_gpio.gpio = GPIO_PB6;
    switch_device[devi].switch_gpio.input = ON;
    switch_device[devi].switch_gpio.output = OFF;
    switch_device[devi].switch_gpio.func = AS_GPIO;
    switch_device[devi].switch_gpio.pull = PM_PIN_PULLUP_10K;
    switch_device[devi].switch_debounce = 16;
    switch_device[devi].relay_gpio.gpio = GPIO_PA1;
    switch_device[devi].relay_gpio.input = ON;
    switch_device[devi].relay_gpio.output = ON;;
    switch_device[devi].relay_gpio.func = AS_GPIO;
    switch_device[devi].relay_on = 1;
    switch_device[devi].relay_off = 0;
    switch_device[devi].debug_gpio.gpio = UART_TX_PC2;
    switch_device[devi].debug_gpio.input = OFF;
    switch_device[devi].debug_gpio.output = ON;
    switch_device[devi].debug_gpio.func = AS_GPIO;
    switch_device[devi].debug_gpio.pull = PM_PIN_PULLUP_1M;

    device_model_restore();
}

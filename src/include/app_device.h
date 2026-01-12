#ifndef SRC_INCLUDE_APP_DEVICE_H_
#define SRC_INCLUDE_APP_DEVICE_H_

//enum {
//    VK_SW1 = 0x01,
//    VK_SW2 = 0x02,
//    VK_SW_MAX
//};

typedef enum {
    DEVICE_SWITCH_NONE  = 0,
    DEVICE_SWITCH_1,
    DEVICE_SWITCH_2,
    DEVICE_SWITCH_MAX
} device_switch_t;

typedef struct {
    uint32_t            gpio;
    bool                input;
    bool                output;
    GPIO_FuncTypeDef    func;
    GPIO_PullTypeDef    pull;
} device_gpio_t;

typedef struct {
    bool                device_en;
    uint8_t             button_max;
    device_gpio_t       button_gpio;
    device_gpio_t       led_gpio;
//    uint8_t             switch_max;
    device_gpio_t       switch_gpio;
    device_gpio_t       relay_gpio;
    bool                uart_debug_en;
    device_gpio_t       uart_debug_gpio;
} swt_device_t;

extern swt_device_t swt_device[DEVICE_SWITCH_MAX];
void device_init();

#endif /* SRC_INCLUDE_APP_DEVICE_H_ */

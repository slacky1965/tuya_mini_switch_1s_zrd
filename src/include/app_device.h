#ifndef SRC_INCLUDE_APP_DEVICE_H_
#define SRC_INCLUDE_APP_DEVICE_H_

enum {
    VK_SW1 = 0x01,
    VK_SW2 = 0x02,
    VK_SW_MAX
};

#define MAX_BUTTON_NUM  1

#define DEVICE_EN(d)    (d?1:0)
#define LED_ON(d)       (d?1:0)
#define LED_OFF(d)      (d?1:0)
#define RELAY_ON(d)     (d?1:0)
#define RELAY_OFF(d)    (d?1:0)

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

typedef struct __attribute__((packed)) {
    uint8_t             device_en :1;
    uint8_t             len_on :1;
    uint8_t             len_off :1;
    uint8_t             relay_on :1;
    uint8_t             relay_off :1;
    uint8_t             uart_debug_en :1;
    uint8_t             reserve :2;
    uint8_t             button_max;
    device_gpio_t       button_gpio;
    device_gpio_t       led_gpio;
    device_gpio_t       switch_gpio;
    uint8_t             switch_debounce;
    device_gpio_t       relay_gpio;
    device_gpio_t       uart_debug_gpio;
} swt_device_t;

extern swt_device_t swt_device[DEVICE_SWITCH_MAX];
extern swt_device_t *cur_device;
void device_init();

#endif /* SRC_INCLUDE_APP_DEVICE_H_ */

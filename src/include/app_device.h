#ifndef SRC_INCLUDE_APP_DEVICE_H_
#define SRC_INCLUDE_APP_DEVICE_H_

#define DEVICE_EN(d)    (d?1:0)
#define LED_ON(d)       (d?1:0)
#define LED_OFF(d)      (d?1:0)
#define RELAY_ON(d)     (d?1:0)
#define RELAY_OFF(d)    (d?1:0)

typedef enum {
    DEVICE_SWITCH_NONE  = 0,
    DEVICE_SWITCH_1,                        /* jr_zds01 Gerier _TZ3000_6axxqqi2 */
    DEVICE_SWITCH_2,                        /* zg301z Hobean                    */
    DEVICE_SWITCH_MAX
} device_switch_model_t;

typedef struct __attribute__((packed)) {
    uint32_t            gpio;
    uint8_t             input;
    uint8_t             output;
    GPIO_FuncTypeDef    func;
    GPIO_PullTypeDef    pull;
} device_gpio_t;

typedef struct __attribute__((packed)) {
    uint8_t             device_en :1;
    uint8_t             len_on :1;
    uint8_t             len_off :1;
    uint8_t             relay_on :1;
    uint8_t             relay_off :1;
    uint8_t             reserve :3;
    uint8_t             button_max;
    device_gpio_t       button_gpio;
    device_gpio_t       led_gpio;
    device_gpio_t       switch_gpio;
    uint8_t             switch_debounce;
    device_gpio_t       relay_gpio;
    device_gpio_t       debug_gpio;
} switch_device_t;

extern device_switch_model_t device_switch_model;
extern switch_device_t switch_device[DEVICE_SWITCH_MAX];
extern switch_device_t *device;

void device_model_restore();
void device_model_save(uint8_t model);
void device_init();

#endif /* SRC_INCLUDE_APP_DEVICE_H_ */

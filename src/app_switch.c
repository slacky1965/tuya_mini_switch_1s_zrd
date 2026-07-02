#include "app_main.h"

#ifndef DEBOUNCE_SWITCH
#define DEBOUNCE_SWITCH     128     /* number of polls for debounce                 */
#endif
#define FR_COUNTER_MAX      10      /* number for factory reset                     */

#define THRESHOLD_MODEL_2   500
#define THRESHOLD_MODEL_3   600

typedef enum {
    SWITCH_OPEN = 0,
    SWITCH_CLOSED,
    SWITCH_FLOAT,
} switch_status_t;

typedef struct {
    uint8_t     status :2;
    uint8_t     released :1;
    uint8_t     pressed :1;
    uint8_t     hold: 1;
    uint8_t     level_dir :1;
    uint8_t     color_dir :1;
    uint8_t     color_used :1;
    uint8_t     counter;
    uint8_t     debounce;
    uint32_t    pressed_time;
    uint32_t    released_time;
    uint32_t    hold_time;
} app_switch_t;

static app_switch_t app_switch[AMT_RELAY];

static int32_t net_steer_start_offCb(void *args) {

    g_appCtx.net_steer_start = false;

    light_blink_stop();

    return -1;
}

static void switch_factory_reset_start(void *args) {

//    printf("Factory reset\r\n");

    zb_factoryReset();

    g_appCtx.net_steer_start = true;
    TL_ZB_TIMER_SCHEDULE(net_steer_start_offCb, NULL, TIMEOUT_1MIN30SEC);
    light_blink_start(90, 250, 750);

}

static void switch_first_start(uint8_t i, switch_status_t status) {

    switch(relay_settings.startUpOnOff[i]) {
        case ZCL_START_UP_ONOFF_SET_ONOFF_TO_PREVIOUS:
            if (status_onoff.status_onoff[i]) cmdOnOff_on(dev_relay.unit_relay[i].ep);
            else cmdOnOff_off(dev_relay.unit_relay[i].ep);
            break;
        case ZCL_START_UP_ONOFF_SET_ONOFF_TOGGLE:
            cmdOnOff_toggle(dev_relay.unit_relay[i].ep);
            break;
        case ZCL_START_UP_ONOFF_SET_ONOFF_TO_ON:
            cmdOnOff_on(dev_relay.unit_relay[i].ep);
            break;
        case ZCL_START_UP_ONOFF_SET_ONOFF_TO_OFF:
            cmdOnOff_off(dev_relay.unit_relay[i].ep);
            break;
        default:
            cmdOnOff_off(dev_relay.unit_relay[i].ep);
            break;
    }
    app_switch[i].status = status;
}

//static void test(uint8_t i) {
//
//    static uint32_t counter = 0;
////    drv_adc_mode_pin_set(DRV_ADC_BASE_MODE, dev_relay.unit_relay[i].sw);
//    drv_adc_enable(ON);
//
//    uint16_t key_level = drv_get_adc_data();
//
//    drv_adc_enable(OFF);
//
////    if (key_level < 80)
//
//
//        printf("key_level: %d\r\n", key_level);
//
//
////    if (!drv_gpio_read(dev_relay.unit_relay[i].sw)) {
////        printf("Level Low, time: %d\r\n", clock_time());
//////        printf("Level Low: %d\r\n", counter++);
//////        printf("Level Low\r\n");
////    } else {
////        counter = 0;
//////        printf("Level High\r\n");
////    }
////
//
//
//    sleep_ms(30);
//}

static bool switch_gpio_read(uint32_t pin) {
    uint16_t threshold = THRESHOLD_MODEL_2;
    if (device_switch_model == DEVICE_SWITCH_2 || device_switch_model == DEVICE_SWITCH_3) {
        drv_adc_enable(ON);
        uint16_t key_level = drv_get_adc_data();
        drv_adc_enable(OFF);
        switch(device_switch_model) {
            case DEVICE_SWITCH_2:
                threshold = THRESHOLD_MODEL_2;
                break;
            case DEVICE_SWITCH_3:
                threshold = THRESHOLD_MODEL_3;
                break;
            default:
                break;
        }
        return (key_level < threshold)?false:true;

    }
    return drv_gpio_read(pin)?true:false;
}

static void read_switch_toggle(uint8_t i) {

    app_switch_t *p_switch = &app_switch[i];

    if (!switch_gpio_read(dev_relay.unit_relay[i].sw)) {
        if (p_switch->status == SWITCH_FLOAT) {
            switch_first_start(i, SWITCH_CLOSED);
        } else if (p_switch->debounce != DEBOUNCE_SWITCH) {
            p_switch->debounce++;
            if (p_switch->debounce == 5 && !zb_isDeviceJoinedNwk() && !zb_isDeviceFactoryNew()) {
                zb_rejoinReq(zb_apsChannelMaskGet(), g_bdbAttrs.scanDuration);
                APP_DEBUG(UART_PRINTF_MODE, "Device not joined. Rejoin\r\n");
            }
            if (p_switch->debounce == DEBOUNCE_SWITCH && p_switch->status != SWITCH_CLOSED) {
                p_switch->status = SWITCH_CLOSED;
#if UART_PRINTF_MODE && DEBUG_SWITCH_EN
                APP_DEBUG(DEBUG_SWITCH_EN, "Key %d pressed toggle\r\n", i+1);
#endif
                if (p_switch->counter == 0) {
                    p_switch->counter++;
                    p_switch->pressed = true;
#if UART_PRINTF_MODE && DEBUG_SWITCH_EN
                    APP_DEBUG(DEBUG_SWITCH_EN, "Switch %d is CLOSED\r\n", dev_relay.unit_relay[i].ep);
#endif
                    if (relay_settings.switchType[i] == ZCL_SWITCH_TYPE_MOMENTARY) {
                        if (relay_settings.switchActions[i] == ZCL_SWITCH_ACTION_OFF_ON) {
                            if (relay_settings.switch_decoupled[i] == CUSTOM_SWITCH_DECOUPLED_OFF) {
                                cmdOnOff_on(dev_relay.unit_relay[i].ep);
                            } else {
                                remoteCmdOnOff(dev_relay.unit_relay[i].ep, ZCL_CMD_ONOFF_ON);
                            }
                        } else {
                            if (relay_settings.switch_decoupled[i] == CUSTOM_SWITCH_DECOUPLED_OFF) {
                                cmdOnOff_off(dev_relay.unit_relay[i].ep);
                            } else {
                                remoteCmdOnOff(dev_relay.unit_relay[i].ep, ZCL_CMD_ONOFF_OFF);
                            }
                        }
                    } else {
                        if (relay_settings.switch_decoupled[i] == CUSTOM_SWITCH_DECOUPLED_OFF) {
                            cmdOnOff_toggle(dev_relay.unit_relay[i].ep);
                        } else {
                            remoteCmdOnOff(dev_relay.unit_relay[i].ep, ZCL_CMD_ONOFF_TOGGLE);
                        }
                    }
                } else if (p_switch->pressed && !clock_time_exceed(p_switch->pressed_time, TIMEOUT_TICK_500MS)) {
                    p_switch->counter++;
                }
                p_switch->hold_time = p_switch->pressed_time = clock_time();
            }
        }
    } else {
        if (p_switch->status == SWITCH_FLOAT) {
            switch_first_start(i, SWITCH_OPEN);
        } else if (p_switch->debounce != 1) {
            p_switch->debounce--;
            if (p_switch->debounce == (DEBOUNCE_SWITCH - 5) && !zb_isDeviceJoinedNwk() && !zb_isDeviceFactoryNew()) {
                zb_rejoinReq(zb_apsChannelMaskGet(), g_bdbAttrs.scanDuration);
                APP_DEBUG(UART_PRINTF_MODE, "Device not joined. Rejoin\r\n");
            }
            if (p_switch->debounce == 1 && p_switch->pressed && p_switch->status != SWITCH_OPEN) {
                p_switch->status = SWITCH_OPEN;
                p_switch->released = true;
#if UART_PRINTF_MODE && DEBUG_SWITCH_EN
                APP_DEBUG(DEBUG_SWITCH_EN, "Switch %d is OPEN\r\n", dev_relay.unit_relay[i].ep);
#endif
                if(p_switch->counter == 1 && zb_isDeviceJoinedNwk()) {
                    APP_DEBUG(DEBUG_SWITCH_EN, "Key %d released toggle\r\n", i+1);
                    if (relay_settings.switchType[i] == ZCL_SWITCH_TYPE_MOMENTARY) {
                        if (relay_settings.switchActions[i] == ZCL_SWITCH_ACTION_OFF_ON) {
                            if (relay_settings.switch_decoupled[i] == CUSTOM_SWITCH_DECOUPLED_OFF) {
                                cmdOnOff_off(dev_relay.unit_relay[i].ep);
                            } else {
                                remoteCmdOnOff(dev_relay.unit_relay[i].ep, ZCL_CMD_ONOFF_OFF);
                            }
                        } else {
                            if (relay_settings.switch_decoupled[i] == CUSTOM_SWITCH_DECOUPLED_OFF) {
                                cmdOnOff_on(dev_relay.unit_relay[i].ep);
                            } else {
                                remoteCmdOnOff(dev_relay.unit_relay[i].ep, ZCL_CMD_ONOFF_ON);
                            }
                        }
                    }
                }
            }
        }
    }

    if (p_switch->released && clock_time_exceed(p_switch->pressed_time, TIMEOUT_TICK_500MS)) {
#if UART_PRINTF_MODE && DEBUG_SWITCH_EN
        APP_DEBUG(DEBUG_SWITCH_EN, "Toggle. Button %d pressed %d times\r\n", i+1, p_switch->counter);
#endif
        if (p_switch->counter >= FR_COUNTER_MAX) {
#if UART_PRINTF_MODE && DEBUG_SWITCH_EN
            APP_DEBUG(DEBUG_SWITCH_EN, "Reset Factory is ready from toggle\r\n");
#endif
            TL_SCHEDULE_TASK(switch_factory_reset_start, NULL);
        }
        p_switch->counter = 0;
        p_switch->pressed = false;
        p_switch->released = false;
        p_switch->hold = false;
    }
}

static void read_switch_multifunction(uint8_t i) {

    bool report = false;
    zcl_msInputAttr_t *msInputAttr = zcl_msInputAttrsGet();
    msInputAttr += i;

    app_switch_t *p_switch = &app_switch[i];

    if (!switch_gpio_read(dev_relay.unit_relay[i].sw)) {
        if (p_switch->pressed) {
            if (clock_time_exceed(p_switch->hold_time, TIMEOUT_TICK_500MS)) {
                if (p_switch->hold == false) {
                    p_switch->hold = true;
#if UART_PRINTF_MODE && DEBUG_SWITCH_EN
                    APP_DEBUG(DEBUG_SWITCH_EN, "Multifunction. Press and hold switch: %d\r\n", i+1);
#endif
                    msInputAttr->value = ACTION_HOLD;
                    APP_DEBUG(DEBUG_REPORTING_EN, "MSI report ep: %d value %d\r\n", i+1, msInputAttr->value);
                    app_forcedReport(i+1, ZCL_CLUSTER_GEN_MULTISTATE_INPUT_BASIC, ZCL_MULTISTATE_INPUT_ATTRID_PRESENT_VALUE);
                }
            }
        }
        if (p_switch->debounce != DEBOUNCE_SWITCH) {
            p_switch->debounce++;
            if (p_switch->debounce == 5 && !zb_isDeviceJoinedNwk() && !zb_isDeviceFactoryNew()) {
                zb_rejoinReq(zb_apsChannelMaskGet(), g_bdbAttrs.scanDuration);
                APP_DEBUG(UART_PRINTF_MODE, "Device not joined. Rejoin\r\n");
            }
            if (p_switch->debounce == DEBOUNCE_SWITCH) {
                p_switch->pressed = true;
                if (!clock_time_exceed(p_switch->pressed_time, TIMEOUT_TICK_500MS)) {
                    p_switch->counter++;
                } else {
                    p_switch->counter = 1;
                }
                p_switch->hold_time = p_switch->pressed_time = clock_time();
            }
        }
    } else {
        if (p_switch->debounce != 1) {
            p_switch->debounce--;
            if (p_switch->debounce == (DEBOUNCE_SWITCH - 5) && !zb_isDeviceJoinedNwk() && !zb_isDeviceFactoryNew()) {
                zb_rejoinReq(zb_apsChannelMaskGet(), g_bdbAttrs.scanDuration);
                APP_DEBUG(UART_PRINTF_MODE, "Device not joined. Rejoin\r\n");
            }
            if (p_switch->debounce == 1 && p_switch->pressed) {
                p_switch->released = true;
            }
        }
    }

    if (p_switch->released && clock_time_exceed(p_switch->pressed_time, TIMEOUT_TICK_500MS)) {
        if (p_switch->hold) {
            msInputAttr->value = ACTION_RELEASE;
            report = true;
#if UART_PRINTF_MODE && DEBUG_SWITCH_EN
            APP_DEBUG(DEBUG_SWITCH_EN, "Multifunction. Released switch: %d\r\n", i+1);
#endif
        } else {
            APP_DEBUG(DEBUG_SWITCH_EN, "Multifunction. Switch %d press %d times\r\n", i+1, p_switch->counter);
            switch(p_switch->counter) {
                case ACTION_SINGLE:                                         // 1
                case ACTION_DOUBLE:                                         // 2
                case ACTION_TRIPLE:                                         // 3
                    msInputAttr->value = p_switch->counter;
                    report = true;
#if UART_PRINTF_MODE && DEBUG_SWITCH_EN
                    APP_DEBUG(DEBUG_SWITCH_EN, "Switch %d click %d times\r\n", dev_relay.unit_relay[i].ep, msInputAttr->value);
#endif
                    break;
                case ACTION_CLEAR:                                          // 250
                    msInputAttr->value = ACTION_EMPTY;                      // 300
                    report = true;
                    break;
                default:
                    if (app_switch[i].counter >= FR_COUNTER_MAX) {
#if UART_PRINTF_MODE && DEBUG_SWITCH_EN
                        APP_DEBUG(DEBUG_SWITCH_EN, "Reset Factory is ready from multifunction\r\n");
#endif
                        TL_SCHEDULE_TASK(switch_factory_reset_start, NULL);
                    }
                    break;
            }

        }

        if (report) {
            APP_DEBUG(DEBUG_REPORTING_EN, "MSI report ep: %d value %d\r\n", i+1, msInputAttr->value);
            app_forcedReport(i+1, ZCL_CLUSTER_GEN_MULTISTATE_INPUT_BASIC, ZCL_MULTISTATE_INPUT_ATTRID_PRESENT_VALUE);
        }

        p_switch->counter = 0;
        p_switch->pressed = false;
        p_switch->released = false;
        p_switch->hold = false;
    }
}

static void read_switch_light_control(uint8_t i) {
    uint8_t level_dir;
    zcl_levelAttr_t *levelAttr = zcl_levelAttrsGet();
    levelAttr += i;

    app_switch_t *p_switch = &app_switch[i];

    if (!switch_gpio_read(dev_relay.unit_relay[i].sw)) {
        if (p_switch->pressed) {
            if (clock_time_exceed(p_switch->hold_time, TIMEOUT_TICK_500MS)) {
                if (p_switch->hold == false) {
                    p_switch->hold = true;
#if UART_PRINTF_MODE && DEBUG_SWITCH_EN
                    APP_DEBUG(DEBUG_SWITCH_EN, "Light control. Press and hold switch: %d\r\n", i+1);
#endif
                    if (!p_switch->level_dir) {
                        level_dir = LEVEL_MOVE_UP;
                        p_switch->level_dir = true;
                    } else {
                        level_dir = LEVEL_MOVE_DOWN;
                        p_switch->level_dir = false;
                    }

#if UART_PRINTF_MODE && DEBUG_SWITCH_EN
                    APP_DEBUG(DEBUG_BUTTON_EN, "Light control. Key: %d, level_dir: %d, button->level_dir: %d\r\n", i+1, level_dir, p_switch->level_dir);
#endif
                    app_move_to_level(i+1, level_dir);
                }
            }
        }
        if (p_switch->debounce != DEBOUNCE_SWITCH) {
            p_switch->debounce++;
            if (p_switch->debounce == 5 && !zb_isDeviceJoinedNwk() && !zb_isDeviceFactoryNew()) {
                zb_rejoinReq(zb_apsChannelMaskGet(), g_bdbAttrs.scanDuration);
                APP_DEBUG(UART_PRINTF_MODE, "Device not joined. Rejoin\r\n");
            }
            if (p_switch->debounce == DEBOUNCE_SWITCH) {
                p_switch->pressed = true;
                if (!clock_time_exceed(p_switch->pressed_time, TIMEOUT_TICK_500MS)) {
                    p_switch->counter++;
                } else {
                    p_switch->counter = 1;
                }
                p_switch->hold_time = p_switch->pressed_time = clock_time();
            }
        }
    } else {
        if (p_switch->debounce != 1) {
            p_switch->debounce--;
            if (p_switch->debounce == (DEBOUNCE_SWITCH - 5) && !zb_isDeviceJoinedNwk() && !zb_isDeviceFactoryNew()) {
                zb_rejoinReq(zb_apsChannelMaskGet(), g_bdbAttrs.scanDuration);
                APP_DEBUG(UART_PRINTF_MODE, "Device not joined. Rejoin\r\n");
            }
            if (p_switch->debounce == 1 && p_switch->pressed) {
                p_switch->released = true;
            }
        }
    }

    if (p_switch->released && clock_time_exceed(p_switch->pressed_time, TIMEOUT_TICK_500MS)) {
        if (p_switch->hold) {
#if UART_PRINTF_MODE && DEBUG_SWITCH_EN
            APP_DEBUG(DEBUG_SWITCH_EN, "Light control. Released button: %d\r\n", i+1);
#endif
            app_level_stop(i+1);
        } else {
#if UART_PRINTF_MODE && DEBUG_SWITCH_EN
            APP_DEBUG(DEBUG_SWITCH_EN, "Light control. Switch %d press %d times\r\n", i+1, p_switch->counter);
#endif
            switch(p_switch->counter) {
                case ACTION_SINGLE:                                         // 1
                    remoteCmdOnOff(i+1, ZCL_CMD_ONOFF_TOGGLE);
                    break;
                default:
                    if (app_switch[i].counter >= FR_COUNTER_MAX) {
#if UART_PRINTF_MODE && DEBUG_SWITCH_EN
                        APP_DEBUG(DEBUG_SWITCH_EN, "Reset Factory is ready from level\r\n");
#endif
                        TL_SCHEDULE_TASK(switch_factory_reset_start, NULL);
                    }
                    break;
            }
        }

        p_switch->counter = 0;
        p_switch->pressed = false;
        p_switch->released = false;
        p_switch->hold = false;
    }
}


void switch_handler() {

    for (uint8_t i = 0; i < dev_relay.amt; i++) {
        switch(relay_settings.switchType[i]) {
            case ZCL_SWITCH_TYPE_TOGGLE:
            case ZCL_SWITCH_TYPE_MOMENTARY:
                read_switch_toggle(i);
                break;
            case ZCL_SWITCH_TYPE_MULTIFUNCTION:
                read_switch_multifunction(i);
                break;
            case ZCL_CUSTOM_SWITCH_TYPE_LIGHT_CONTROL:
                read_switch_light_control(i);
                break;
            default:
                break;
        }
    }
}

bool switch_idle() {
    for (uint8_t i = 0; i < dev_relay.amt; i++) {
        if (app_switch[i].debounce != 1 && app_switch[i].debounce != DEBOUNCE_SWITCH && app_switch[i].counter) return true;
    }

    return false;
}

void switch_init() {

    app_switch_t *p_switch = NULL;

    for (uint8_t i = 0; i < dev_relay.amt; i++) {
        p_switch = &app_switch[i];
        p_switch->debounce = (DEBOUNCE_SWITCH / 2),
        p_switch->hold = false;
        p_switch->counter = 0;
        p_switch->pressed = false;
        p_switch->released = false;
        p_switch->level_dir = false;
        p_switch->color_dir = false;
        if (relay_settings.switchType[i] < ZCL_SWITCH_TYPE_MULTIFUNCTION)
            p_switch->status = SWITCH_FLOAT;
        else
            p_switch->status = SWITCH_OPEN;
        p_switch->pressed_time = clock_time();
    }
}

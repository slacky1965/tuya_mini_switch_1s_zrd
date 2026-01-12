#include "app_main.h"

relay_settings_t relay_settings;
dev_relay_t      dev_relay;

static uint8_t checksum(uint8_t *data, uint16_t length) {

    uint8_t crc8 = 0;

    for(uint8_t i = 0; i < length; i++) {
        crc8 += data[i];
    }

    return crc8;
}

static void check_first_start(uint8_t i) {

    switch(relay_settings.startUpOnOff[i]) {
        case ZCL_START_UP_ONOFF_SET_ONOFF_TO_PREVIOUS:
            if (relay_settings.status_onoff[i]) cmdOnOff_on(dev_relay.unit_relay[i].ep);
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
}



//bool get_relay_status() {
//    return drv_gpio_read(RELAY1_GPIO)?true:false;
//}

void set_relay_status(uint8_t i, uint8_t status) {
//    printf("set_relay_status(i = %d, status = %d). GPIO: %d\r\n", i, status, dev_relay.unit_relay[i].rl);
    drv_gpio_write(dev_relay.unit_relay[i].rl, status);
}

static void print_setting_sr(nv_sts_t st, relay_settings_t *relay_settings_tmp, bool save) {

#if UART_PRINTF_MODE && DEBUG_SAVE

    printf("Settings %s. Return: %s\r\n", save?"saved":"restored", st==NV_SUCC?"Ok":"Error");

    for (uint8_t i = 0; i < AMT_RELAY; i++) {
        printf("status_onoff%d:      0x%02x\r\n", i, relay_settings_tmp->status_onoff[i]);
        printf("startUpOnOff%d:      0x%02x\r\n", i, relay_settings_tmp->startUpOnOff[i]);
        printf("switchActions%d:     0x%02x\r\n", i, relay_settings_tmp->switchActions[i]);
        printf("switchType%d:        0x%02x\r\n", i, relay_settings_tmp->switchType[i]);
        printf("switch_decoupled%d:  0x%02x\r\n", i, relay_settings_tmp->switch_decoupled[i]);
    }

#endif
}

nv_sts_t relay_settings_save() {
    nv_sts_t st = NV_SUCC;

#if NV_ENABLE

#if UART_PRINTF_MODE
    printf("Saved relay settings\r\n");
#endif

    relay_settings.crc = checksum((uint8_t*)&relay_settings, sizeof(relay_settings_t)-1);
    st = nv_flashWriteNew(1, NV_MODULE_APP,  NV_ITEM_APP_USER_CFG, sizeof(relay_settings_t), (uint8_t*)&relay_settings);

#else
    st = NV_ENABLE_PROTECT_ERROR;
#endif

    return st;
}

nv_sts_t relay_settings_restore() {
    nv_sts_t st = NV_SUCC;

#if NV_ENABLE

    relay_settings_t relay_settings_tmp;

    st = nv_flashReadNew(1, NV_MODULE_APP,  NV_ITEM_APP_USER_CFG, sizeof(relay_settings_t), (uint8_t*)&relay_settings_tmp);

    if (st == NV_SUCC && relay_settings_tmp.crc == checksum((uint8_t*)&relay_settings_tmp, sizeof(relay_settings_t)-1)) {

#if UART_PRINTF_MODE
        printf("Restored relay settings\r\n");
#if DEBUG_SAVE
        print_setting_sr(st, &relay_settings_tmp, false);
#endif
#endif

    } else {
        /* default config */
#if UART_PRINTF_MODE
        printf("Default relay settings \r\n");
#endif

        for (uint8_t i = 0; i < AMT_RELAY; i++) {
            relay_settings_tmp.startUpOnOff[i] = ZCL_START_UP_ONOFF_SET_ONOFF_TO_OFF;
            relay_settings_tmp.status_onoff[i] = ZCL_ONOFF_STATUS_OFF;
            relay_settings_tmp.switchActions[i] = ZCL_SWITCH_ACTION_OFF_ON;
            relay_settings_tmp.switchType[i] = ZCL_SWITCH_TYPE_MOMENTARY;
            relay_settings_tmp.switch_decoupled[i] = CUSTOM_SWITCH_DECOUPLED_OFF;
        }
    }

    memcpy(&relay_settings, &relay_settings_tmp, (sizeof(relay_settings_t)));
    for (uint8_t i = 0; i < AMT_RELAY; i++) {
        g_zcl_onOffAttrs[i].onOff = relay_settings.status_onoff[i];
        g_zcl_onOffAttrs[i].startUpOnOff = relay_settings.startUpOnOff[i];
        g_zcl_onOffCfgAttrs[i].custom_swtichType = g_zcl_onOffCfgAttrs[i].switchType = relay_settings.switchType[i];
        g_zcl_onOffCfgAttrs[i].switchActions = relay_settings.switchActions[i];
        g_zcl_onOffCfgAttrs[i].custom_decoupled = relay_settings.switch_decoupled[i];
    }

#else
    st = NV_ENABLE_PROTECT_ERROR;
#endif

    return st;
}

void dev_relay_init() {
    dev_relay.amt = AMT_RELAY;
    dev_relay.unit_relay[0].ep = APP_ENDPOINT1;
    dev_relay.unit_relay[0].sw = cur_device->switch_gpio.gpio; //SWITCH1_GPIO;
    dev_relay.unit_relay[0].rl = cur_device->relay_gpio.gpio; //RELAY1_GPIO;

    if (relay_settings.switchType[0] == ZCL_SWITCH_TYPE_MULTIFUNCTION) {
        check_first_start(0);
    }

}

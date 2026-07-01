#ifndef SRC_INCLUDE_APP_ONOFF_H_
#define SRC_INCLUDE_APP_ONOFF_H_

typedef struct __attribute__((packed)) {
    uint8_t  id;                            /* ID_SETTING               */
    uint8_t  status_onoff[AMT_RELAY];
    uint8_t  crc;
} status_onoff_t;

extern status_onoff_t   status_onoff;

void cmdOnOff_toggle(uint8_t ep);
void cmdOnOff_on(uint8_t ep);
void cmdOnOff_off(uint8_t ep);
void remoteCmdOnOff(uint8_t ep, uint8_t cmd);
void init_default_status_onoff(status_onoff_t *status);
void status_onoff_restore();
void status_onoff_save();

#if DEBUG_TEST_STATUS_ONOFF_SAVE_EN
int32_t test_onoff_save(void *args);
#endif


#endif /* SRC_INCLUDE_APP_ONOFF_H_ */

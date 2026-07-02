#include "app_main.h"

#define ID_SETTING              0x65
#define FLASH_SAVE_SIZE         4

status_onoff_t   status_onoff;
static uint32_t  flash_addr_start = BEGIN_STATUS_ONOFF_DATA;


void cmdOnOff_toggle(uint8_t ep) {

    uint8_t i = ep-1;

    zcl_onOffAttr_t *pOnOff = zcl_onOffAttrsGet();
    pOnOff += i;

#if UART_PRINTF_MODE && DEBUG_ONOFF_EN
    APP_DEBUG(DEBUG_ONOFF_EN, "cmdOnOff_toggle. ep: %d, onoff: %d, status_onoff: %d\r\n", ep, pOnOff->onOff, status_onoff.status_onoff[i]);
#endif

    status_onoff.status_onoff[i] = !status_onoff.status_onoff[i];
    pOnOff->onOff = status_onoff.status_onoff[i];
    if (pOnOff->startUpOnOff > ZCL_START_UP_ONOFF_SET_ONOFF_TO_ON) {
        status_onoff_save();
    }

    uint8_t status = RELAY_OFF(device->relay_off);

    if (pOnOff->onOff) {
        status = RELAY_ON(device->relay_on);
    }

    set_relay_status(i, status);
}

void cmdOnOff_on(uint8_t ep) {

    uint8_t i = ep-1;

    zcl_onOffAttr_t *pOnOff = zcl_onOffAttrsGet();
    pOnOff += i;

#if UART_PRINTF_MODE && DEBUG_ONOFF_EN
    APP_DEBUG(DEBUG_ONOFF_EN, "cmdOnOff_on. ep: %d, onoff: %d, status_onoff: %d\r\n", ep, pOnOff->onOff, status_onoff.status_onoff[i]);
#endif

    if (pOnOff->onOff != ZCL_ONOFF_STATUS_ON || status_onoff.status_onoff[i] != ZCL_ONOFF_STATUS_ON) {
        pOnOff->onOff = status_onoff.status_onoff[i] = ZCL_ONOFF_STATUS_ON;
        if (pOnOff->startUpOnOff > ZCL_START_UP_ONOFF_SET_ONOFF_TO_ON) {
            status_onoff_save();
        }
    }

    uint8_t status = RELAY_ON(device->relay_on);

    set_relay_status(i, status);
}

void cmdOnOff_off(uint8_t ep) {

    uint8_t i = ep-1;

    zcl_onOffAttr_t *pOnOff = zcl_onOffAttrsGet();
    pOnOff += i;

#if UART_PRINTF_MODE && DEBUG_ONOFF_EN
    APP_DEBUG(DEBUG_ONOFF_EN, "cmdOnOff_off. ep: %d, onoff: %d, status_onoff: %d\r\n", ep, pOnOff->onOff, status_onoff.status_onoff[i]);
#endif

    if (pOnOff->onOff != ZCL_ONOFF_STATUS_OFF || status_onoff.status_onoff[i] != ZCL_ONOFF_STATUS_OFF) {
        pOnOff->onOff = status_onoff.status_onoff[i] = ZCL_ONOFF_STATUS_OFF;
        if (pOnOff->startUpOnOff > ZCL_START_UP_ONOFF_SET_ONOFF_TO_ON) {
            status_onoff_save();
        }
    }

    uint8_t status = RELAY_OFF(device->relay_off);

    set_relay_status(i, status);
}

static status_t cmdOnOffSend(uint8_t ep, epInfo_t *dstEpInfo, uint8_t command) {

    status_t st = 0xFF;

    /* command 0x00 - off, 0x01 - on, 0x02 - toggle */
    switch(command) {
        case ZCL_CMD_ONOFF_OFF:
            APP_DEBUG(DEBUG_ONOFF_EN, "OnOff command: off\r\n");
            st = zcl_onOff_offCmd(ep, dstEpInfo, FALSE);
            break;
        case ZCL_CMD_ONOFF_ON:
            APP_DEBUG(DEBUG_ONOFF_EN, "OnOff command: on\r\n");
            st = zcl_onOff_onCmd(ep, dstEpInfo, FALSE);
            break;
        case ZCL_CMD_ONOFF_TOGGLE:
            APP_DEBUG(DEBUG_ONOFF_EN, "OnOff command: toggle\r\n");
            st = zcl_onOff_toggleCmd(ep, dstEpInfo, FALSE);
            break;
        default:
            APP_DEBUG(DEBUG_ONOFF_EN, "OnOff command: unknown\r\n");
            break;
    }
//    APP_DEBUG(DEBUG_ONOFF_EN, ", status: 0x%02x\r\n", st);
    return st;
}


void remoteCmdOnOff(uint8_t ep, uint8_t command) {
    status_t st;
    epInfo_t dstEpInfo;
    TL_SETSTRUCTCONTENT(dstEpInfo, 0);
    dstEpInfo.profileId = HA_PROFILE_ID;

    uint16_t groupList[APS_GROUP_TABLE_NUM];
    uint8_t groupCnt = 0;
    aps_group_list_get(&groupCnt, groupList);

    /* command for groups */
    dstEpInfo.dstAddrMode = APS_SHORT_GROUPADDR_NOEP;
    for (uint8_t i = 0; i < groupCnt; i++) {
        aps_group_tbl_ent_t *grEntry = aps_group_search(groupList[i], ep);
        if (grEntry) {
            dstEpInfo.dstAddr.shortAddr = grEntry->group_addr;
            st = cmdOnOffSend(ep, &dstEpInfo, command);
            APP_DEBUG(DEBUG_ONOFF_EN, "OnOff in groups. cmd: %s, src_ep: %d, dst_ep: %d, addr: 0x%04x, status: %d\r\n",
                    (command == 0)?"Off":(command == 1)?"On":"Toggle", ep, grEntry->n_endpoints, grEntry->group_addr, st);
        }
    }

    /* command when binding */
    TL_SETSTRUCTCONTENT(dstEpInfo, 0);
    dstEpInfo.profileId = HA_PROFILE_ID;
//    dstEpInfo.dstAddrMode = APS_DSTADDR_EP_NOTPRESETNT;
//    dstEpInfo.dstAddrMode = APS_SHORT_GROUPADDR_NOEP;
//    dstEpInfo.dstAddrMode = APS_LONG_DSTADDR_WITHEP;

    aps_binding_entry_t *bind_tbl = bindTblEntryGet();
    for (uint8_t j = 0; j < APS_BINDING_TABLE_NUM; j++) {
        if (bind_tbl->used && bind_tbl->clusterId == ZCL_CLUSTER_GEN_ON_OFF && bind_tbl->srcEp == ep) {
            dstEpInfo.dstAddrMode = bind_tbl->dstAddrMode;
            if (dstEpInfo.dstAddrMode == APS_SHORT_GROUPADDR_NOEP) {
                dstEpInfo.dstAddr.shortAddr = bind_tbl->groupAddr;
            } else {
                dstEpInfo.dstAddrMode = APS_LONG_DSTADDR_WITHEP;
                dstEpInfo.dstEp = bind_tbl->dstExtAddrInfo.dstEp;
                memcpy(dstEpInfo.dstAddr.extAddr, bind_tbl->dstExtAddrInfo.extAddr, sizeof(extAddr_t));
            }
            st = cmdOnOffSend(ep, &dstEpInfo, command);
#if DEBUG_ONOFF_EN
            APP_DEBUG(DEBUG_ONOFF_EN, "OnOff for bind. cmd: %s, ep: %d, clId: 0x%04x, addrMode: %d - %s, ",
                    (command == 0)?"Off":(command == 1)?"On":"Toggle",
                     bind_tbl->srcEp, bind_tbl->clusterId, dstEpInfo.dstAddrMode,
                    (dstEpInfo.dstAddrMode == APS_DSTADDR_EP_NOTPRESETNT)?"APS_DSTADDR_EP_NOTPRESETNT":
                    (dstEpInfo.dstAddrMode == APS_SHORT_GROUPADDR_NOEP)?"APS_SHORT_GROUPADDR_NOEP":
                    (dstEpInfo.dstAddrMode == APS_SHORT_DSTADDR_WITHEP)?"APS_SHORT_DSTADDR_WITHEP":"APS_LONG_DSTADDR_WITHEP");
            if (dstEpInfo.dstAddrMode == APS_LONG_DSTADDR_WITHEP) {
                APP_DEBUG(DEBUG_ONOFF_EN, "ieee: 0x%02x%02x%02x%02x%02x%02x%02x%02x, ",
                        bind_tbl->dstExtAddrInfo.extAddr[0], bind_tbl->dstExtAddrInfo.extAddr[1],
                        bind_tbl->dstExtAddrInfo.extAddr[2], bind_tbl->dstExtAddrInfo.extAddr[3],
                        bind_tbl->dstExtAddrInfo.extAddr[4], bind_tbl->dstExtAddrInfo.extAddr[5],
                        bind_tbl->dstExtAddrInfo.extAddr[6], bind_tbl->dstExtAddrInfo.extAddr[7]);
            } else if (dstEpInfo.dstAddrMode == APS_SHORT_GROUPADDR_NOEP) {
                APP_DEBUG(DEBUG_ONOFF_EN, "groupAddr: 0x%04x, ",
                        dstEpInfo.dstAddr.shortAddr);
            } else {
                APP_DEBUG(DEBUG_ONOFF_EN, "shortAddr: 0x%04x, ",
                        dstEpInfo.dstAddr.shortAddr);
            }
            APP_DEBUG(DEBUG_ONOFF_EN, "status: 0x%02x\r\n", st);
#endif
        }
        bind_tbl++;
    }
}

void init_default_status_onoff(status_onoff_t *status) {

    uint32_t status_onoff_size = sizeof(status_onoff_t);

    memset(status, 0, status_onoff_size);

    for(uint8_t i = 0; i < AMT_RELAY; i++) {
        status->status_onoff[i] = ZCL_ONOFF_STATUS_OFF;
    }

    status->id = ID_SETTING;
    flash_addr_start = BEGIN_STATUS_ONOFF_DATA;
    status->crc = checksum((uint8_t*)status, status_onoff_size);
    flash_erase(flash_addr_start);
    flash_write(flash_addr_start, status_onoff_size, (uint8_t*)status);
    APP_DEBUG(DEBUG_SAVE_EN, "Save default status onoff to flash address - 0x%x\r\n", flash_addr_start);
//    APP_DEBUG(DEBUG_SAVE_EN, "id: 0x%02x, crc1: 0x%02x, crc2: 0x%02x\r\n", status->id, checksum((uint8_t*)status, status_onoff_size), status->crc);
}

void status_onoff_restore() {

    status_onoff_t onoff_curr, onoff_next;

    uint32_t flash_addr = BEGIN_STATUS_ONOFF_DATA;
    uint32_t status_onoff_size = sizeof(status_onoff_t);

    while (flash_addr < END_STATUS_ONOFF_DATA) {
        flash_read_page(flash_addr, status_onoff_size, (uint8_t*)&onoff_curr);
        if (onoff_curr.id == ID_SETTING && checksum((uint8_t*)&onoff_curr, status_onoff_size) == onoff_curr.crc) {
            break;
        }
        flash_addr += FLASH_SAVE_SIZE;
    }

    if (flash_addr >= END_STATUS_ONOFF_DATA) {
        APP_DEBUG(DEBUG_SAVE_EN, "No saved status onoff! Init.\r\n");
        init_default_status_onoff(&onoff_curr);
    } else {
        flash_addr += FLASH_SAVE_SIZE;
        while(flash_addr < END_STATUS_ONOFF_DATA) {
            flash_read_page(flash_addr, status_onoff_size, (uint8_t*)&onoff_next);
            if (onoff_next.id == ID_SETTING && checksum((uint8_t*)&onoff_next, status_onoff_size) == onoff_next.crc) {
                memcpy(&onoff_curr, &onoff_next, status_onoff_size);
                flash_addr += FLASH_SAVE_SIZE;
                continue;
            }
            break;
        }
        flash_addr_start = flash_addr - FLASH_SAVE_SIZE;
    }

    APP_DEBUG(DEBUG_SAVE_EN, "Read status onoff from flash address - 0x%x\r\n", flash_addr_start);

    memcpy(&status_onoff, &onoff_curr, status_onoff_size);

#if UART_PRINTF_MODE
    for (uint8_t i = 0; i < AMT_RELAY; i++) {
        APP_DEBUG(DEBUG_SAVE_EN, "status_onoff_%d: 0x%02x\r\n", i+1, status_onoff.status_onoff[i]);
    }
#endif
}

void status_onoff_save() {

    uint32_t status_onoff_size = sizeof(status_onoff_t);

    flash_addr_start += FLASH_SAVE_SIZE;
    if (flash_addr_start == END_STATUS_ONOFF_DATA) {
        flash_addr_start = BEGIN_STATUS_ONOFF_DATA;
    }
    if (flash_addr_start % FLASH_SECTOR_SIZE == 0) {
        flash_erase(flash_addr_start);
    }
    status_onoff.crc = checksum((uint8_t*)&status_onoff, status_onoff_size);
    flash_write(flash_addr_start, status_onoff_size, (uint8_t*)&(status_onoff));
#if UART_PRINTF_MODE
    APP_DEBUG(DEBUG_SAVE_EN, "Save status onoff to flash address - 0x%x\r\n", flash_addr_start);
    for (uint8_t i = 0; i < AMT_RELAY; i++) {
        APP_DEBUG(DEBUG_SAVE_EN, "status_onoff: 0x%02x\r\n", status_onoff.status_onoff[i]);
    }
#endif
}

#if DEBUG_TEST_STATUS_ONOFF_SAVE_EN
int32_t test_onoff_save(void *args) {

    status_onoff_save();

    if (flash_addr_start == (END_STATUS_ONOFF_DATA - FLASH_SAVE_SIZE)) {
        return -1;
    }


    return 0;

}
#endif

#include "app_main.h"

uint32_t last_timeReportMsi[AMT_RELAY];     // time of the last attribute report ZCL_MULTISTATE_INPUT_ATTRID_PRESENT_VALUE
uint8_t  last_seqNum[AMT_RELAY];

//static int32_t resetMsiTimerCb(void *args) {
//
//    uint8_t i = (uint8_t)((uint32_t)args) - 1;
//
////    printf("resetMsiTimerCb. i: %d\r\n", i);
//
//    zcl_msInputAttr_t *msInputAttr = zcl_msInputAttrsGet();
//    msInputAttr += i;
//    msInputAttr->value = ACTION_EMPTY;
//
////    app_forcedReport(dev_relay.unit_relay[i].ep, ZCL_CLUSTER_GEN_MULTISTATE_INPUT_BASIC, ZCL_MULTISTATE_INPUT_ATTRID_PRESENT_VALUE);
//
//    return -1;
//}

void app_all_forceReporting(void *args) {

    if (zb_isDeviceJoinedNwk()) {
//        app_forcedReport(APP_ENDPOINT1, ZCL_CLUSTER_SE_METERING, ZCL_ATTRID_CUSTOM_DEVICE_MODEL);
//        app_forcedReport(APP_ENDPOINT1, ZCL_CLUSTER_GEN_DEVICE_TEMP_CONFIG, ZCL_ATTRID_DEV_TEMP_CURR_TEMP);
//        app_forcedReport(APP_ENDPOINT1, ZCL_CLUSTER_SE_METERING, ZCL_ATTRID_METER_SERIAL_NUMBER);
//        app_forcedReport(APP_ENDPOINT1, ZCL_CLUSTER_SE_METERING, ZCL_ATTRID_CUSTOM_DATE_RELEASE);
//        app_forcedReport(APP_ENDPOINT1, ZCL_CLUSTER_SE_METERING, ZCL_ATTRID_MULTIPLIER);
//        app_forcedReport(APP_ENDPOINT1, ZCL_CLUSTER_SE_METERING, ZCL_ATTRID_DIVISOR);
//        app_forcedReport(APP_ENDPOINT1, ZCL_CLUSTER_SE_METERING, ZCL_ATTRID_CURRENT_SUMMATION_DELIVERD);
//        app_forcedReport(APP_ENDPOINT1, ZCL_CLUSTER_SE_METERING, ZCL_ATTRID_CURRENT_TIER_1_SUMMATION_DELIVERD);
//        app_forcedReport(APP_ENDPOINT1, ZCL_CLUSTER_SE_METERING, ZCL_ATTRID_CURRENT_TIER_2_SUMMATION_DELIVERD);
//        app_forcedReport(APP_ENDPOINT1, ZCL_CLUSTER_SE_METERING, ZCL_ATTRID_CURRENT_TIER_3_SUMMATION_DELIVERD);
//        app_forcedReport(APP_ENDPOINT1, ZCL_CLUSTER_SE_METERING, ZCL_ATTRID_CURRENT_TIER_4_SUMMATION_DELIVERD);
//        app_forcedReport(APP_ENDPOINT1, ZCL_CLUSTER_MS_ELECTRICAL_MEASUREMENT, ZCL_ATTRID_AC_VOLTAGE_MULTIPLIER);
//        app_forcedReport(APP_ENDPOINT1, ZCL_CLUSTER_MS_ELECTRICAL_MEASUREMENT, ZCL_ATTRID_AC_VOLTAGE_DIVISOR);
//        app_forcedReport(APP_ENDPOINT1, ZCL_CLUSTER_MS_ELECTRICAL_MEASUREMENT, ZCL_ATTRID_RMS_VOLTAGE);
//        app_forcedReport(APP_ENDPOINT1, ZCL_CLUSTER_MS_ELECTRICAL_MEASUREMENT, ZCL_ATTRID_AC_POWER_MULTIPLIER);
//        app_forcedReport(APP_ENDPOINT1, ZCL_CLUSTER_MS_ELECTRICAL_MEASUREMENT, ZCL_ATTRID_AC_POWER_DIVISOR);
//        app_forcedReport(APP_ENDPOINT1, ZCL_CLUSTER_MS_ELECTRICAL_MEASUREMENT, ZCL_ATTRID_ACTIVE_POWER);
//        app_forcedReport(APP_ENDPOINT1, ZCL_CLUSTER_MS_ELECTRICAL_MEASUREMENT, ZCL_ATTRID_AC_CURRENT_MULTIPLIER);
//        app_forcedReport(APP_ENDPOINT1, ZCL_CLUSTER_MS_ELECTRICAL_MEASUREMENT, ZCL_ATTRID_AC_CURRENT_DIVISOR);
//        app_forcedReport(APP_ENDPOINT1, ZCL_CLUSTER_MS_ELECTRICAL_MEASUREMENT, ZCL_ATTRID_RMS_CURRENT);
//        app_forcedReport(APP_ENDPOINT1, ZCL_CLUSTER_SE_METERING, ZCL_ATTRID_REMAINING_BATTERY_LIFE);
    }

}

void app_forcedReport(uint8_t endpoint, uint16_t claster_id, uint16_t attr_id) {

    if (zb_isDeviceJoinedNwk()) {

        epInfo_t dstEpInfo;
        TL_SETSTRUCTCONTENT(dstEpInfo, 0);

        status_t st = 0;
//        uint8_t i = endpoint - 1;
        dstEpInfo.profileId = HA_PROFILE_ID;
        dstEpInfo.dstAddrMode = APS_DSTADDR_EP_NOTPRESETNT;

        zclAttrInfo_t *pAttrEntry = zcl_findAttribute(endpoint, claster_id, attr_id);

        if (pAttrEntry) {

            aps_binding_entry_t *bind_tbl = bindTblEntryGet();
            for (uint8_t j = 0; j < APS_BINDING_TABLE_NUM; j++) {
                if (bind_tbl->used && bind_tbl->clusterId == claster_id && bind_tbl->srcEp == endpoint) {
                    dstEpInfo.dstAddrMode = bind_tbl->dstAddrMode;
                    if (dstEpInfo.dstAddrMode == APS_SHORT_GROUPADDR_NOEP) {
                        dstEpInfo.dstAddr.shortAddr = bind_tbl->groupAddr;
                    } else {
                        dstEpInfo.dstAddrMode = APS_LONG_DSTADDR_WITHEP;
                        dstEpInfo.dstEp = bind_tbl->dstExtAddrInfo.dstEp;
                        memcpy(dstEpInfo.dstAddr.extAddr, bind_tbl->dstExtAddrInfo.extAddr, sizeof(extAddr_t));
                    }
                    st = zcl_sendReportCmd(endpoint, &dstEpInfo,  TRUE, ZCL_FRAME_SERVER_CLIENT_DIR,
                                claster_id, pAttrEntry->id, pAttrEntry->type, pAttrEntry->data);
#if DEBUG_REPORTING_EN
                    APP_DEBUG(DEBUG_REPORTING_EN, "forceReportCb. Ep: %d, clId: 0x%04x, attr_id: 0x%04x, data: 0x%08x, addrMode: %d - %s, ",
                            bind_tbl->srcEp, bind_tbl->clusterId, attr_id, *pAttrEntry->data, dstEpInfo.dstAddrMode,
                            (dstEpInfo.dstAddrMode == APS_DSTADDR_EP_NOTPRESETNT)?"APS_DSTADDR_EP_NOTPRESETNT":
                            (dstEpInfo.dstAddrMode == APS_SHORT_GROUPADDR_NOEP)?"APS_SHORT_GROUPADDR_NOEP":
                            (dstEpInfo.dstAddrMode == APS_SHORT_DSTADDR_WITHEP)?"APS_SHORT_DSTADDR_WITHEP":"APS_LONG_DSTADDR_WITHEP");
                    if (dstEpInfo.dstAddrMode == APS_LONG_DSTADDR_WITHEP) {
                        APP_DEBUG(DEBUG_REPORTING_EN, "ieee: 0x%02x%02x%02x%02x%02x%02x%02x%02x, ",
                                bind_tbl->dstExtAddrInfo.extAddr[0], bind_tbl->dstExtAddrInfo.extAddr[1],
                                bind_tbl->dstExtAddrInfo.extAddr[2], bind_tbl->dstExtAddrInfo.extAddr[3],
                                bind_tbl->dstExtAddrInfo.extAddr[4], bind_tbl->dstExtAddrInfo.extAddr[5],
                                bind_tbl->dstExtAddrInfo.extAddr[6], bind_tbl->dstExtAddrInfo.extAddr[7]);
                    } else if (dstEpInfo.dstAddrMode == APS_SHORT_GROUPADDR_NOEP) {
                        APP_DEBUG(DEBUG_REPORTING_EN, "groupAddr: 0x%04x, ", dstEpInfo.dstAddr.shortAddr);
                    } else {
                        APP_DEBUG(DEBUG_REPORTING_EN, "shortAddr: 0x%04x, ", dstEpInfo.dstAddr.shortAddr);
                    }
                    APP_DEBUG(DEBUG_REPORTING_EN, "status: 0x%02x\r\n", st);
#endif
                }
                bind_tbl++;
            }
        }
//        if (!pAttrEntry) {
//            //should not happen.
//            ZB_EXCEPTION_POST(SYS_EXCEPTTION_ZB_ZCL_ENTRY);
//            return;
//        }
//
//        ret = zcl_sendReportCmd(endpoint, &dstEpInfo,  TRUE, ZCL_FRAME_SERVER_CLIENT_DIR,
//                    claster_id, pAttrEntry->id, pAttrEntry->type, pAttrEntry->data);
//
////        if (attr_id == ZCL_MULTISTATE_INPUT_ATTRID_PRESENT_VALUE) {
////            last_timeReportMsi[i] = clock_time();
////            last_seqNum[i] = ZCL_SEQ_NUM;
//////            printf("MSI report. seqNum: %d\r\n", last_seqNum[i]);
////            ret = zcl_report(endpoint, &dstEpInfo, TRUE, ZCL_FRAME_SERVER_CLIENT_DIR, last_seqNum[i],
////                    MANUFACTURER_CODE_NONE, claster_id, pAttrEntry->id, pAttrEntry->type, pAttrEntry->data);
////            TL_ZB_TIMER_SCHEDULE(resetMsiTimerCb, (void*)((uint32_t)endpoint), TIMEOUT_750MS);
////        } else {
////            ret = zcl_sendReportCmd(endpoint, &dstEpInfo,  TRUE, ZCL_FRAME_SERVER_CLIENT_DIR,
////                    claster_id, pAttrEntry->id, pAttrEntry->type, pAttrEntry->data);
////        }
//
//
//        APP_DEBUG(DEBUG_REPORTING_EN, "ret: %d, forceReportCb. endpoint: 0x%x, claster_id: 0x%x, attr_id: 0x%x\r\n", ret, endpoint, claster_id, attr_id);
    }


}

int32_t forcedReportCb(void *arg) {

    TL_SCHEDULE_TASK(app_all_forceReporting, NULL);

    return -1;
}

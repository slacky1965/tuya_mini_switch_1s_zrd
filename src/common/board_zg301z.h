#ifndef SRC_COMMON_BOARD_ZG301Z_H_
#define SRC_COMMON_BOARD_ZG301Z_H_

#if (BOARD == BOARD_ZG301Z)

#define ZCL_BASIC_MODEL_ID     {13,'T','S','M','1','-','0','0','2','5','-','S','l','D'}


/************************* Configure SWITCH GPIO ***************************************/
#define SWITCH1_GPIO            GPIO_PB6
//#define PB6_INPUT_ENABLE        ON
//#define PB6_OUTPUT_ENABLE       OFF
//#define PB6_FUNC                AS_GPIO
//#define PULL_WAKEUP_SRC_PB6     PM_PIN_PULLUP_10K //PM_PIN_PULLDOWN_100K //


/************************* Configure KEY GPIO ***************************************/
#define MAX_BUTTON_NUM  1

#define BUTTON                  GPIO_PB1
#define PB1_INPUT_ENABLE        ON
#define PB1_DATA_OUT            OFF
#define PB1_OUTPUT_ENABLE       OFF
#define PB1_FUNC                AS_GPIO
#define PULL_WAKEUP_SRC_PB1     PM_PIN_PULLUP_10K

enum {
    VK_SW1 = 0x01,
};

#define KB_MAP_NORMAL   {\
        {VK_SW1,}}

#define KB_MAP_NUM      KB_MAP_NORMAL
#define KB_MAP_FN       KB_MAP_NORMAL

#define KB_DRIVE_PINS  {NULL }
#define KB_SCAN_PINS   {BUTTON}

/************************** Configure LED ****************************************/

#define LED_ON                  0
#define LED_OFF                 1

#define LED_GPIO                GPIO_PD4
#define PD4_FUNC                AS_GPIO
#define PD4_OUTPUT_ENABLE       ON
#define PD4_INPUT_ENABLE        OFF

#if (__PROJECT_TL_BOOT_LOADER__)

#define LED_POWER               GPIO_PA0
#define PA0_FUNC                AS_GPIO
#define PA0_OUTPUT_ENABLE       ON
#define PA0_INPUT_ENABLE        OFF

#define LED_PERMIT              LED_GPIO

#endif

/********************* Configure Relay ***************************/

#define RELAY_ON                1
#define RELAY_OFF               0

#define RELAY1_GPIO             GPIO_PA1
#define PA1_FUNC                AS_GPIO
#define PA1_OUTPUT_ENABLE       ON
#define PA1_INPUT_ENABLE        ON
#define PA1_DATA_OUT            RELAY_OFF

/********************* Configure printf UART ***************************/

#if UART_PRINTF_MODE
#define DEBUG_INFO_TX_PIN       UART_TX_PC2    //printf
#define DEBUG_BAUDRATE          115200

#endif /* UART_PRINTF_MODE */

#endif /* (BOARD == BOARD_ZG301Z) */

#endif /* SRC_COMMON_BOARD_ZG301Z_H_ */

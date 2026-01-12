#ifndef SRC_COMMON_BOARD_JR_ZDS01_H_
#define SRC_COMMON_BOARD_JR_ZDS01_H_

#if (BOARD == BOARD_JR_ZDS01)

#define ZCL_BASIC_MODEL_ID     {13,'T','S','M','1','-','0','0','2','6','-','S','l','D'}

/************************* Configure SWITCH GPIO ***************************************/
#define SWITCH1_GPIO            GPIO_PB4
#define PB4_INPUT_ENABLE        ON
#define PB4_OUTPUT_ENABLE       OFF
#define PB4_FUNC                AS_GPIO
#define PULL_WAKEUP_SRC_PB4     PM_PIN_PULLDOWN_100K //PM_PIN_PULLUP_10K //

#define DEBOUNCE_SWITCH         64  /* number of polls for debounce                 */



/************************* Configure KEY GPIO ***************************************/
#define MAX_BUTTON_NUM  1

#define BUTTON                  GPIO_PC2
#define PC2_INPUT_ENABLE        ON
#define PC2_DATA_OUT            OFF
#define PC2_OUTPUT_ENABLE       OFF
#define PC2_FUNC                AS_GPIO
#define PULL_WAKEUP_SRC_PC2     PM_PIN_PULLUP_10K

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

#define LED_GPIO                GPIO_PB5
#define PB5_FUNC                AS_GPIO
#define PB5_OUTPUT_ENABLE       ON
#define PB5_INPUT_ENABLE        OFF

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

#define RELAY1_GPIO             GPIO_PD2
#define PD2_FUNC                AS_GPIO
#define PD2_OUTPUT_ENABLE       ON
#define PD2_INPUT_ENABLE        ON
#define PD2_DATA_OUT            RELAY_OFF

/********************* Configure printf UART ***************************/

#if UART_PRINTF_MODE
#define DEBUG_INFO_TX_PIN       GPIO_PD3    //printf
#define DEBUG_BAUDRATE          115200

#endif /* UART_PRINTF_MODE */

#endif /* (BOARD == BOARD_JR_ZDS01) */

#endif /* SRC_COMMON_BOARD_JR_ZDS01_H_ */

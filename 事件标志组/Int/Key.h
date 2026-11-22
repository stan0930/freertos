#ifndef __KEY_H
#define __KEY_H

#include "main.h"

#define KEY_NONE      0U
#define KEY1_PRESS    1U
#define KEY2_PRESS    2U
#define KEY3_PRESS    3U
#define KEY4_PRESS    4U

#define KEY_DEBOUNCE_MS 10U

/* KEY1/PF8、KEY2/PF9 按下接地；KEY3/PF10、KEY4/PF11 按下接 3.3V。 */
#define KEY1_ACTIVE_LEVEL GPIO_PIN_RESET
#define KEY2_ACTIVE_LEVEL GPIO_PIN_RESET
#define KEY3_ACTIVE_LEVEL GPIO_PIN_SET
#define KEY4_ACTIVE_LEVEL GPIO_PIN_SET

#define KEY1 HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin)

#define KEY2 HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin)
#define KEY3 HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin)
#define KEY4 HAL_GPIO_ReadPin(KEY4_GPIO_Port, KEY4_Pin)

/* 仅由一个任务/主循环调用；按下触发一次，松开消抖后可再次触发。
 * 同时按下多个键，只返回编号最大的键：KEY4 > KEY3 > KEY2 > KEY1。
 */
uint8_t Key_Detect(void);

#endif

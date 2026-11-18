#ifndef __KEY_H
#define __KEY_H

#include "main.h"

#define KEY1        HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin)   /* 读取KEY1引脚状态（上拉输入） */

#define KEY1_PRESS    1              

uint8_t Key_Detect(void);

#endif

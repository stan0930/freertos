#include "Key.h"

/* 使用位图分别记录四个按键的按下状态。 */
static uint8_t Key_ReadMask(void)
{
    uint8_t pressed = 0U;

    if (KEY1 == KEY1_ACTIVE_LEVEL)
    {
        pressed |= (1U << 0);
    }
    if (KEY2 == KEY2_ACTIVE_LEVEL)
    {
        pressed |= (1U << 1);
    }
    if (KEY3 == KEY3_ACTIVE_LEVEL)
    {
        pressed |= (1U << 2);
    }
    if (KEY4 == KEY4_ACTIVE_LEVEL)
    {
        pressed |= (1U << 3);
    }

    return pressed;
}

uint8_t Key_Detect(void)
{
    static uint8_t stable_keys = 0U;
    uint8_t sampled_keys;
    uint8_t new_presses;

    sampled_keys = Key_ReadMask();
    if (sampled_keys == stable_keys)
    {
        return KEY_NONE;
    }

    /* 按下和松开均消抖；当前工程由 TIM7 为 HAL 提供毫秒时基。
     * 不等待松手，也不使用需要调度器运行的 vTaskDelay()。
     */
    HAL_Delay(KEY_DEBOUNCE_MS);
    if (sampled_keys != Key_ReadMask())
    {
        return KEY_NONE;
    }

    /* 只报告从松开变为按下的键，持续按住不会重复触发。 */
    new_presses = sampled_keys & (uint8_t)(~stable_keys);
    stable_keys = sampled_keys;

    if ((new_presses & (1U << 3)) != 0U)
    {
        return KEY4_PRESS;
    }
    if ((new_presses & (1U << 2)) != 0U)
    {
        return KEY3_PRESS;
    }
    if ((new_presses & (1U << 1)) != 0U)
    {
        return KEY2_PRESS;
    }
    if ((new_presses & (1U << 0)) != 0U)
    {
        return KEY1_PRESS;
    }

    return KEY_NONE;
}

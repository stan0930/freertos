#include "freertos_demo.h"
/*1.包含FreeRTOS相关头文件*/
#include "freertos.h"
#include "task.h"
/*需要用到的其他头文件*/
#include "stdio.h"
#include "Led.h"
#include "Key.h"

/*启动任务的配置*/
#define START_TASK_STACK 128
#define START_TASK_PRIORITY 1
TaskHandle_t start_task_handle;
void start_task(void *const pvParameters);

/*任务1的配置*/
#define TASK1_STACK 128
#define TASK1_PRIORITY 2
TaskHandle_t task1_handle;
void task1(void *pvParameters);

/**
 * @brief 启动FreeRTOS
 *
 */
void freertos_start(void)
{   
    /*1.创建一个启动任务*/
    xTaskCreate((TaskFunction_t)start_task,                     // 任务函数的地址
                (char *)"start_task",                           // 任务名称字符串
                (const configSTACK_DEPTH_TYPE)START_TASK_STACK, // 任务栈大小，默认大小128，单位4字节
                (void *)NULL,                                   // 传递给任务的参数
                (UBaseType_t)START_TASK_PRIORITY,               // 任务的优先级
                (TaskHandle_t *)&start_task_handle);            // 任务句柄的地址
    /*2.启动调度器：会自动创建空闲任务*/
    vTaskStartScheduler();//在这里启动
}


/**
 * @brief 启动任务:用来创建其他task
 * 
 * @param pvParameters 
 */
void start_task(void * pvParameters)
{   
    /*进入临界区：保护临界区里面的代码不会被打断*/
    portENTER_CRITICAL();// 不写临界区且不delay，task1被创建后就会一直在task1执行


    /*创建任务1*/
    xTaskCreate((TaskFunction_t)task1,                     // 任务函数的地址
                (char *)"task1",                           // 任务名称字符串
                (const configSTACK_DEPTH_TYPE)TASK1_STACK, // 任务栈大小，默认大小128，单位4字节
                (void *)NULL,                                   // 传递给任务的参数
                (UBaseType_t)TASK1_PRIORITY,               // 任务的优先级
                (TaskHandle_t *)&task1_handle);            // 任务句柄的地址
    /*退出临界区*/
    portEXIT_CRITICAL();

    /*启动任务只需要执行一次，用完就删除启动任务自己*/
    vTaskDelete(NULL);
}


/**
 * @brief 任务一：实现LED1每500ms闪烁一次
 * 
 * @param pvParameters 
 */
void task1(void *pvParameters){


    uint8_t key = 0;
    while(1)
    {   
        // printf("task1运行...\r\n");
        key = Key_Detect();
        if(key == KEY1_PRESS)
        {
            /*关中断*/
            printf(">>> 关中断...\r\n");
            portDISABLE_INTERRUPTS();
        }
        else if(key == KEY2_PRESS)
        {
            /*开中断*/
            printf(">>> 开中断...\r\n");
            portENABLE_INTERRUPTS();
        }

        /*开中断*/
        /*为了观察实验现象，不要调用freertos的延时函数，底层会去开关中断，影响现象*/
        // vTaskDelay(500);
        /*使用HAL_Delay前提：HAL时钟修改成其他定时器，并且中断优先级高于freeRTOS的管理范围*/
        HAL_Delay(500);
    }
}

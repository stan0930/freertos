#include "freertos_demo.h"
/*1.包含FreeRTOS相关头文件*/
#include "freertos.h"
#include "task.h"
#include "event_groups.h"//事件标志组头文件
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

/*任务2的配置*/
#define TASK2_STACK 128
#define TASK2_PRIORITY 3
TaskHandle_t task2_handle;
void task2(void *pvParameters);

/*任务3的配置*/
#define TASK3_STACK 128
#define TASK3_PRIORITY 4
TaskHandle_t task3_handle;
void task3(void *pvParameters);





/**
 * @brief 启动FreeRTOS
 *
 */

EventGroupHandle_t event_group_handle;
void freertos_start(void)
{   
    event_group_handle = xEventGroupCreate();//创建事件标志组
    if(event_group_handle == NULL){
        printf("创建事件标志组失败...\r\n");
    }


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


    /*创建任务1,任务2,任务3*/
    xTaskCreate((TaskFunction_t)task1,                     // 任务函数的地址
                (char *)"task1",                           // 任务名称字符串
                (const configSTACK_DEPTH_TYPE)TASK1_STACK, // 任务栈大小，默认大小128，单位4字节
                (void *)NULL,                                   // 传递给任务的参数
                (UBaseType_t)TASK1_PRIORITY,               // 任务的优先级
                (TaskHandle_t *)&task1_handle);            // 任务句柄的地址
    xTaskCreate((TaskFunction_t)task2,                     // 任务函数的地址
                (char *)"task2",                           // 任务名称字符串
                (const configSTACK_DEPTH_TYPE)TASK2_STACK, // 任务栈大小，默认大小128，单位4字节
                (void *)NULL,                                   // 传递给任务的参数
                (UBaseType_t)TASK2_PRIORITY,               // 任务的优先级
                (TaskHandle_t *)&task2_handle);            // 任务句柄的地址

    /*退出临界区*/
    portEXIT_CRITICAL();

    /*启动任务只需要执行一次，用完就删除启动任务自己*/
    vTaskDelete(NULL);
}


/**
 * @brief 任务一：读取按键按下键值，根据不同键值将事件标志组相应事件位置一，模拟事件发生。
 * 
 * @param pvParameters 
 */
#define EVENTBIT_0 (1<<0)
#define EVENTBIT_1 (1<<1)
#define EVENTBIT_2 (1<<2)



void task1(void *pvParameters){
    uint8_t key = 0;
    while(1){
        key = Key_Detect();
        if(key == KEY1_PRESS){
            /*key1按下，对bit0置1*/
            xEventGroupSetBits(event_group_handle, EVENTBIT_0);//底层去做或操作
            printf("key1按下，对bit0置1...\r\n");
        }else if(key == KEY2_PRESS){
            /*key2按下，对bit1置1*/
            xEventGroupSetBits(event_group_handle, EVENTBIT_1);//底层去做或操作
            printf("key2按下，对bit1置1...\r\n");
        }
        vTaskDelay(500);
    }
}

/**
 * @brief 任务二：同时等待事件标志组中的多个事件位，当这些事件位都置 1 的话就执行相应的处理。
 * 
 * @param pvParameters 
 */
void task2(void *pvParameters){
    EventBits_t event_bits = 0;
    while (1)
    {
        event_bits = xEventGroupWaitBits(
            event_group_handle, //事件标志组句柄
            EVENTBIT_0 | EVENTBIT_1,//要等待的标志位，可以写多个，用|拼接
            pdTRUE,             //满足等待的标志位后，是否清除对应的标志位
            pdTRUE,             //所有bit都成立还是任意一个成立
            portMAX_DELAY
        );
        printf("task2接收到的事件标志组=%#x......\r\n", event_bits);
    }
    
}

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
StackType_t start_task_stack[START_TASK_STACK]; // 静态任务栈的空间
StaticTask_t start_task_tcb;
void start_task(void *const pvParameters);

/*任务1的配置*/
#define TASK1_STACK 128
#define TASK1_PRIORITY 2
TaskHandle_t task1_handle;
StackType_t task1_stack[TASK1_STACK]; // 静态任务栈的空间
StaticTask_t task1_tcb;
void task1(void *pvParameters);

/*任务2的配置*/
#define TASK2_STACK 128
#define TASK2_PRIORITY 3
TaskHandle_t task2_handle;
StackType_t task2_stack[TASK2_STACK]; // 静态任务栈的空间
StaticTask_t task2_tcb;
void task2(void *pvParameters);

/*任务3的配置*/
#define TASK3_STACK 128
#define TASK3_PRIORITY 4
TaskHandle_t task3_handle;
StackType_t task3_stack[TASK3_STACK]; // 静态任务栈的空间
StaticTask_t task3_tcb;
void task3(void *pvParameters);

/*======静态创建方式，需要手动指定2个特殊任务的资源=====*/
/*空闲任务的配置*/
StackType_t idle_task_stack[configMINIMAL_STACK_SIZE]; // 静态任务栈的空间
StaticTask_t idle_task_tcb;

/*软件定时器任务的配置*/
StackType_t timer_task_stack[configTIMER_TASK_STACK_DEPTH]; // 静态任务栈的空间
StaticTask_t timer_task_tcb;


/*分配空闲任务的资源*/
void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer,
                                        StackType_t ** ppxIdleTaskStackBuffer,
                                        uint32_t * pulIdleTaskStackSize )
{
    *ppxIdleTaskTCBBuffer = &idle_task_tcb;
    *ppxIdleTaskStackBuffer = idle_task_stack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer,
                                         StackType_t ** ppxTimerTaskStackBuffer,
                                         uint32_t * pulTimerTaskStackSize )
{
    *ppxTimerTaskTCBBuffer = &timer_task_tcb;
    *ppxTimerTaskStackBuffer = timer_task_stack;
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}                                         


/**
 * @brief 启动FreeRTOS
 *
 */
void freertos_start(void)
{
    /*1.创建一个启动任务*/
    // xTaskCreate((TaskFunction_t)start_task,                     // 任务函数的地址
    //             (char *)"start_task",                           // 任务名称字符串
    //             (const configSTACK_DEPTH_TYPE)START_TASK_STACK, // 任务栈大小，默认大小128，单位4字节
    //             (void *)NULL,                                   // 传递给任务的参数
    //             (UBaseType_t)START_TASK_PRIORITY,               // 任务的优先级
    //             (TaskHandle_t *)&start_task_handle);            // 任务句柄的地址

    start_task_handle = xTaskCreateStatic((TaskFunction_t)start_task,       // 任务函数的地址
                                          (char *)"start_task",             // 任务名称字符串
                                          (uint32_t)START_TASK_STACK,       // 任务栈大小，默认大小128，单位4字节
                                          (void *)NULL,                     // 传递给任务的参数
                                          (UBaseType_t)START_TASK_PRIORITY, // 任务的优先级
                                          (StackType_t *)start_task_stack,  // 我们创建的任务栈（数组存储）的地址
                                          (StaticTask_t *)&start_task_tcb); // 我们创建的tcb的地址

    /*2.启动调度器：会自动创建空闲任务和软件定时器（如果开启了软件定时器），
    静态创建的方式需要去实现两个分配资源的接口函数*/
    vTaskStartScheduler(); // 在这里启动
}

/**
 * @brief 启动任务:用来创建其他task
 *
 * @param pvParameters
 */
void start_task(void *pvParameters)
{
    /*进入临界区：保护临界区里面的代码不会被打断*/
    portENTER_CRITICAL(); // 不写临界区且不delay，task1被创建后就会一直在task1执行
    //使用静态创建任务1,任务2,任务3
    task1_handle = xTaskCreateStatic(
        (TaskFunction_t)task1,       // 任务函数的地址
        (char *)"task1",             // 任务名称字符串
        (uint32_t)TASK1_STACK,       // 任务栈大小，默认大小128，单位4字节
        (void *)NULL,                // 传递给任务的参数
        (UBaseType_t)TASK1_PRIORITY, // 任务的优先级
        (StackType_t *)task1_stack,  // 我们创建的任务栈（数组存储）的地址
        (StaticTask_t *)&task1_tcb); // 我们创建的tcb的地址
    task2_handle = xTaskCreateStatic(
        (TaskFunction_t)task2,       // 任务函数的地址
        (char *)"task2",             // 任务名称字符串
        (uint32_t)TASK2_STACK,       // 任务栈大小，默认大小128，单位4字节
        (void *)NULL,                // 传递给任务的参数
        (UBaseType_t)TASK2_PRIORITY, // 任务的优先级
        (StackType_t *)task2_stack,  // 我们创建的任务栈（数组存储）的地址
        (StaticTask_t *)&task2_tcb); // 我们创建的tcb的地址
    task3_handle = xTaskCreateStatic(
        (TaskFunction_t)task3,       // 任务函数的地址
        (char *)"task3",             // 任务名称字符串
        (uint32_t)TASK3_STACK,       // 任务栈大小，默认大小128，单位4字节
        (void *)NULL,                // 传递给任务的参数
        (UBaseType_t)TASK3_PRIORITY, // 任务的优先级
        (StackType_t *)task3_stack,  // 我们创建的任务栈（数组存储）的地址
        (StaticTask_t *)&task3_tcb); // 我们创建的tcb的地址
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
void task1(void *pvParameters)
{

    while (1)
    {
        printf("task1运行...\r\n");
        LED_Toggle(LED1_Pin);
        vTaskDelay(500);
    }
}

/**
 * @brief 任务二：实现LED2每500ms闪烁一次
 *
 * @param pvParameters
 */
void task2(void *pvParameters)
{
    while (1)
    {
        printf("task2运行...\r\n");
        LED_Toggle(LED2_Pin);
        vTaskDelay(500);
    }
}

/**
 * @brief 任务三：判断按键key1是否按下，按下则删掉task1
 *
 * @param pvParameters
 */
void task3(void *pvParameters)
{
    uint8_t key = 0;
    while (1)
    {
        printf("task3运行...\r\n");
        key = Key_Detect();
        if (key == KEY1_PRESS)
        {
            /*判断task1是否存在，存在则删除task1*/
            if (task1_handle != NULL)
            {
                /* key1按下了，删除task1*/
                printf("key1按下了，删除task1...\r\n");
                vTaskDelete(task1_handle);
                task1_handle = NULL; // 删除任务后手动置空句柄
            }
        }
        vTaskDelay(500);
        // HAL_Delay(500);//这种delay不会让任务进入阻塞态，不会让cpu使用权
    }
}
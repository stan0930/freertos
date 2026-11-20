#include "freertos_demo.h"
/*1.包含FreeRTOS相关头文件*/
#include "freertos.h"
#include "task.h"
#include "semphr.h" //信号量相关头文件
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


/**
 * @brief 启动FreeRTOS
 *
 */
QueueHandle_t sem_count_handle;
void freertos_start(void)
{   
    UBaseType_t count = 0;
    /*创建计数型信号量*/
    sem_count_handle = xSemaphoreCreateCounting(100, 0);
    if(sem_count_handle == NULL){
        printf("创建计数型信号量失败\r\n");
    }
    else{
        /*创建成功,获取一次计数值打印出来*/
        count = uxSemaphoreGetCount(sem_count_handle);
        printf("创建计数型信号量成功，初始计数值为%d\r\n", count);
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
 * @brief 任务一：用于按键扫描，当检测到按键KEY1被按下时，释放计数型信号量。
 * 
 * @param pvParameters 
 */
void task1(void *pvParameters){
    uint8_t key=0;
    BaseType_t res=0;

    while(1){
        key = Key_Detect();
        if(key==KEY1_PRESS){
            /*释放计数型信号量*/
            res = xSemaphoreGive(sem_count_handle);
            if(res == pdPASS){
                printf("Task1释放计数型信号量成功\r\n");
            }else{
                printf("Task1释放计数型信号量失败\r\n");
            }
        }
        vTaskDelay(500);
    }
}

/**
 * @brief 每过一秒获取一次计数型信号量，当成功获取后打印信号量计数值。
 * 
 * @param pvParameters 
 */
void task2(void *pvParameters){
    BaseType_t res = 0;
    UBaseType_t current_count = 0;
    while (1)
    {
        /*获取计数型信号量*/
        res = xSemaphoreTake(sem_count_handle,portMAX_DELAY);
        if(res == pdPASS){
            printf("Task2获取计数型信号量成功\r\n");
        }else{
            printf("Task2获取计数型信号量失败\r\n");
        }
        current_count = uxSemaphoreGetCount(sem_count_handle);
        printf("当前计数值为%d\r\n", current_count);
        vTaskDelay(1000);
    }
    
}


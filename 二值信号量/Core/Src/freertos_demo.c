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

/*任务3的配置*/
#define TASK3_STACK 128
#define TASK3_PRIORITY 4
TaskHandle_t task3_handle;
void task3(void *pvParameters);





/**
 * @brief 启动FreeRTOS
 *
 */
QueueHandle_t sem_handle;
void freertos_start(void)
{   
    /*创建二值信号量*/
    //xSemaphoreCreateBinary();//x开头的，不会主动释放信号量
    vSemaphoreCreateBinary(sem_handle);//v开头的，创建完会主动释放信号量
    if(sem_handle == NULL){
        printf("创建二值信号量失败\r\n");
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
 * @brief 任务一：用于按键扫描，当检测到按键KEY1被按下时，释放二值信号量。
 * 
 * @param pvParameters 
 */
void task1(void *pvParameters){
    uint8_t key=0;
    BaseType_t res=0;

    while(1){
        key = Key_Detect();
        if(key==KEY1_PRESS){
            /*释放二值信号量*/
            res = xSemaphoreGive(sem_handle);
            if(res == pdPASS){
                printf("Task1释放二值信号量成功\r\n");
            }else{
                printf("Task1释放二值信号量失败\r\n");
            }
        }
        vTaskDelay(500);
    }
}

/**
 * @brief 任务二：获取二值信号量，当成功获取后打印提示信息。
 * 
 * @param pvParameters 
 */
void task2(void *pvParameters){
    BaseType_t res = 0;
    while (1)
    {
        /*获取二值信号量*/
        res = xSemaphoreTake(sem_handle,portMAX_DELAY);
        if(res == pdPASS){
            printf("Task2获取二值信号量成功\r\n");
        }else{
            printf("Task2获取二值信号量失败\r\n");
        }
        vTaskDelay(500);
    }
    
}


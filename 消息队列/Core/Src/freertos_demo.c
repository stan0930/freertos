#include "freertos_demo.h"
/*1.包含FreeRTOS相关头文件*/
#include "freertos.h"
#include "task.h"
#include "queue.h"
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

/*队列句柄*/
QueueHandle_t queue1;
QueueHandle_t big_queue;




/**
 * @brief 启动FreeRTOS
 *
 */
void freertos_start(void)
{   

    /*在创建任务之前，先创建好需要的队列*/
    /*创建一个存放KEY值编号的小队列queue1*/
    queue1 = xQueueCreate(2,sizeof(uint8_t));
    if(queue1 != NULL)
    {
        printf("queue1创建成功\r\n");
    }else{
        printf("queue1创建失败\r\n");
    }


    /*创建一个存放大数据的队列big_queue ===> 存放地址就好*/
    big_queue = xQueueCreate(1,sizeof(char *));
    if(queue1 != NULL)
    {
        printf("big_queue创建成功\r\n");
    }else{
        printf("big_queue创建失败\r\n");
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
    xTaskCreate((TaskFunction_t)task3,                     // 任务函数的地址
                (char *)"task3",                           // 任务名称字符串
                (const configSTACK_DEPTH_TYPE)TASK3_STACK, // 任务栈大小，默认大小128，单位4字节
                (void *)NULL,                                   // 传递给任务的参数
                (UBaseType_t)TASK3_PRIORITY,               // 任务的优先级
                (TaskHandle_t *)&task3_handle);            // 任务句柄的地址

    /*退出临界区*/
    portEXIT_CRITICAL();

    /*启动任务只需要执行一次，用完就删除启动任务自己*/
    vTaskDelete(NULL);
}


/**
 * @brief 任务一：当按键key1或key2按下，将键值拷贝到队列queue1（入队）；当按键key3按下，将传输大数据，这里拷贝大数据的地址到队列big_queue中。
 * 
 * @param pvParameters 
 */
void task1(void *pvParameters){
    char * big_data = "hello world 123!@$#$%^1231231231+_)(*&^^%$$#@!)";
    uint8_t key = 0;
    uint8_t res = 0;
    while(1){
        key = Key_Detect();
        if(key == KEY1_PRESS || key == KEY2_PRESS)
        {   
            /*将Key的编号写入queue1 */
            res = xQueueSend(queue1,&key,portMAX_DELAY);//阻塞写入
            if(res == pdPASS)
            {
                printf("往queue1发送数据[%d]成功\r\n",key);
            }else{
                printf("往queue1发送数据失败\r\n");
            }
        }
        else if(key == KEY3_PRESS)
        {
            /*将大数据的地址写入big_queue */
            res = xQueueSend(big_queue,&big_data,portMAX_DELAY);//阻塞写入,如果队列满，会阻塞等待
            if(res == pdPASS)
            {
                printf("往big_queue发送数据成功\r\n");
            }else{
                printf("往big_queue发送数据失败\r\n");
            }
        }

        vTaskDelay(500);
    }
}

/**
 * @brief 任务二：读取队列queue1中的消息（出队），打印出接收到的键值。
 * 
 * @param pvParameters 
 */
void task2(void *pvParameters){
    uint8_t receive = 0;
    uint8_t res=0;
    while (1)
    {
        res = xQueueReceive(queue1,&receive,portMAX_DELAY);//阻塞出队
            if(res == pdPASS)
            {
                printf("task2从queue1接收数据[%d]成功\r\n",receive);
            }else{
                printf("task2从queue1接收数据失败\r\n");
            }

    }
    
}

/**
 * @brief 任务三：从队列big_queue读取大数据地址，通过地址访问大数据。
 * 
 * @param pvParameters 
 */
void task3(void *pvParameters){
    uint8_t key = 0;
    char * recevie_buff = NULL;
    uint8_t res=0;
    while (1)
    {
        res = xQueueReceive(big_queue,&recevie_buff,portMAX_DELAY);//阻塞出队
            if(res == pdPASS)
            {
                printf("task3从big_queue接收数据[%s]成功\r\n",recevie_buff);
            }else{
                printf("task3从big_queue接收数据失败\r\n");
            }
    }
}
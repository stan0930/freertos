#include "freertos_demo.h"
/*1.包含FreeRTOS相关头文件*/
#include "freertos.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
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
QueueHandle_t sem_handle;
QueueSetHandle_t queue_set_handle;



/**
 * @brief 启动FreeRTOS
 *
 */
void freertos_start(void)
{   
    BaseType_t res=0;

    /*在创建任务之前，先创建好需要的队列*/
    /*创建一个存放KEY值编号的小队列queue1*/
    queue1 = xQueueCreate(2,sizeof(uint8_t));
    if(queue1 != NULL)
    {
        printf("queue1创建成功\r\n");
    }else{
        printf("queue1创建失败\r\n");
    }


    /*创建一个二值信号量*/
    sem_handle = xSemaphoreCreateBinary();
    if(sem_handle != NULL)
    {
        printf("二值信号量创建成功\r\n");
    }else{
        printf("二值信号量创建失败\r\n");
    }

    /*创建队列集*/
    queue_set_handle = xQueueCreateSet(2);
    if(queue_set_handle != NULL){
        printf("队列集创建成功\r\n");
    }else{
        printf("队列集创建失败\r\n");
    }
   /*将队列，信号量添加到队列集中*/
    res = xQueueAddToSet(queue1, queue_set_handle);
    if(res == pdTRUE){
        printf("queue1添加成功\r\n");
    }else{
        printf("queue1添加失败\r\n");
    }
    res = xQueueAddToSet(sem_handle, queue_set_handle);
    if(res == pdTRUE){
        printf("二值信号量添加成功\r\n");
    }else{
        printf("二值信号量添加失败\r\n");
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
 * @brief 任务一：用于扫描按键，当KEY1按下，往队列写入数据，当KEY2按下，释放二值信号量。
 * 
 * @param pvParameters 
 */
void task1(void *pvParameters){
    uint8_t key = 0;
    uint8_t res = 0;
    while(1){
        key = Key_Detect();
        if(key == KEY1_PRESS)
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
        else if(key == KEY2_PRESS)
        {
            /*释放二值信号量*/
            res = xSemaphoreGive(sem_handle);
            if(res == pdPASS)
            {
                printf("释放二值信号量成功\r\n");
            }else{
                printf("释放二值信号量失败\r\n");
            }
        }

        vTaskDelay(500);
    }
}

/**
 * @brief 任务二：读取队列集中的消息，并打印
 * 
 * @param pvParameters 
 */
void task2(void *pvParameters){
    QueueSetMemberHandle_t member_handle;
    uint8_t receive = 0;
    uint8_t res=0;
    while (1)
    {   
        /* 查看哪个队列有数据来了 */
        member_handle = xQueueSelectFromSet(queue_set_handle,portMAX_DELAY);
        /* 根据对应的队列，去获取数据 */
        if(member_handle == queue1){
            res = xQueueReceive(queue1,&receive,portMAX_DELAY);//阻塞出队
            if(res == pdPASS)
            {
                printf("task2从queue1接收数据[%d]成功\r\n",receive);
            }else{
                printf("task2从queue1接收数据失败\r\n");
            }
        }else if(member_handle == sem_handle){
            res = xSemaphoreTake(sem_handle,portMAX_DELAY);
            if(res == pdPASS)
            {
                printf("task2从二值信号量接收数据成功\r\n");
            }else{
                printf("task2从二值信号量接收数据失败\r\n");
            }
        }else{
            printf("从队列集接收数据失败，未知队列成员\r\n");
        }
    }
    
}


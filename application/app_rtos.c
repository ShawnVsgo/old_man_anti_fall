#include "app_rtos.h"
// task1的任务参数
void buzzer_task(void *args); //任务函数
#define TASK1_STK_SIZE 128 //定义栈的深度 128*32 =512字节 32位单片机
#define TASK1_PRIO 1 //任务优先级
TaskHandle_t task1_handle = {0}; //任务句柄

// task2的任务参数
void task2(void *args);
#define TASK2_STK_SIZE 128
#define TASK2_PRIO 2
TaskHandle_t task2_handle = {0};

// task3的任务参数
void task3(void *args);
#define TASK3_STK_SIZE 128
#define TASK3_PRIO 3
TaskHandle_t task3_handle = {0};
void App_FreeRTOS_Start(void)
{
    Driver_I2C2_Init();
    Int_MPU6050_Init();
    // 1. 创建任务
    xTaskCreate(buzzer_task, "task1", TASK1_STK_SIZE, NULL, TASK1_PRIO, &task1_handle);
    xTaskCreate(task2, "task2", TASK2_STK_SIZE, NULL, TASK2_PRIO, &task2_handle);
    xTaskCreate(task3, "task3", TASK3_STK_SIZE, NULL, TASK3_PRIO, &task3_handle);
   
    // 2. 启动任务调度器 => 启动freeRTOS操作系统
    vTaskStartScheduler();
}

void task1(void *args)
{
    while (1)
    {
        debug_println("task1 is running...");
        // 必须使用freeRTOS的延时  => 将当前任务阻塞500ms
        vTaskDelay(500);
    }
}

void task2(void *args)
{
    while (1)
    {
        debug_println("task2 is running...");
        // 必须使用freeRTOS的延时  => 将当前任务阻塞500ms
        vTaskDelay(500);
    }
}

void task3(void *args)
{
    while (1)
    {
        // 如果高优先级的不阻塞自身  调度器永远调度高优先级任务  => 别人永远执行不了
       debug_println("task3 is running...");
        // 必须使用freeRTOS的延时  => 将当前任务阻塞500ms
        vTaskDelay(500);
    }
}

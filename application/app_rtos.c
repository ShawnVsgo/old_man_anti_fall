#include "app_rtos.h"
#include <timers.h>
// task1的任务参数
void alarm_task(void *args); //任务函数
#define ALARM_STK_SIZE 128 //定义栈的深度 128*32 =512字节 32位单片机
#define ALARM_PRIO 3 //任务优先级
TaskHandle_t alarm_handle = {0}; //任务句柄

// task2的任务参数
void detect_fall_task(void *args);
#define DETECT_FALL_STK_SIZE 128
#define DETECT_FALL_PRIO 3
TaskHandle_t detect_fall_task_handle = {0};

void App_FreeRTOS_Start(void)
{
   
    // 1. 创建任务
    xTaskCreate(alarm_task, "task1", ALARM_STK_SIZE, NULL, ALARM_PRIO, &alarm_handle);
    xTaskCreate(detect_fall_task, "task2", DETECT_FALL_STK_SIZE, NULL, DETECT_FALL_PRIO, &detect_fall_task_handle);
   
    // 2. 启动任务调度器 => 启动freeRTOS操作系统
    vTaskStartScheduler();
}

TimerHandle_t xAlarmTimer;

void vAlarmTimerCallback(TimerHandle_t xTimer)
{
    Inf_Buzzer_Alarm(0);
}

void alarm_task(void *args)
{
    // 创建软件定时器
    xAlarmTimer = xTimerCreate("AlarmTimer", 
        pdMS_TO_TICKS(500),  // 500ms周期
        pdTRUE,              // 自动重载
        0,
        vAlarmTimerCallback); // 定时器回调函数
        while (1)
        {
            xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
            
            // 启动定时器，周期性触发蜂鸣器
            xTimerStart(xAlarmTimer, 0);
            
            vTaskDelay(50);
        }
}

void detect_fall_task(void *args)
{
    Driver_I2C2_Init(); 
    Int_MPU6050_Init(); //vTaskStartScheduler();  // 之后才能使用vTaskDelay等API
    uint8_t is_send_messege = 0;
    while (1)
    {
        if (app_fall_detect()==true&&is_send_messege == 0)
        {
            //1. 空气蜂鸣器
            xTaskNotify(alarm_handle, 0, eNoAction); // 发送通知
            //2. 发送警报信息 
            App_Exit_Low_Power();
            App_Location_Get_Step_Count();
            App_Location_Get_GPS_Data();
            App_Location_Data_to_JSON();
            App_Location_Send_Data();
            APP_Enter_Low_Power();

            is_send_messege = 1;
        }
        
        vTaskDelay(6);
    }
}


#include "app_rtos.h"
#include <timers.h>
#include "Inf_qs100_mqtt.h"
// 用于创建任务的任务
void App_FreeRTOS_Start_task(void *args);
#define APP_FREERTOS_START_STK_SIZE 128       // 定义栈的深度 128*32 =512字节 32位单片机
#define APP_FREERTOS_START_PRIO 1             // 任务优先级
TaskHandle_t App_FreeRTOS_Start_handle = {0}; // 任务句柄

// task1的任务参数
void alarm_task(void *args);     // 任务函数
#define ALARM_STK_SIZE 128       // 定义栈的深度 128*32 =512字节 32位单片机
#define ALARM_PRIO 4             // 任务优先级
TaskHandle_t alarm_handle = {0}; // 任务句柄

// task2的任务参数
void detect_fall_task(void *args);
#define DETECT_FALL_STK_SIZE 128
#define DETECT_FALL_PRIO 4
TaskHandle_t detect_fall_task_handle = {0};

// task3的任务参数
void test_task(void *args);
#define TEST_STK_SIZE 128
#define TEST_PRIO 3
TaskHandle_t test_handle = {0};

void App_FreeRTOS_Start(void)
{
    // 创建一个任务用来创建其他任务
    xTaskCreate(App_FreeRTOS_Start_task, "App_FreeRTOS_Start_task", APP_FREERTOS_START_STK_SIZE, NULL, APP_FREERTOS_START_PRIO, &App_FreeRTOS_Start_handle);
    // 2. 启动任务调度器 => 启动freeRTOS操作系统
    vTaskStartScheduler();
}

void App_FreeRTOS_Start_task(void *args)
{
    // 1. 初始化硬件外设
    Int_MPU6050_Init(); // vTaskStartScheduler();  // 之后才能使用vTaskDelay等API
    Int_DS3553_Init();
    Int_AT6558R_EXIT_Low_Power(); // 退出GPS的低功耗

    Inf_QS100_MQTT_Init();
    Inf_QS100_MQTT_CreatClient();
    Inf_QS100_MQTT_Connect();

    Int_AT6558R_Init();
    // 0. 初始化
    Driver_I2C2_Init();

    // 1. 创建任务
    xTaskCreate(alarm_task, "task1", ALARM_STK_SIZE, NULL, ALARM_PRIO, &alarm_handle);
    xTaskCreate(detect_fall_task, "task2", DETECT_FALL_STK_SIZE, NULL, DETECT_FALL_PRIO, &detect_fall_task_handle);
    // xTaskCreate(test_task, "task3", TEST_STK_SIZE, NULL, TEST_PRIO, &test_handle);

    // 3. 删除当前任务
    vTaskDelete(NULL); // 删除当前任务
}

void alarm_task(void *args)
{
    uint8_t buzzer_active = 0;
    
    while (1)
    {
        if (xTaskNotifyWait(0, 0, NULL, 0) == pdTRUE)
        {
            buzzer_active = 1;
        }
        if (buzzer_active)
        {
            Inf_Buzzer_Alarm(0);
            vTaskDelay(pdMS_TO_TICKS(50));  // 500ms间隔
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(50));   // 空闲时的延时
        }
    }
}

void detect_fall_task(void *args)
{
    uint8_t is_send_messege = 0;
    while (1)
    {

        if (app_fall_detect() == true && is_send_messege == 0)
        {
            //1. 空气蜂鸣器
            xTaskNotify(alarm_handle, 0, eNoAction); // 发送通知
            // 2. 发送警报信息
            // App_Location_Get_Step_Count();
            // App_Location_Get_GPS_Data();
            // App_Location_Data_to_JSON();
            // App_Location_Send_Data();
            is_send_messege = 1;
        }

        vTaskDelay(6);
    }
}

void test_task(void *args)
{

    while (1)
    {
        App_Location_Get_Step_Count();
        App_Location_Get_GPS_Data();
        App_Location_Data_to_JSON();
        Inf_QS100_MQTT_Publish(upload_data.data, strlen((char *)upload_data.data));
        vTaskDelay(20000); // 20秒延时
    }
}

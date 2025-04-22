#ifndef __APP_RTOS_H__
#define __APP_RTOS_H__

#include "FreeRTOS.h"
#include "task.h"
#include "Com_Debug.h"
#include "Inf_buzzer.h"
#include "Driver_I2C2.h"
#include "Inf_MPU6050.h"
#include "app_detect_fall.h"
#include "app_location.h"

void App_FreeRTOS_Start(void);
#endif /* __APP_RTOS_H__ */

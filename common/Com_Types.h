#ifndef __COM_TYPES_H__
#define __COM_TYPES_H__
#include "gpio.h"
typedef enum
{
    COM_OK,
    COM_ERROR,
}Com_Status;
// typedef struct
// {
//  TIM_HandleTypeDef *htim;
//  uint32_t Channel;
//  int16_t speed;
// }Motor_Struct;

/// @brief 遥控器的状态
typedef enum{
    DISCONNECTED,
    CONNECTED,
}Remote_Status;
/// @brief 飞行状态
typedef enum{
    IDELE,
    NORMAL,
    FIX_HIGHT,//固定高度
    FAULT
}Flight_Status;

/// @brief LED灯结构体
typedef struct
{
    GPIO_TypeDef *port;
    uint16_t pin;
    uint32_t last_tick;
    
}led_struct;
//遥控器传过来的数据结构
typedef struct
{
    int16_t THR;
    int16_t YAW;
    int16_t PIT;
    int16_t ROL;
    uint8_t FIX_HEIGHT;
    uint8_t SHUT_DOWN;
} Remote_Struct;


typedef enum
{
    FREE,
    MAX,
    MIN,
    LEAVE_MAX,
    UNLOCK
} Throttle_State;
// 陀螺仪结构体
typedef struct
{
    int16_t x;
    int16_t y;
    int16_t z;
} Gyro_Struct;

// 加速度结构体
typedef struct
{
    int16_t x;
    int16_t y;
    int16_t z;
} Accel_Struct;

typedef struct
{
    Gyro_Struct gyro;
    Accel_Struct accel;
}GyroAccel_Struct;

//欧拉角数据结构
typedef struct {
    float roll;
    float pitch;
    float yaw;
} EulerAngle;


#endif /* __COM_TYPES_H__ */

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

/// @brief ң������״̬
typedef enum{
    DISCONNECTED,
    CONNECTED,
}Remote_Status;
/// @brief ����״̬
typedef enum{
    IDELE,
    NORMAL,
    FIX_HIGHT,//�̶��߶�
    FAULT
}Flight_Status;

/// @brief LED�ƽṹ��
typedef struct
{
    GPIO_TypeDef *port;
    uint16_t pin;
    uint32_t last_tick;
    
}led_struct;
//ң���������������ݽṹ
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
// �����ǽṹ��
typedef struct
{
    int16_t x;
    int16_t y;
    int16_t z;
} Gyro_Struct;

// ���ٶȽṹ��
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

//ŷ�������ݽṹ
typedef struct {
    float roll;
    float pitch;
    float yaw;
} EulerAngle;


#endif /* __COM_TYPES_H__ */

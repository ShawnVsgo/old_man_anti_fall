#ifndef __COM_IMU_H__
#define __COM_IMU_H__
#include "Com_Types.h"
#include "math.h"

/* ��ʾ��Ԫ���Ľṹ�� */
typedef struct
{
    float q0;
    float q1;
    float q2;
    float q3;
} Quaternion_Struct;

extern float RtA;
extern float Gyro_G;
extern float Gyro_Gr;

void Com_IMU_GetEulerAngle(GyroAccel_Struct *gyroAccel,
                              EulerAngle *eulerAngle,
                              float dt);
float Com_IMU_GetNormAccZ(void);

#endif /* __COM_IMU_H__ */

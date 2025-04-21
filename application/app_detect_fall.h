#ifndef __APP_DETECT_FALL_H__
#define __APP_DETECT_FALL_H__

#include "Inf_mpu6050.h"
#include "stdbool.h"
#include "math.h"
#include "stdlib.h"
#include "Com_IMU.h"
#include "Com_Filter.h"
// 添加跌倒检测相关定义
#define FALL_ACCEL_THRESHOLD    0.1f    // 加速度阈值 (约2.5g)
#define FALL_ANGLE_THRESHOLD    70      // 角度阈值 (度)
#define STATIC_DETECT_TIME      3000     // 静止检测时间(ms)
// 跌倒检测状态
typedef enum {
    FALL_STATE_NORMAL = 0,
    FALL_STATE_IMPACT, // 加速度突变状态
    FALL_STATE_LYING // 跌倒状态
} FallState;

bool app_fall_detect(void);
#endif /* __APP_DETECT_FALL_H__ */

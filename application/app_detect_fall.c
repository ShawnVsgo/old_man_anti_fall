#include "app_detect_fall.h"
Gyro_Struct last_gyro;
EulerAngle euler_angle;
static uint32_t last_time = 0;
float dt = 0;
bool app_fall_detect(void)
{
    static FallState fall_state = FALL_STATE_NORMAL;
    static uint32_t impact_time = 0;
    GyroAccel_Struct mpu_data;
    float acc_total;
    // float angle;
    // 在读取MPU6050数据之前计算dt
    uint32_t current_time = HAL_GetTick();
    dt = (current_time - last_time) / 1000.0f;  // 转换为秒
    last_time = current_time;

    // 读取MPU6050数据
    Int_MPU6050_ReadGryo_Accel(&mpu_data);
     // 对角速度 使用一阶低通滤波
     mpu_data.gyro.x = Com_Filter_LowPass(mpu_data.gyro.x, last_gyro.x);
     mpu_data.gyro.y = Com_Filter_LowPass(mpu_data.gyro.y, last_gyro.y);
     mpu_data.gyro.z = Com_Filter_LowPass(mpu_data.gyro.z, last_gyro.z);
     last_gyro = mpu_data.gyro;
     // 对加速度 使用卡尔曼滤波
     mpu_data.accel.x = Com_Filter_KalmanFilter(&kfs[0], mpu_data.accel.x);
     mpu_data.accel.y = Com_Filter_KalmanFilter(&kfs[1], mpu_data.accel.y);
     mpu_data.accel.z = Com_Filter_KalmanFilter(&kfs[2], mpu_data.accel.z);
    
    // 计算欧拉角
    Com_IMU_GetEulerAngle(&mpu_data, &euler_angle, dt);
    // 计算Z轴加速度
    acc_total = Com_IMU_GetNormAccZ()/8192.0f;
    
    switch(fall_state)
    {
        case FALL_STATE_NORMAL:
            // 检测冲击
            if(acc_total > FALL_ACCEL_THRESHOLD) {
                fall_state = FALL_STATE_IMPACT;
                impact_time = HAL_GetTick();
            }
            break;
            
        case FALL_STATE_IMPACT:
            debug_println("acc_total: %f", acc_total);
            if((abs(euler_angle.pitch) > FALL_ANGLE_THRESHOLD)||(abs(euler_angle.roll) > FALL_ANGLE_THRESHOLD) ||(abs(euler_angle.yaw) > FALL_ANGLE_THRESHOLD)) {
                fall_state = FALL_STATE_LYING;
            } else  {
                fall_state = FALL_STATE_NORMAL;
            }
            break;
            
        case FALL_STATE_LYING:
            debug_println("pitch: %f, roll: %f, yaw: %f", euler_angle.pitch, euler_angle.roll, euler_angle.yaw);
            if ((abs(euler_angle.pitch) < 30)&&(abs(euler_angle.roll) < 30) &&(abs(euler_angle.yaw) < 30))
            {
                fall_state = FALL_STATE_NORMAL;
            }
            if(HAL_GetTick() - impact_time > STATIC_DETECT_TIME)
            {
                fall_state = FALL_STATE_NORMAL;
                return true; // 检测到跌倒
            }
            break;
    }
    
    return false;
}

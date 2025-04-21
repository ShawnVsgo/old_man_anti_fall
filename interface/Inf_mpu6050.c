#include "Inf_mpu6050.h"
void Int_MPU6050_Calibration(void);
GyroAccel_Struct gyro_acc_bias;
void Int_MPU6050_Write_Byte(uint8_t reg, uint8_t data)
{
    // HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR_W, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
    Driver_I2C2_Start();
    Driver_I2C2_SendByte(MPU6050_ADDR_W);
    Driver_I2C2_ReceiveACK();
    Driver_I2C2_SendByte(reg);
    Driver_I2C2_ReceiveACK();
    Driver_I2C2_SendByte(data);
    Driver_I2C2_ReceiveACK();
    Driver_I2C2_Stop();

}
void Int_MPU6050_Read_Byte(uint8_t reg, uint8_t *data)
{
    // HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR_R, reg, I2C_MEMADD_SIZE_8BIT, data, 1, 100);
    Driver_I2C2_Start();
    Driver_I2C2_SendByte(MPU6050_ADDR_W);
    Driver_I2C2_ReceiveACK();
    Driver_I2C2_SendByte(reg);
    Driver_I2C2_ReceiveACK();
    Driver_I2C2_Start();
    Driver_I2C2_SendByte(MPU6050_ADDR_R);
    Driver_I2C2_ReceiveACK();
    *data = Driver_I2C2_ReceiveByte();
    Driver_I2C2_SendACK(1);
    Driver_I2C2_Stop();

}
void Int_MPU6050_Read_Bytes(uint8_t reg, uint8_t *data, uint8_t len)
{
    // HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR_R, reg, I2C_MEMADD_SIZE_8BIT, data, len, 100);
    Driver_I2C2_Start();
    Driver_I2C2_SendByte(MPU6050_ADDR_W);
    Driver_I2C2_ReceiveACK();
    Driver_I2C2_SendByte(reg);
    Driver_I2C2_ReceiveACK();
    Driver_I2C2_Start();
    Driver_I2C2_SendByte(MPU6050_ADDR_R);
    Driver_I2C2_ReceiveACK();
    for (int i = 0; i < len; i++)
    {
        data[i] = Driver_I2C2_ReceiveByte();
        if (i == len - 1)
        {
            Driver_I2C2_SendACK(1);
        }
        else
        {
            Driver_I2C2_SendACK(0);
        }
    }
    Driver_I2C2_Stop();
}
void Int_MPU6050_Init(void)
{
    // 1.I2C初始化
    //  HAL_I2C_Init(&hi2c1);
    // 2.复位（复位后的值不是0x00而是0x40）
    Int_MPU6050_Write_Byte(MPU_PWR_MGMT1_REG, 0X80);
    HAL_Delay(100);
    // 3.唤醒(将0x40改为0x00)即将SLEEP位清零
    Int_MPU6050_Write_Byte(MPU_PWR_MGMT1_REG, 0X00);
    // 4.设置角速度传感器(陀螺仪)和加速度传感器的满量程范围
    Int_MPU6050_Write_Byte(MPU_GYRO_CFG_REG, 0X18);  //+/-2000dps
    //设置量程+/-4g
    Int_MPU6050_Write_Byte(MPU_ACCEL_CFG_REG, 0X08); //+/-4g
    // Int_MPU6050_Write_Byte(MPU_ACCEL_CFG_REG, 0X00); //+/-2g
    // 5.设置采样率和低通滤波器
    Int_MPU6050_Write_Byte(MPU_SAMPLE_RATE_REG, 0X01); // 
    Int_MPU6050_Write_Byte(MPU_CFG_REG, 0X02);         // 
    // 6.配置系统时钟源 并始能角速度和加速度传感器
    Int_MPU6050_Write_Byte(MPU_PWR_MGMT1_REG, 0X01); // 时钟源为X轴
    Int_MPU6050_Write_Byte(MPU_PWR_MGMT2_REG, 0X00); // 使能加速度和陀螺仪

    // 7.校准芯片
    Int_MPU6050_Calibration();
}
void Int_MPU6050_Read_Gyro(Gyro_Struct *gyro)
{
    uint8_t data[6];
    Int_MPU6050_Read_Bytes(MPU_GYRO_XOUTH_REG, data, 6);
    gyro->x = (data[0] << 8 | data[1]);
    gyro->y = (data[2] << 8 | data[3]);
    gyro->z = (data[4] << 8 | data[5]);
}
void Int_MPU6050_Read_Accel(Accel_Struct *accel)
{
    uint8_t data[6];
    Int_MPU6050_Read_Bytes(MPU_ACCEL_XOUTH_REG, data, 6);
    // 高位在前
    accel->x = (data[0] << 8) | data[1];
    accel->y = (data[2] << 8) | data[3];
    accel->z = (data[4] << 8) | data[5];
}

void Int_MPU6050_ReadGryo_Accel(GyroAccel_Struct *gyro_accel)
{
    Int_MPU6050_Read_Accel(&(gyro_accel->accel));
    Int_MPU6050_Read_Gyro(&(gyro_accel->gyro));

    // 使用校准值
    gyro_accel->gyro.x -= gyro_acc_bias.gyro.x;
    gyro_accel->gyro.y -= gyro_acc_bias.gyro.y;
    gyro_accel->gyro.z -= gyro_acc_bias.gyro.z;

    gyro_accel->accel.x -= gyro_acc_bias.accel.x;
    gyro_accel->accel.y -= gyro_acc_bias.accel.y;
    gyro_accel->accel.z -= gyro_acc_bias.accel.z;
}

void Int_MPU6050_Calibration(void)
{
    // 1. 判断当前飞机是否停稳
    // 连续100次 角速度的值和上一次的测量没有小的浮空
    uint8_t count = 0;
    Gyro_Struct last_gyro;
    Gyro_Struct cur_gyro;
    Int_MPU6050_Read_Gyro(&last_gyro);
    HAL_Delay(2);
    while (count < 100)
    {
        Int_MPU6050_Read_Gyro(&cur_gyro);
        if (ABS(cur_gyro.x - last_gyro.x) < 100 && ABS(cur_gyro.y - last_gyro.y) < 100 && ABS(cur_gyro.z - last_gyro.z) < 100)
        {
            count++;
        }
        else
        {
            // 如果中间发送不稳定 => 重新校准
            count = 0;
        }
        // C语言结构体=结构体  效果是让后面的结构体的属性挨个赋值给前面的结构体
        last_gyro = cur_gyro;
        // last_gyro.x = cur_gyro.x;
        // last_gyro.y = cur_gyro.y;
        // last_gyro.z = cur_gyro.z;
        HAL_Delay(2);
    }

    // 2. 读取当前的值当做偏移量 => 读200次取平均值
    GyroAccel_Struct gyro_acc_temp;
    int32_t gyro_acc_sum[6] = {0};
    for (uint8_t i = 0; i < 200; i++)
    {
        Int_MPU6050_ReadGryo_Accel(&gyro_acc_temp);
        gyro_acc_sum[0] += gyro_acc_temp.gyro.x;
        gyro_acc_sum[1] += gyro_acc_temp.gyro.y;
        gyro_acc_sum[2] += gyro_acc_temp.gyro.z;
        gyro_acc_sum[3] += gyro_acc_temp.accel.x;
        gyro_acc_sum[4] += gyro_acc_temp.accel.y;
        gyro_acc_sum[5] += gyro_acc_temp.accel.z - 8192;
        HAL_Delay(2);
    }
    gyro_acc_bias.gyro.x = gyro_acc_sum[0] / 200;
    gyro_acc_bias.gyro.y = gyro_acc_sum[1] / 200;
    gyro_acc_bias.gyro.z = gyro_acc_sum[2] / 200;
    gyro_acc_bias.accel.x = gyro_acc_sum[3] / 200;
    gyro_acc_bias.accel.y = gyro_acc_sum[4] / 200;
    gyro_acc_bias.accel.z = gyro_acc_sum[5] / 200;
}

// void Int_MPU6050_Convert(GyroAccel_Struct *gyro_accel)
// {
//     // 1. 转换
//     // 陀螺仪
//     // gyro_accel->gyro.x = (float)gyro_accel->gyro.x / 16.4; // 16.4 LSB/dps
//     // gyro_accel->gyro.y = (float)gyro_accel->gyro.y / 16.4; // 16.4 LSB/dps
//     // gyro_accel->gyro.z = (float)gyro_accel->gyro.z / 16.4; // 16.4 LSB/dps
//     // 加速度
//     gyro_accel->accel.x = (float)gyro_accel->accel.x / 16384.0; // 16384 LSB/g
//     gyro_accel->accel.y = (float)gyro_accel->accel.y / 16384.0; // 16384 LSB/g
//     gyro_accel->accel.z = (float)gyro_accel->accel.z / 16384.0; // 16384 LSB/g
// }

#ifndef __INF_MPU6050_H__
#define __INF_MPU6050_H__

#include "Driver_I2C2.h"
#include "Com_Types.h"
#include "FreeRTOS.h"
#include "task.h"
#define my_abs(x)            ((x) < 0 ? -(x) : (x))

// MPU6050地址定义 
#define MPU6050_ADDR_W       (0x68 << 1)
#define MPU6050_ADDR_R       ((0x68 << 1) + 1)

// DMP相关定义
#define DMP_CODE_SIZE        3062    // DMP固件大小
#define MAX_PACKET_LENGTH    42      // FIFO最大数据包长度
#define DMP_SAMPLE_RATE      200     // DMP采样率

// 寄存器定义保持不变
#define MPU_SELF_TESTX_REG   0X0D
#define MPU_SELF_TESTY_REG   0X0E
#define MPU_SELF_TESTZ_REG   0X0F
#define MPU_SELF_TESTA_REG   0X10
#define MPU_SAMPLE_RATE_REG  0X19
#define MPU_CFG_REG          0X1A
#define MPU_GYRO_CFG_REG     0X1B
#define MPU_ACCEL_CFG_REG    0X1C
#define MPU_MOTION_DET_REG   0X1F
#define MPU_FIFO_EN_REG      0X23
#define MPU_I2CMST_CTRL_REG  0X24
#define MPU_I2CSLV0_ADDR_REG 0X25
#define MPU_I2CSLV0_REG      0X26
#define MPU_I2CSLV0_CTRL_REG 0X27
#define MPU_I2CSLV1_ADDR_REG 0X28
#define MPU_I2CSLV1_REG      0X29
#define MPU_I2CSLV1_CTRL_REG 0X2A
#define MPU_I2CSLV2_ADDR_REG 0X2B
#define MPU_I2CSLV2_REG      0X2C
#define MPU_I2CSLV2_CTRL_REG 0X2D
#define MPU_I2CSLV3_ADDR_REG 0X2E
#define MPU_I2CSLV3_REG      0X2F
#define MPU_I2CSLV3_CTRL_REG 0X30
#define MPU_I2CSLV4_ADDR_REG 0X31
#define MPU_I2CSLV4_REG      0X32
#define MPU_I2CSLV4_DO_REG   0X33
#define MPU_I2CSLV4_CTRL_REG 0X34
#define MPU_I2CSLV4_DI_REG   0X35
#define MPU_I2CMST_STA_REG   0X36
#define MPU_INTBP_CFG_REG    0X37
#define MPU_INT_EN_REG       0X38
#define MPU_INT_STA_REG      0X3A
#define MPU_ACCEL_XOUTH_REG  0X3B
#define MPU_ACCEL_XOUTL_REG  0X3C
#define MPU_ACCEL_YOUTH_REG  0X3D
#define MPU_ACCEL_YOUTL_REG  0X3E
#define MPU_ACCEL_ZOUTH_REG  0X3F
#define MPU_ACCEL_ZOUTL_REG  0X40
#define MPU_TEMP_OUTH_REG    0X41
#define MPU_TEMP_OUTL_REG    0X42
#define MPU_GYRO_XOUTH_REG   0X43
#define MPU_GYRO_XOUTL_REG   0X44
#define MPU_GYRO_YOUTH_REG   0X45
#define MPU_GYRO_YOUTL_REG   0X46
#define MPU_GYRO_ZOUTH_REG   0X47
#define MPU_GYRO_ZOUTL_REG   0X48
#define MPU_I2CSLV0_DO_REG   0X63
#define MPU_I2CSLV1_DO_REG   0X64
#define MPU_I2CSLV2_DO_REG   0X65
#define MPU_I2CSLV3_DO_REG   0X66
#define MPU_I2CMST_DELAY_REG 0X67
#define MPU_SIGPATH_RST_REG  0X68
#define MPU_MDETECT_CTRL_REG 0X69
#define MPU_USER_CTRL_REG    0X6A
#define MPU_PWR_MGMT1_REG    0X6B
#define MPU_PWR_MGMT2_REG    0X6C
#define MPU_FIFO_CNTH_REG    0X72
#define MPU_FIFO_CNTL_REG    0X73
#define MPU_FIFO_RW_REG      0X74
#define MPU_DEVICE_ID_REG    0X75

// DMP相关寄存器
#define MPU_DMP_CFG1         0x70
#define MPU_DMP_CFG2         0x71
#define MPU_BANK_SEL         0x6D
#define MPU_MEM_START_ADDR   0x6E
#define MPU_MEM_R_W          0x6F


#define ABS(a) (a > 0 ? a : -(a))
// 原有函数声明保持不变
void Int_MPU6050_Init(void);
void Int_MPU6050_ReadGryo_Accel(GyroAccel_Struct *gryo_accel);
void Int_MPU6050_Calibration(void);
// void Int_MPU6050_Convert(GyroAccel_Struct *gyro_accel);
#endif /* __INF_MPU6050_H__ */

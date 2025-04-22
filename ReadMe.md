```\
在Com_Config.h中设置服务器IP和Port（在keil中不显示头文件）
App_Location.c第118行 生产环境请注释
串口打印调试信息在Com_Debug.h中第9行
```

# 需求分析

本方案设计基于STM32F103C8T6主控的可穿戴设备，集成MPU6050六轴传感器（加速度>2.5g+角度>70°持续3秒触发）与DS3553计步器，通过三阶段检测算法（动态阈值+步数骤降验证）实现老人跌倒识别，误报率＜5%。采用双模通信架构：NB-IoT模块发送含GPS定位（AT6558R，±2.5m）的报警短信至紧急联系人；LoRa模块实现1公里内本地中继传输，保障地下室等弱信号场景通信。设备配备0.96寸OLED屏动态显示步数、电量及报警状态（60Hz刷新）

# day 01

1. 软件方式I2C与MP66050获取测量模拟值,**SCL:PB15 , SDA:PB14 , AD0:GND**

​		MPU6050地址:0x68(7位,不含最后一位)

2. 修改MPU6050的量成+/-4g

   对于MPU6050，当设置加速度计量程为±4g时，分辨率（LSB/g）的计算如下：

   1. MPU6050的ADC是16位的，输出范围是-32768 ~ +32767
   2. 当量程为±4g时，总范围是8g
   3. 因此分辨率计算为：65536/8 = 8192 LSB/g

   ```c
    Int_MPU6050_Write_Byte(MPU_ACCEL_CFG_REG, 0X08); //+/-4g
    // Int_MPU6050_Write_Byte(MPU_ACCEL_CFG_REG, 0X00); //+/-2g
   ```

3. 修改初校准

   ```c
   void Int_MPU6050_Calibration(void)
   {
       // ...existing code...
       
       for (uint8_t i = 0; i < 200; i++)
       {
           Int_MPU6050_ReadGryo_Accel(&gyro_acc_temp);
           gyro_acc_sum[0] += gyro_acc_temp.gyro.x;
           gyro_acc_sum[1] += gyro_acc_temp.gyro.y;
           gyro_acc_sum[2] += gyro_acc_temp.gyro.z;
           gyro_acc_sum[3] += gyro_acc_temp.accel.x;
           gyro_acc_sum[4] += gyro_acc_temp.accel.y;
           gyro_acc_sum[5] += gyro_acc_temp.accel.z - 8192;  // 修改为±4g下的1g值
           HAL_Delay(2);
       }
   ```

 # day02

1. 创建应用层app_detect_fall.h/c文件,实现摔到检测函数**bool app_fall_detect(void);**

2. 在 app_fall_detect(void) 函数中调用Com_IMU.h 中的函数计算Z轴的加速度和欧拉角

3. 状态机方式检测是否跌倒

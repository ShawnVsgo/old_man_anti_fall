#ifndef __DRIVER_I2C2_H__
#define __DRIVER_I2C2_H__

#include "gpio.h"
#include "Com_Tools.h"


#define SCL_HIGH HAL_GPIO_WritePin(I2C3_SCL_GPIO_Port,I2C3_SCL_Pin,GPIO_PIN_SET) // SCL引脚输出高电平
#define SCL_LOW HAL_GPIO_WritePin(I2C3_SCL_GPIO_Port,I2C3_SCL_Pin,GPIO_PIN_RESET) // SCL引脚输出低电平

#define SDA_HIGH HAL_GPIO_WritePin(I2C3_SDA_GPIO_Port,I2C3_SDA_Pin,GPIO_PIN_SET) // SDA引脚输出高电平
#define SDA_LOW HAL_GPIO_WritePin(I2C3_SDA_GPIO_Port,I2C3_SDA_Pin,GPIO_PIN_RESET) // SDA引脚输出低电平

#define I2C2_DELAY Com_Delay_us(10);     // 每次改变引脚状态之后的延时

#define SDA_READ HAL_GPIO_ReadPin(I2C3_SDA_GPIO_Port,I2C3_SDA_Pin) // 读取SDA引脚状态

/**
 * @brief I2C2初始化
 * 
 */
void Driver_I2C2_Init(void);

/**
 * @brief 发送起始信号
 * 
 */
void Driver_I2C2_Start(void);

/**
 * @brief 发送停止信号
 * 
 */
void Driver_I2C2_Stop(void);

/**
 * @brief 写入一个字节
 * 
 * @param byte 要写入的字节
 */
void Driver_I2C2_SendByte(uint8_t byte);

/**
 * @brief 接收应答信号
 * 
 * @return uint8_t 0表示ACK，1表示NACK
 */
uint8_t Driver_I2C2_ReceiveACK(void);

/**
 * @brief 读取一个字节
 *
 * @return uint8_t 读取到的字节
 */
uint8_t Driver_I2C2_ReceiveByte(void);


/**
 * @brief 发送应答信号
 *
 * @param ack 0表示ACK，1表示NACK
 */
void Driver_I2C2_SendACK(uint8_t ack);



#endif /* __DRIVER_I2C2_H__ */

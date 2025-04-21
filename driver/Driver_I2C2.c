#include "Driver_I2C2.h"

/**
 * @brief I2C2初始化
 * 
 */
void Driver_I2C2_Init(void)
{
    // 使能GPIOB时钟
    // RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    // 设置GPIOB的PB10和PB11为通用开漏输出模式，最大输出速度50MHZ，CNF11为01，MODE11为11；CNF10为01，MODE10为11
    // GPIOB->CRH |= (GPIO_CRH_MODE10 | GPIO_CRH_MODE11);  // 设置最大输出速度为50MHz
    // GPIOB->CRH &= ~(GPIO_CRH_CNF10_1 | GPIO_CRH_CNF11_1);    // 设置为通用开漏输出模式
    // GPIOB->CRH |= (GPIO_CRH_CNF10_0 | GPIO_CRH_CNF11_0);    
}

/**
 * @brief 发送起始信号
 * 
 */
void Driver_I2C2_Start(void)
{
    // 拉高SCL线
    SCL_HIGH;
    I2C2_DELAY;

    // 拉高SDA线 SDA产生下降沿作为起始信号
    SDA_HIGH;
    I2C2_DELAY;
    // 拉低SDA线
    SDA_LOW;
    I2C2_DELAY;

    // 拉低SCL线
    SCL_LOW;
    I2C2_DELAY;
}

/**
 * @brief 发送停止信号
 * 
 */
void Driver_I2C2_Stop(void)
{
    // 拉低SDA线
    SDA_LOW;
    I2C2_DELAY;

    // 拉高SCL线
    SCL_HIGH;
    I2C2_DELAY;

    // 拉高SDA线 SDA产生上升沿作为停止信号
    SDA_HIGH;
    I2C2_DELAY;
}

/**
 * @brief 写入一个字节
 * 
 * @param byte 要写入的字节
 */
void Driver_I2C2_SendByte(uint8_t byte)
{
    // 发送8位数据
    for (uint8_t i = 0; i < 8; i++)
    {
        // 准备发送数据, 先判断最高位是否为1, SCL低电平时, SDA上的数据才有效
        if (byte & 0x80)
        {
            SDA_HIGH;
        }
        else
        {
            SDA_LOW;
        }
        I2C2_DELAY;
        
        // 拉高SCL线
        SCL_HIGH;
        I2C2_DELAY;
        
        // 拉低SCL线
        SCL_LOW;
        I2C2_DELAY;

        // 左移一位
        byte <<= 1;
    }
}

/**
 * @brief 接收应答信号
 * 
 * @return uint8_t 0表示ACK，1表示NACK
 */
uint8_t Driver_I2C2_ReceiveACK(void)
{
    // 准备接收数据, 释放数据总线
    SDA_HIGH;
    I2C2_DELAY;

    // 拉高SCL线
    SCL_HIGH;
    I2C2_DELAY;

    // 读取SDA线状态
    uint8_t ack = SDA_READ;

    // 拉低SCL线
    SCL_LOW;
    I2C2_DELAY;

    return ack;
}

/**
 * @brief 读取一个字节
 *
 * @return uint8_t 读取到的字节
 */
uint8_t Driver_I2C2_ReceiveByte(void)
{
    uint8_t byte = 0;

    // 接收8位数据
    for (uint8_t i = 0; i < 8; i++)
    {
        // 准备接收数据, 释放数据总线
        SDA_HIGH;
        I2C2_DELAY;

        // 拉高SCL线
        SCL_HIGH;
        I2C2_DELAY;

        // 读取SDA线状态
        byte <<= 1;
        if (SDA_READ)
        {
            byte |= 0x01;
        }
    
        // 拉低SCL线
        SCL_LOW;
        I2C2_DELAY;
    }

    return byte;
}


/**
 * @brief 发送应答信号
 *
 * @param ack 0表示ACK，1表示NACK
 */
void Driver_I2C2_SendACK(uint8_t ack)
{
    // 发送应答信号
    if (ack)  // NACK
    {
        SDA_HIGH;
    }
    else  // ACK
    {
        SDA_LOW;
    }
    I2C2_DELAY;

    // 拉高SCL线
    SCL_HIGH;
    I2C2_DELAY;

    // 拉低SCL线
    SCL_LOW;
    I2C2_DELAY;
}

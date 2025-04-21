#ifndef __INT_DS3553_H__
#define __INT_DS3553_H__

#include "main.h"
#include "i2c.h"
#include "Com_Debug.h"
#define DS3553_ADDR 0X27
#define DS3553_ADDR_W 0X4E
#define DS3553_ADDR_R 0X4F

#define DS3553_REG_CHIP_ID 0x01
#define DS3553_REG_USER_SET 0XC3
#define DS3553_REG_STEP_CNT_L 0xC4 
#define DS3553_REG_STEP_CNT_M 0xC5 
#define DS3553_REG_STEP_CNT_H 0xC6 


#define DS3553_CS_H HAL_GPIO_WritePin(DS3553_CS_GPIO_Port, DS3553_CS_Pin, GPIO_PIN_SET)
#define DS3553_CS_L HAL_GPIO_WritePin(DS3553_CS_GPIO_Port, DS3553_CS_Pin, GPIO_PIN_RESET)
void Int_DS3553_Init(void);

void Int_DS3553_Get_Step(uint32_t *step);

#endif /* __INT_DS3553_H__ */

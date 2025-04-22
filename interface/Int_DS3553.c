#include "Int_DS3553.h"
static void Int_DS3553_ReadReg(uint8_t reg_addr, uint8_t *reg_data)
{
	DS3553_CS_L;
	vTaskDelay(5);
	HAL_I2C_Mem_Read(&hi2c1, DS3553_ADDR_R, reg_addr, I2C_MEMADD_SIZE_8BIT, reg_data, 1, 1000);
	DS3553_CS_H;
	vTaskDelay(12);
	
}

static void Int_DS3553_WriteReg(uint8_t reg_addr, uint8_t reg_data)
{
	DS3553_CS_L;
	vTaskDelay(5);
	HAL_I2C_Mem_Write(&hi2c1, DS3553_ADDR_W, reg_addr, I2C_MEMADD_SIZE_8BIT, &reg_data, 1, 1000);
	DS3553_CS_H;
	vTaskDelay(12);
}

void Int_DS3553_Init(void)
{
	//初始化I2C
	//MX_I2C1_Init();
	//Int_DS3553_Get_ID();
	uint8_t DS3553_ID = 0XFF;
	Int_DS3553_ReadReg(DS3553_REG_CHIP_ID, &DS3553_ID);
	debug_println("DS3553 ID: %#x", DS3553_ID);
	uint8_t USER_SET_REGISTER_DEFAULT_VALUE = 0X18;
	//配置 计步算法 关闭计步脉冲 
	USER_SET_REGISTER_DEFAULT_VALUE |=(0x01<<1);
	USER_SET_REGISTER_DEFAULT_VALUE &=~(0x01<<4);
	Int_DS3553_WriteReg(DS3553_REG_USER_SET, USER_SET_REGISTER_DEFAULT_VALUE);
}

void Int_DS3553_Get_Step(uint32_t *step)
{
	uint8_t step_cnt_l = 0;
	uint8_t step_cnt_m = 0;
	uint8_t step_cnt_h = 0;
	Int_DS3553_ReadReg(DS3553_REG_STEP_CNT_L, &step_cnt_l);
	Int_DS3553_ReadReg(DS3553_REG_STEP_CNT_M, &step_cnt_m);
	Int_DS3553_ReadReg(DS3553_REG_STEP_CNT_H, &step_cnt_h);
	*step = step_cnt_l | (step_cnt_m << 8) | (step_cnt_h << 16);
}



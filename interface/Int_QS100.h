#ifndef __INT_QS100_H__
#define __INT_QS100_H__
#include "usart.h"
#include "Com_Debug.h"
#include "Com_Config.h"
#include "FreeRTOS.h"
#include "task.h"
void Int_QS100_Init(void);
void Int_QS100_Enter_Lower_Power(void);
void Int_QS100_Exit_Lower_Power(void);
CommmonStatus Int_QS100_Send_Data(uint8_t *data,uint16_t len);

#endif /* __INT_QS100_H__ */

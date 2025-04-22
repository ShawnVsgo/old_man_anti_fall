#ifndef __INT_AT6558R_H__
#define __INT_AT6558R_H__
#include "usart.h"
#include "Com_Debug.h"
#include "FreeRTOS.h"
#include "task.h"
#define GPS_BUFF_MAX_LEN 512
#define GPS_FULL_BUFF_MAX_LEN 1024
extern uint8_t gps_full_buff[GPS_FULL_BUFF_MAX_LEN];
extern uint16_t gps_full_buff_len;
void Int_AT6558R_Init(void);
void Int_AT6558R_Enter_Low_Power(void);
void Int_AT6558R_EXIT_Low_Power(void);
void Int_AT6558R_Read_GPS(void);

#endif /*__INT_AT6558R_H__*/

#ifndef __APP_LOCATION_H__
#define __APP_LOCATION_H__
#include "Int_Lora.h"
#include "rtc.h"
#include "Int_AT6558R.h"
#include "Int_DS3553.h"
#include "Com_Config.h"
#include "Com_Tools.h"
#include "cJSON.h"
#include "Int_QS100.h" //IOTÎïÁ¬Íø·¢ËÍÐ¾Æ¬
void App_Location_Get_GPS_Data(void);

void App_Location_Get_Step_Count(void);

void App_Location_Data_to_JSON(void);

void App_Location_Send_Data(void);
void APP_Enter_Low_Power(void);
void App_Exit_Low_Power(void);
#endif // __APP_LOCATION_H__

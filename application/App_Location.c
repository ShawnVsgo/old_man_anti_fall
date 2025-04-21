#include "App_Location.h"

Upload_Struct upload_data;
void App_Location_Get_GPS_Data(void)
{
    char *gnrmcStart = NULL;

    uint8_t retry_count = 0;
    const uint8_t max_retry = 5;
     
     char time[7];
     char date[7];
     double lat, lon;
     uint8_t lat_dir, lon_dir;

    Int_AT6558R_Init();

   
   
    while (retry_count < max_retry)
    {
        Int_AT6558R_Read_GPS();
        if (gps_full_buff_len > 0)
        {
            uint8_t tmp;
            //$GNRMC,013050.000,A,3106.67898,N,12113.52954,E,5.19,77.74,160125,,,A,V*31
            //$GNVTG,77.74,T,,M,5.19,N,9.61,K,A*13
            //$GNZDA,013050.000,16,01,2025,00,00*4C
            //$GPTXT,01,01,01,ANTENNA OPEN*25
            // gnrmcStart = strstr((char *)gps_full_buff, "$GNRMC");
            gnrmcStart = "$GNRMC,203050.000,A,3106.67898,N,12113.52954,E,5.19,77.74,310125,,,A,V*31";
            // debug_printf("gnrmc_stsart_pointer:[%s]", gnrmcStart);
            sscanf(gnrmcStart, "%*[^AV]%c", &tmp);
            if (tmp == 'A') // 数据有效
            {
                debug_println("获取到的定位信息数据是有效的");
                break;
            }
            else
            {
                debug_println("GPS location point is Invalid");
                retry_count++;
            }
        }

       
    }

   
    // 获取到有效的定位数据 => 解析定位数据
    // $GNRMC,%6c     %*6c,   %lf    ,%c, %lf        ,%c,%*lf,%*lf ,%6c
    //"$GNRMC,013050.000,A,3106.67898,N , 12113.52954,E ,5.19,77.74,160125,,,A,V*31"
    sscanf(gnrmcStart, "$GNRMC,%6c%*6c,%lf,%c,%lf,%c,%*f,%*f,%6c",time, &lat, (char *)&lat_dir, &lon, (char *)&lon_dir, date);
    debug_println("time:%s,lat:%lf,lat_dir:%c,lon:%lf,lon_dir:%c,data:%s", time, lat, lat_dir, lon, lon_dir, date);

    sprintf((char *)upload_data.dateTime,"20%c%c-%c%c-%c%c %c%c:%c%c:%c%c",
                            date[4],
                            date[5],
                            date[2],
                            date[3],
                            date[0],
                            date[1],
                            time[0],
                            time[1],
                            time[2],
                            time[3],
                            time[4],
                            time[5]
    );

   Com_UTC_to_Bj_Time(upload_data.dateTime,upload_data.dateTime);

   //解经纬度的信息
   // 转换经纬度
  // 3106.678980 => 31° 06.678980′ N = 31 + 06.678980/60 = 31.111316°
  upload_data.lat = (uint8_t)(lat / 100) + (lat - ((uint8_t)(lat / 100)) * 100) / 60;
  upload_data.lon = (uint8_t)(lon / 100) + (lon - ((uint8_t)(lon / 100)) * 100) / 60;

  upload_data.latDir[0] = lat_dir;
  upload_data.lonDir[0] = lon_dir;
}

void App_Location_Get_Step_Count(void)
{
    Int_DS3553_Init();
    uint32_t step_count = 0;
    //读取运动步数
    Int_DS3553_Get_Step(&step_count);
    upload_data.stepCount = step_count;
    debug_println("应用层获取步数%d",upload_data.stepCount);

}

void App_Location_Data_to_JSON(void)
{
    //获取唯一标识符号
    sprintf((char *)upload_data.uuid,"%08X%08X%08X", HAL_GetUIDw2(), HAL_GetUIDw1(), HAL_GetUIDw0());

    cJSON *json_data = cJSON_CreateObject();
    cJSON_AddStringToObject(json_data, "uuid", (char *)upload_data.uuid);
    cJSON_AddNumberToObject(json_data, "lat", upload_data.lat);
    cJSON_AddNumberToObject(json_data, "lon", upload_data.lon);
    cJSON_AddStringToObject(json_data, "latDir", (char *)upload_data.latDir);
    cJSON_AddStringToObject(json_data, "lonDir", (char *)upload_data.lonDir);
    cJSON_AddStringToObject(json_data, "dateTime", (char *)upload_data.dateTime);
    cJSON_AddNumberToObject(json_data, "stepCount", upload_data.stepCount);

    // char *json_data_str = cJSON_Print(json_data);
    char *json_data_str = cJSON_PrintUnformatted(json_data);
    upload_data.dataLen = strlen(json_data_str);
    memcpy(upload_data.data,json_data_str,upload_data.dataLen);
    debug_println("json_str:%s", upload_data.data);

    cJSON_Delete(json_data);
    cJSON_free(json_data_str);
}

void App_Location_Send_Data(void)
{
    Int_QS100_Init();
    
    CommmonStatus status = Int_QS100_Send_Data(upload_data.data,upload_data.dataLen);
    status = COMMON_ERROR;//生产环境请注释掉
    if(status == COMMON_OK)
    {
        debug_println("物联网QS100发送数据成功");
    }
    else
    {
        debug_println("物联网QS100发送失败");
        debug_println("使用LORA发送......");
        Int_Lora_Init();
        //lora每次最大只能发送256个字节
        //计算需要发送的次数
        uint16_t times =upload_data.dataLen/255;
        
        for (uint16_t i = 0; i < times; i++)
        {
            Int_Lora_Send_Data(&upload_data.data[i*255],255);
            upload_data.dataLen -= 255; 
        }
        //发送剩余的最后一次
        if (upload_data.dataLen > 0)
        {
            Int_Lora_Send_Data(upload_data.data,upload_data.dataLen);
        }
        
        
        debug_println("lora send cplexed");
    }

}

void APP_Enter_Low_Power(void)
{
    Int_AT6558R_Enter_Low_Power();//GPS进入低功耗
    Int_QS100_Enter_Lower_Power();//IOT物联网模块进入低功耗

    // RTC_TimeTypeDef rtc_time;
    // RTC_AlarmTypeDef alarm;

    // HAL_RTC_GetTime(&hrtc,&rtc_time,RTC_FORMAT_BIN);

    // alarm.AlarmTime.Hours = rtc_time.Hours;
    // alarm.AlarmTime.Minutes = rtc_time.Minutes;
    // alarm.AlarmTime.Seconds = rtc_time.Seconds+20;

    // HAL_RTC_SetAlarm(&hrtc,&alarm,RTC_FORMAT_BIN);
    // __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    // __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);

    // HAL_Delay(100);

    // HAL_PWR_EnterSTANDBYMode();
    
}

void App_Exit_Low_Power()
{
    Int_AT6558R_EXIT_Low_Power();//退出GPS的低功耗
    Int_QS100_Exit_Lower_Power();
}

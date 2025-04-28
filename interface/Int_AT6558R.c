#include "Int_AT6558R.h"
uint8_t gps_buff[GPS_BUFF_MAX_LEN];
uint16_t gps_buff_len;

uint8_t gps_full_buff[GPS_FULL_BUFF_MAX_LEN];
uint16_t gps_full_buff_len;
//定位频率 1s/次
#define AT6558R_LOCATION_FREQ "PCAS02,1000"
//工作模式3 GPS+北斗定位
#define AT6558R_LOCATION_MODE "PCAS04,3"
//查询定位模块的固件信息
#define AT6558R_LOCATION_VERSION "PCAS06,0"
//发送命令缓冲区
uint8_t send_cmd[64];
/**
 * 通信字符命令
 * @param cmd 标准字符串 '\0'结尾的
 */
static void Int_AT6558R_Send_CMD(const char * cmd)
{
    memset(send_cmd,0,sizeof(send_cmd));
    //计算 校验和 cs
    uint8_t cs =cmd[0];
    for (uint8_t i = 1; cmd[i]!='\0'; i++)
    {
        cs^=cmd[i];
    }
    // 拼接发送的名称 => 拼接校验和的细节 8和08不一样  1A 和 1a不一样
    sprintf((char *)send_cmd,"$%s*%02X\r\n",cmd,cs);
    debug_println("send cmd is:%s",send_cmd);
    taskENTER_CRITICAL();
    HAL_UART_Transmit(&huart2,send_cmd,strlen((char*)send_cmd),1000);
    taskEXIT_CRITICAL();
    
}
void Int_AT6558R_Init(void)
{
    //初始化通讯方式
    // MX_USART2_UART_Init();
    //使能GPS模块
    // Int_AT6558R_EXIT_Low_Power();

    // 配置芯片工作模式
    //Int_AT65558R_Send_CMD(AT6558R_LOCATION_FREQ);
    //Int_AT65558R_Send_CMD(AT6558R_LOCATION_MODE);
    // Int_AT6558R_Send_CMD(AT6558R_LOCATION_VERSION);
    Int_AT6558R_Send_CMD(AT6558R_LOCATION_FREQ); //设置每1s钟获取一次定位信息
    Int_AT6558R_Send_CMD(AT6558R_LOCATION_MODE);//设置定位模式为GPS+北斗

}

void Int_AT6558R_Enter_Low_Power(void)
{
    HAL_GPIO_WritePin(GPS_EN_GPIO_Port,GPS_EN_Pin,GPIO_PIN_RESET);
}

void Int_AT6558R_EXIT_Low_Power(void)
{
    HAL_GPIO_WritePin(GPS_EN_GPIO_Port,GPS_EN_Pin,GPIO_PIN_SET);
    //给予一定的唤醒时间，跳过开机信息的输出 $GPTXT,01,01,02 保证后期数据处理的完整性
    vTaskDelay(1000);
}

/**
 * 
 * [Int_AT6558R.c:30]send cmd is:$PCAS06,0*1B

  [Int_AT6558R.c:30]send cmd is:$PCAS02,1000*2E

  [Int_AT6558R.c:30]send cmd is:$PCAS04,3*1A
 * 
 */
void Int_AT6558R_Read_GPS(void)
{
    /**
     *  $GNGGA,013050.000,3106.67898,N,12113.52954,E,1,07,3.1,-10.7,M,10.3,M,,*5A
        $GNGLL,3106.67898,N,12113.52954,E,013050.000,A,A*43
        $GNGSA,A,3,10,16,26,194,,,,,,,,,5.2,3.1,4.2,1*0C
        $GNGSA,A,3,13,23,38,,,,,,,,,,5.2,3.1,4.2,4*3F
        $GPGSV,3,1,11,03,20,308,,10,12,183,17,16,27,221,33,25,23,043,,0*61
        $GPGSV,3,2,11,26,59,238,41,28,59,010,,29,31,085,,31,52,320,,0*67
        $GPGSV,3,3,11,32,63,126,44,194,66,109,35,195,08,160,,0*54
        $BDGSV,2,1,06,07,,,37,13,32,220,34,14,15,159,,23,43,269,30,0*42
        $BDGSV,2,2,06,25,63,008,,38,20,197,35,0*78
        $GNRMC,013050.000,A,3106.67898,N,12113.52954,E,5.19,77.74,160125,,,A,V*31
        $GNVTG,77.74,T,,M,5.19,N,9.61,K,A*13
        $GNZDA,013050.000,16,01,2025,00,00*4C
        $GPTXT,01,01,01,ANTENNA OPEN*25
     */
    //  GNRMC
    memset(gps_full_buff,0,GPS_FULL_BUFF_MAX_LEN);
    gps_full_buff_len =0;

    do
    {   taskENTER_CRITICAL();
        HAL_UARTEx_ReceiveToIdle(&huart2,gps_buff,GPS_BUFF_MAX_LEN,&gps_buff_len,1000);
        taskEXIT_CRITICAL();
        memcpy(&gps_full_buff[gps_full_buff_len],gps_buff,gps_buff_len);
        gps_full_buff_len += gps_buff_len;

        //清零过度数组
        memset(gps_buff,0,GPS_BUFF_MAX_LEN);
        gps_buff_len = 0;
        
        //当接收到的信息完整的时候退出循环
        if (strstr((char *)gps_full_buff, "GGA") != NULL && strstr((char *)gps_full_buff, "TXT") != NULL)
        {
            // debug_println("full gps_info:--->[%s]",gps_full_buff);
            // 当前一次接收数据完整
            break;
        }
    } while (1);

}



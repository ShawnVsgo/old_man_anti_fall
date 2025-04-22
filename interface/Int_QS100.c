#include "Int_QS100.h"
#define IOT_BUFF_MAX_LEN 128
uint8_t iot_buff[IOT_BUFF_MAX_LEN];
uint16_t iot_buff_len;

#define IOT_FULL_BUFF_MAX_LEN 256
uint8_t iot_full_buff[IOT_FULL_BUFF_MAX_LEN];
uint16_t iot_full_buff_len;
// QS100发送数据的最大重试次数
#define QS100_SEND_MAX_RETRY_TIMES 5
//
#define SEQUENCE 1
/**
 * @brief 启动时不会应答有
 *
 */
static void Int_QS100_Reboot(void)
{
    uint8_t *cmd = "AT+RB\r\n";
    HAL_UART_Transmit(&huart3, cmd, strlen((char *)cmd), 1000);

    HAL_UART_Receive(&huart3, iot_buff, IOT_BUFF_MAX_LEN, 5000);
    iot_buff_len = strlen((char *)iot_buff);

    if (iot_buff_len > 0)
    {
        debug_println("QS100 Init end:[%s]", iot_buff);
        iot_buff_len = 0;
        memset(iot_buff, 0, IOT_BUFF_MAX_LEN);
    }
}

static void Int_QS100_SendCmd(uint8_t *cmd)
{
    HAL_UART_Transmit(&huart3, cmd, strlen((char *)cmd), 1000);

    memset(iot_full_buff, 0, IOT_FULL_BUFF_MAX_LEN);
    iot_full_buff_len = 0;
    do
    {
        HAL_UARTEx_ReceiveToIdle(&huart3, iot_buff, IOT_BUFF_MAX_LEN, &iot_buff_len, 3000);
        if (iot_buff_len > 0)
        {
            memcpy(&iot_full_buff[iot_full_buff_len], iot_buff, iot_buff_len);
            iot_full_buff_len += iot_buff_len;
            memset(iot_buff, 0, IOT_BUFF_MAX_LEN);
            iot_buff_len = 0;
        }

    } while (strstr((char *)iot_full_buff, "OK") == NULL && strstr((char *)iot_full_buff, "ERROR") == NULL);
    debug_println("when QS100 send cmd:[%s] Reply :[%s]", cmd, iot_full_buff);
}
void Int_QS100_Wkup(void)
{
    HAL_GPIO_WritePin(NB_WKUP_GPIO_Port, NB_WKUP_Pin, GPIO_PIN_SET);
    vTaskDelay(5);
    HAL_GPIO_WritePin(NB_WKUP_GPIO_Port, NB_WKUP_Pin, GPIO_PIN_RESET);
    vTaskDelay(5);
}

void Int_QS100_Init()
{
    // 初始化USART3
    //  MX_USART3_UART_Init();
    // 唤醒芯片
    //  Int_QS100_Wkup();

    // 重启设备
    Int_QS100_Reboot();
    // 打开串口回显
    Int_QS100_SendCmd("ATE1\r\n");
    // 查询软件版本信息
    Int_QS100_SendCmd("AT+CGMR\r\n");
}
void Int_QS100_Enter_Lower_Power(void)
{
    uint8_t *cmd = "AT+FASTOFF=0\r\n";
    Int_QS100_SendCmd(cmd);
}
void Int_QS100_Exit_Lower_Power(void)
{
    Int_QS100_Wkup();
}
//网络是否通常
CommmonStatus Int_QS100_GetIP(void)
{

    Int_QS100_SendCmd("AT+CGATT?\r\n");

    // 2. 判断结果 => 不能在打印完结果之后就清除缓存
    if (strstr((char *)iot_full_buff, "+CGATT:1") == NULL)
    {
        return COMMON_ERROR;
    }

    return COMMON_OK;
}
//创建客户端
CommmonStatus Int_QS100_Create_Client(uint8_t *socket)
{
    /*
    AT+NSOCR=STREAM,6,0,0

   +NSOCR:0

   OK
    */
    Int_QS100_SendCmd("AT+NSOCR=STREAM,6,0,0\r\n");
    if(strstr((char*)iot_full_buff,"OK")==NULL)
    {
        return COMMON_ERROR;
    }
    char * tmp = strstr((char*)iot_full_buff,"+NSOCR:");
    // socket接收的值是字节 => 不是数字0 => 0字节对应的ASCII码就是48
    *socket = tmp[7]-'0';
    return COMMON_OK;

}
CommmonStatus Int_QS100_Connect_Server(uint8_t socket,uint8_t *ip,uint16_t port)
{
    uint8_t cmd[64] = {0};
    sprintf((char *)cmd,"AT+NSOCO=%d,%s,%d\r\n",socket,ip,port);
    Int_QS100_SendCmd(cmd);
    // 判断连接成功和失败 => 返回值OK和ERROR
    if (strstr((char *)iot_full_buff, "OK") == NULL)
    {
        return COMMON_ERROR;
    }

    return COMMON_OK;

}
uint8_t cmd[512] ={0};
CommmonStatus Int_QS100_Send_Data_to_Server(uint8_t socket, uint8_t *pData, uint16_t len)
{
    //需要将数据转换成16进制字符串发送 
    uint16_t hex_len = len * 2 + 1;
    uint8_t hex_data[hex_len];
    memset(hex_data,0,hex_len);

    for (uint16_t i = 0; i < len; i++)
    {
        sprintf((char*)hex_data+i*2,"%02X",pData[i]);
    }
   
    sprintf((char *)cmd, "AT+NSOSD=%d,%d,%s,0x200,%d\r\n", socket, len, hex_data, SEQUENCE);
    Int_QS100_SendCmd(cmd);
    // 判断发送数据成功和失败 => 不等于对方收到数据了 => ACK机制 => 回复ACK
    memset(cmd, 0, sizeof(cmd));
    // 发送确认收到ACK的命令
    /**
     *  AT+SEQUENCE=0,1

        2

        OK
     */
    sprintf((char *)cmd, "AT+SEQUENCE=%d,%d\r\n", socket, SEQUENCE);

    // 需要循环不断发送请求 => 一直等待显示发送成功或者失败
    char tmp = 0xff;

    while (tmp != '0' && tmp != '1')
    {
        // 手动在这里加延迟
        vTaskDelay(5);
        Int_QS100_SendCmd(cmd);
        tmp = iot_full_buff[iot_full_buff_len - 9];
    }
    if (tmp == '0')
    {
        return COMMON_ERROR;
    }

    return COMMON_OK;
}
CommmonStatus Int_QS100_Close_Client(uint8_t socket_id)
{
    // 拼接命令
    uint8_t cmd[32] = {0};
    sprintf((char *)cmd, "AT+NSOCL=%d\r\n", socket_id);

    // 发送命令
    Int_QS100_SendCmd(cmd);

    // 判断连接成功和失败 => 返回值OK和ERROR
    if (strstr((char *)iot_full_buff, "OK") == NULL)
    {
        return COMMON_ERROR;
    }

    return COMMON_OK;
}
CommmonStatus Int_QS100_Send_Data(uint8_t *data, uint16_t len)
{
    // 但定义网络错误状态
    CommmonStatus status = COMMON_ERROR;
    // 1.查看网络是否通
    status = Int_QS100_GetIP();
    int8_t count =QS100_SEND_MAX_RETRY_TIMES;
    while (status == COMMON_ERROR && count > 0)
    {
        status = Int_QS100_GetIP();
        count --;
        vTaskDelay(100);
    }

    if(count == 0)
    {
        debug_println("QS100 网络连接失败");
        return COMMON_ERROR;
    }
    
    debug_println("QS100网络连接成功");

    //2.创建客户端
    count = QS100_SEND_MAX_RETRY_TIMES;
    status = COMMON_ERROR;
    uint8_t socket = 0xff;
    while (status == COMMON_ERROR && count >0)
    {
       status = Int_QS100_Create_Client(&socket);
       count--;
       vTaskDelay(100);
    }
    if (count==0)
    {
        debug_println("QS100socket客户端创建失败socket_id:%d",socket);
        return COMMON_ERROR;
    }

    debug_println("QS100socket客户端创建成功socket_id:%d",socket);
    //3.连接服务器
    count = QS100_SEND_MAX_RETRY_TIMES;
    status = COMMON_ERROR;

    while (status == COMMON_ERROR && count>0)
    {
        status = Int_QS100_Connect_Server(socket,HTTP_SERVER_IP,HTTP_SERVER_PORT);
        count --;
        vTaskDelay(100);
    }

    if (count == 0)
    {
       debug_println("QS100连接服务器失败,服务器ip[%s:%d]",HTTP_SERVER_IP,HTTP_SERVER_PORT);
       Int_QS100_Close_Client(socket);
       return COMMON_ERROR;
    }
    debug_println("QS100连接服务器成功,服务器ip[%s:%d]",HTTP_SERVER_IP,HTTP_SERVER_PORT);

   count = QS100_SEND_MAX_RETRY_TIMES;
   status = COMMON_ERROR;

   while (status == COMMON_ERROR && count > 0)
   {
     status = Int_QS100_Send_Data_to_Server(socket,data,len);
     count --;
     vTaskDelay(100);
   }
   if (count == 0)
   {
    debug_println("QS100发送数据到服务器失败.服务器ip[%s:%d]",HTTP_SERVER_IP,HTTP_SERVER_PORT);
    Int_QS100_Close_Client(socket);
    return COMMON_ERROR;
   }
   debug_println("QS100发送数据到服务器成功.服务器ip[%s:%d]",HTTP_SERVER_IP,HTTP_SERVER_PORT);

   // 6. 关闭客户端
   count = QS100_SEND_MAX_RETRY_TIMES;
   status = COMMON_ERROR;
   while (status == COMMON_ERROR && count > 0)
   {
       status = Int_QS100_Close_Client(socket);
       count--;
   }
   if (count == 0)
   {
       debug_println("QS100关闭客户端失败");
       return COMMON_ERROR;
   }
   debug_println("QS100关闭客户端成功");
   //发送不能过于频繁 1s/次
   vTaskDelay(1000);
   return COMMON_OK;
}


#include "Inf_qs100_mqtt.h"
#include <stdlib.h>
void Inf_QS100_MQTT_Init(void)
{
    Int_QS100_Exit_Lower_Power();
    Int_QS100_Init();
}
//建立MQTT客户段
void Inf_QS100_MQTT_CreatClient(void)
{
    uint8_t cmd[64] = {0};
    sprintf((char *)cmd,"AT+MQNEW=%s,%s,%d,%d\r\n",SERVER,PORT,5000,200);
    Int_QS100_SendCmd(cmd);
}
//连接MQTT服务器
void Inf_QS100_MQTT_Connect(void)
{
    uint8_t cmd[64] = {0};
    sprintf((char *)cmd,"AT+MQCON=0,4,%s,1000,1,0\r\n",CLIENT_ID);
    Int_QS100_SendCmd(cmd);
    // 添加延时等待连接完全建立
    vTaskDelay(2000 / portTICK_PERIOD_MS);
}
//订阅主题
void Inf_QS100_MQTT_Subscribe(void)
{
    uint8_t cmd[64] = {0};
    // 添加延时等待连接完全建立
    HAL_Delay(2000);
   // 修改订阅命令格式，添加引号
    sprintf((char *)cmd, "AT+MQSUB=0,%s,%d\r\n", "vsgo", 1);
    Int_QS100_SendCmd(cmd);
}
//发布主题
void Inf_QS100_MQTT_Publish(uint8_t *data,uint16_t len)
{
    uint8_t cmd[512] = {0};//这里面包含了负载的数据，所以长度要大一些（需要的话试试改成动态的大小，通过长度来分配）
    uint8_t *hex_data = (uint8_t *)malloc(len * 2 + 1); // 分配足够的内存来存储16进制字符串
  
    //将数据转换成16进制字符串发送
    for (int i = 0; i < len; i++) {
        sprintf((char *)(hex_data + i * 2), "%02X", data[i]); 
    }
    sprintf((char *)cmd, "AT+MQPUB=0,%s,%d,0,0,%d,%s\r\n",PULL_TOPIC,QoS,len, hex_data);

   
    Int_QS100_SendCmd(cmd);
  
   
  

   
    free(hex_data); // 释放内存
}

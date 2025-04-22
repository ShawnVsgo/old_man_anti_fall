#include "Int_QS100.h"
#define IOT_BUFF_MAX_LEN 128
uint8_t iot_buff[IOT_BUFF_MAX_LEN];
uint16_t iot_buff_len;

#define IOT_FULL_BUFF_MAX_LEN 256
uint8_t iot_full_buff[IOT_FULL_BUFF_MAX_LEN];
uint16_t iot_full_buff_len;
// QS100�������ݵ�������Դ���
#define QS100_SEND_MAX_RETRY_TIMES 5
//
#define SEQUENCE 1
/**
 * @brief ����ʱ����Ӧ����
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
    // ��ʼ��USART3
    //  MX_USART3_UART_Init();
    // ����оƬ
    //  Int_QS100_Wkup();

    // �����豸
    Int_QS100_Reboot();
    // �򿪴��ڻ���
    Int_QS100_SendCmd("ATE1\r\n");
    // ��ѯ����汾��Ϣ
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
//�����Ƿ�ͨ��
CommmonStatus Int_QS100_GetIP(void)
{

    Int_QS100_SendCmd("AT+CGATT?\r\n");

    // 2. �жϽ�� => �����ڴ�ӡ����֮����������
    if (strstr((char *)iot_full_buff, "+CGATT:1") == NULL)
    {
        return COMMON_ERROR;
    }

    return COMMON_OK;
}
//�����ͻ���
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
    // socket���յ�ֵ���ֽ� => ��������0 => 0�ֽڶ�Ӧ��ASCII�����48
    *socket = tmp[7]-'0';
    return COMMON_OK;

}
CommmonStatus Int_QS100_Connect_Server(uint8_t socket,uint8_t *ip,uint16_t port)
{
    uint8_t cmd[64] = {0};
    sprintf((char *)cmd,"AT+NSOCO=%d,%s,%d\r\n",socket,ip,port);
    Int_QS100_SendCmd(cmd);
    // �ж����ӳɹ���ʧ�� => ����ֵOK��ERROR
    if (strstr((char *)iot_full_buff, "OK") == NULL)
    {
        return COMMON_ERROR;
    }

    return COMMON_OK;

}
uint8_t cmd[512] ={0};
CommmonStatus Int_QS100_Send_Data_to_Server(uint8_t socket, uint8_t *pData, uint16_t len)
{
    //��Ҫ������ת����16�����ַ������� 
    uint16_t hex_len = len * 2 + 1;
    uint8_t hex_data[hex_len];
    memset(hex_data,0,hex_len);

    for (uint16_t i = 0; i < len; i++)
    {
        sprintf((char*)hex_data+i*2,"%02X",pData[i]);
    }
   
    sprintf((char *)cmd, "AT+NSOSD=%d,%d,%s,0x200,%d\r\n", socket, len, hex_data, SEQUENCE);
    Int_QS100_SendCmd(cmd);
    // �жϷ������ݳɹ���ʧ�� => �����ڶԷ��յ������� => ACK���� => �ظ�ACK
    memset(cmd, 0, sizeof(cmd));
    // ����ȷ���յ�ACK������
    /**
     *  AT+SEQUENCE=0,1

        2

        OK
     */
    sprintf((char *)cmd, "AT+SEQUENCE=%d,%d\r\n", socket, SEQUENCE);

    // ��Ҫѭ�����Ϸ������� => һֱ�ȴ���ʾ���ͳɹ�����ʧ��
    char tmp = 0xff;

    while (tmp != '0' && tmp != '1')
    {
        // �ֶ���������ӳ�
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
    // ƴ������
    uint8_t cmd[32] = {0};
    sprintf((char *)cmd, "AT+NSOCL=%d\r\n", socket_id);

    // ��������
    Int_QS100_SendCmd(cmd);

    // �ж����ӳɹ���ʧ�� => ����ֵOK��ERROR
    if (strstr((char *)iot_full_buff, "OK") == NULL)
    {
        return COMMON_ERROR;
    }

    return COMMON_OK;
}
CommmonStatus Int_QS100_Send_Data(uint8_t *data, uint16_t len)
{
    // �������������״̬
    CommmonStatus status = COMMON_ERROR;
    // 1.�鿴�����Ƿ�ͨ
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
        debug_println("QS100 ��������ʧ��");
        return COMMON_ERROR;
    }
    
    debug_println("QS100�������ӳɹ�");

    //2.�����ͻ���
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
        debug_println("QS100socket�ͻ��˴���ʧ��socket_id:%d",socket);
        return COMMON_ERROR;
    }

    debug_println("QS100socket�ͻ��˴����ɹ�socket_id:%d",socket);
    //3.���ӷ�����
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
       debug_println("QS100���ӷ�����ʧ��,������ip[%s:%d]",HTTP_SERVER_IP,HTTP_SERVER_PORT);
       Int_QS100_Close_Client(socket);
       return COMMON_ERROR;
    }
    debug_println("QS100���ӷ������ɹ�,������ip[%s:%d]",HTTP_SERVER_IP,HTTP_SERVER_PORT);

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
    debug_println("QS100�������ݵ�������ʧ��.������ip[%s:%d]",HTTP_SERVER_IP,HTTP_SERVER_PORT);
    Int_QS100_Close_Client(socket);
    return COMMON_ERROR;
   }
   debug_println("QS100�������ݵ��������ɹ�.������ip[%s:%d]",HTTP_SERVER_IP,HTTP_SERVER_PORT);

   // 6. �رտͻ���
   count = QS100_SEND_MAX_RETRY_TIMES;
   status = COMMON_ERROR;
   while (status == COMMON_ERROR && count > 0)
   {
       status = Int_QS100_Close_Client(socket);
       count--;
   }
   if (count == 0)
   {
       debug_println("QS100�رտͻ���ʧ��");
       return COMMON_ERROR;
   }
   debug_println("QS100�رտͻ��˳ɹ�");
   //���Ͳ��ܹ���Ƶ�� 1s/��
   vTaskDelay(1000);
   return COMMON_OK;
}


#include "Com_Debug.h"

void Com_Debug_Init(void)
{
    // ��ʼ����־��ӡ���
    // MX_USART1_UART_Init();
}

int fputc(int ch, FILE *file)
{
    // ��printf�����ض��� ʹ�ô���USART1���������
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
    return ch;
}

#ifndef __COM_CONFIG_H__
#define __COM_CONFIG_H__

#define HTTP_SERVER_IP "112.125.89.8"
#define HTTP_SERVER_PORT 47336

typedef enum
{
    COMMON_OK,
    COMMON_ERROR

} CommmonStatus;

typedef struct
{
    // Ψһ��ʶ�� => ����������оƬstm32ID
    uint8_t uuid[33];

    // GPS��λ��Ϣ
    double lon;        // ����
    uint8_t lonDir[2]; // ���ȷ���
    double lat;        // ά��
    uint8_t latDir[2]; // ά�ȷ���

    // ��λʱ��
    uint8_t dateTime[21]; // 2025-01-21 12:26:30

    // �Ʋ���Ϣ
    uint32_t stepCount;

    // ���������͵�����
    uint8_t data[512];
    int16_t dataLen;
} Upload_Struct;

#endif /* __COM_CONFIG_H__ */

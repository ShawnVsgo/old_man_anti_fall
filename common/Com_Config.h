#ifndef __COM_CONFIG_H__
#define __COM_CONFIG_H__

#define HTTP_SERVER_IP "112.125.89.8"
#define HTTP_SERVER_PORT 46227

typedef enum
{
    COMMON_OK,
    COMMON_ERROR

} CommmonStatus;

typedef struct
{
    // 唯一标识符 => 开发板主控芯片stm32ID
    uint8_t uuid[33];

    // GPS定位信息
    double lon;        // 经度
    uint8_t lonDir[2]; // 经度方向
    double lat;        // 维度
    uint8_t latDir[2]; // 维度方向

    // 定位时间
    uint8_t dateTime[21]; // 2025-01-21 12:26:30

    // 计步信息
    uint32_t stepCount;

    // 最终整理发送的数据
    uint8_t data[512];
    int16_t dataLen;
} Upload_Struct;

#endif /* __COM_CONFIG_H__ */

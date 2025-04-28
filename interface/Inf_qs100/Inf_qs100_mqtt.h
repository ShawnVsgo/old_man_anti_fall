#ifndef __INF_QS100_MQTT_H__
#define __INF_QS100_MQTT_H__

#include "Int_QS100.h"

#define SERVER "broker.emqx.io" // Replace with your MQTT broker address
#define PORT "1883" // Replace with your MQTT broker port
#define CLIENT_ID "Inf_QS100" // Replace with your client ID
#define PULL_TOPIC "vsgo" // Replace with your topic
#define PUSH_TOPIC "vsgo" // Replace with your topic
#define QoS 0 // Quality of Service level (0, 1, or 2)
void Inf_QS100_MQTT_Init(void);
void Inf_QS100_MQTT_CreatClient(void);
void Inf_QS100_MQTT_Connect(void);
void Inf_QS100_MQTT_Subscribe(void);
void Inf_QS100_MQTT_Publish(uint8_t *data,uint16_t len);


#endif /* __INF_QS100_MQTT_H__ */

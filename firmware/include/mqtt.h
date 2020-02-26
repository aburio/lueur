#ifndef __MQTT_INCLUDED__
#define __MQTT_INCLUDED__

#include "data.h"
#include "PubSubClient.h"

void MqttInit(const char* mqtt_domain, const uint16_t mqtt_port, const String mqtt_id, WiFiClientSecure& tls_client);
void MqttConnect();
void MqttConnectionCallBack(); // Weak function
void MqttProcess();
void MqttSubscribe(String topic);
void MqttSend(String topic, String msg);
void MqttReceivedCallBack(String topic, byte* payload, uint32_t length); // Weak function
void MqttDisconnectedCallBack(); // Weak function

#endif

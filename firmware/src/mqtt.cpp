#include "mqtt.h"

PubSubClient *mqtt_client;
String mqtt_device_id;

//-----------------
// Mqtt functions
//-----------------
void MqttInit(const char* mqtt_domain, const uint16_t mqtt_port, const String mqtt_id, WiFiClientSecure& tls_client) {
  if (mqtt_client != NULL) {
    delete mqtt_client;
  }
  mqtt_client = new PubSubClient(mqtt_domain, mqtt_port, MqttReceivedCallBack, tls_client);
  mqtt_device_id = mqtt_id;
  MqttConnect();
}

void MqttConnect() {
  if (!mqtt_client->connected() && WiFi.status() == WL_CONNECTED) {
    Serial.println("waiting for connection");
    if (mqtt_client->connect(mqtt_device_id.c_str())) {
      MqttConnectionCallBack();
    }
  }
}

__attribute__ ((weak)) void MqttConnectionCallBack() {
  // To be defined by user
}


void MqttProcess() {
  if (mqtt_client != NULL) {
    if (!mqtt_client->loop()) {
      MqttDisconnectedCallBack();
      MqttConnect();
    }
  }
}

void MqttSubscribe(String topic) {
  if (mqtt_client != NULL) {
    mqtt_client->subscribe(topic.c_str());
  }
}

void MqttSend(String topic, String msg) {
  if (mqtt_client != NULL) {
    mqtt_client->publish(topic.c_str(), msg.c_str());
  }
}

__attribute__ ((weak)) void MqttReceivedCallBack(String topic, byte * payload, uint32_t length) {
  // To be defined by user
}

__attribute__ ((weak)) void MqttDisconnectedCallBack() {
  // To be defined by user
}

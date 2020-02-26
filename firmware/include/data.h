#ifndef __DATA_INCLUDED__
#define __DATA_INCLUDED__

#include "Arduino.h"
#include "ESP8266WiFi.h"

//-----------------
// Define
//-----------------
//MQTT
#define MQTT_KEEPALIVE 60
#define ACKNOWLEDGE_TIMEOUT 500;

// BTN
#define BUTTON_PIN 12
#define BUTTON_DEBOUNCE_PERIOD 15u
#define BUTTON_DOUBLE_CLICK_PERIOD 500ul
#define BUTTON_LONG_PRESS_PERIOD 2000ul
#define BUTTON_RESET_PERIOD 5000ul

// LED
#define LED_PIN 14
#define LED_BRIGHT 180
#define LED_CONFIG_COLOR 0xFFFFFFu
#define LED_UPDATE_COLOR 0x00FF00u
#define LED_UPDATE_PERIOD 250u
#define LED_ERROR_COLOR 0xFFF600u
#define LED_ERROR_PERIOD 1000u

extern const String kBuildCommit;
extern const String kName;
extern const String kId;
extern const String kDeviceId;

/* Soft AP network parameters */
extern const IPAddress kApIp;
extern const IPAddress kApMask;
extern const String kCaptivePortalDomain;

extern const String kUpdateServer;

extern const char* kMqttDomain;
extern const uint16_t kMqttPort;

extern const String kBaseTopic;
extern const String kDeviceTopic;
extern const String kWelcomeTopic;
extern const String kBuildTopic;
extern const String kDeviceStatusTopic;
extern const String kMaintenanceTopic;

extern const String kPairingFile;

#endif

#include "data.h"
#include "wifi.h"
#include "ota.h"
#include "mqtt.h"
#include "led.h"
#include "button.h"

WiFiClientSecure *tls_client;
bool wait_user = false;
bool wait_ack = false;
uint32_t ack_timeout;
String pair_topic = "null";
String pair_id = "null";

//------------------
// Setup TLS
//------------------
bool SetupTlsConnection(WiFiClientSecure* client)
{
  bool result = true;

  return  result;
}

//------------------
// Startup section
//------------------
void setup() {
  bool result;

  Serial.begin(115200);
  Serial.println(String(ESP.getChipId()));

  SPIFFS.begin();
  LedInit(LED_PIN);
  ButtonInit(BUTTON_PIN);
  WifiInit();

  tls_client = new WiFiClientSecure();

  // Load certificate file
  File cert = SPIFFS.open("/cert/cert.der", "r"); //replace cert.crt eith your uploaded file name
  if (!cert) {
    Serial.println("Failed to open cert file");
    LedBlink(0x0000FF, 500);
    result = false;
  }
  else {
    result = tls_client->loadCertificate(cert);
    if (!result) {
      Serial.println("Failed to load cert file");
    }
  }

  // Load private key file
  File private_key = SPIFFS.open("/cert/privatekey.der", "r"); //replace private eith your uploaded file name
  if (!private_key) {
    Serial.println("Failed to open private cert file");
    result = false;
  }
  else {
    result = tls_client->loadPrivateKey(private_key);
    if (!result) {
      Serial.println("Failed to load private key file");
    }
  }

  if (result) {
    MqttInit(kMqttDomain, kMqttPort, kDeviceId, *tls_client);
  }
  else {
    Serial.println("Cert shall be update to establish MQTT connection");
  }
}

//-----------------
// Main section
//-----------------
void loop() {
  ButtonProcess();
  WifiProcess();
  if (wait_ack && ack_timeout <= millis()) {
    LedBlink(LED_ERROR_COLOR, 1000 ,1);
    wait_ack = false;
  }
  yield();
}

//-----------------
// Wifi Callback section
//-----------------
void WifiCaptivePortalStartCallBack() {
  LedOn(LED_CONFIG_COLOR);
}

void WifiCaptivePortalEndCallBack() {
  LedOff();
  delay(500);
  ESP.restart();
}

void WifiWaitingConnectionCallBack() {
  LedBlink(LED_ERROR_COLOR, LED_ERROR_PERIOD);
}

void WifiProcessCallBack() {
  MqttProcess();
}

//-----------------
// Device section
//-----------------

void DeviceInboxMessage(String message) {
  if (message == "0" && wait_ack) {
    // Pair message acknowledgment
    wait_ack = false;
    LedFade((uint32_t)0xFF0000, 20, 2); // Heart beat
  }
  else if (message == pair_id) {
    // New message from pair
    MqttSend(pair_topic,"0"); // acknowledgment
    LedFadeIn((uint32_t)0xFF0000, 50); // light on
    wait_user = true;
  }
  else if (message.startsWith(kBaseTopic)) {
    // Pairing information received
    File f = SPIFFS.open(kPairingFile, "w");

    if (f) {
      pair_topic = message;
      pair_id = message.substring(message.lastIndexOf('/') + 1);
      f.println(pair_topic);
      f.close();
    }
  }
}

//-----------------
// Maintenance section
//-----------------

void MaintenanceUpdateStatus(const String update_name, OtaStatus status) {
  if (status == SUCCESS) {
    MqttSend(kDeviceStatusTopic, update_name + " updated");
  }
  else if (status == UP_TO_DATE) {
    MqttSend(kDeviceStatusTopic, update_name + " up to date");
  }
  else {
    MqttSend(kDeviceStatusTopic, update_name + " failed to update");
  }
}

void MaintenanceUpdate() {
  MqttSend(kDeviceStatusTopic, "update");
  LedRainbow(50);

  OtaStatus spiffs_status = OtaSpiffsUpdate(kUpdateServer);
  MaintenanceUpdateStatus("spiffs", spiffs_status);

  OtaStatus app_status = OtaAppUpdate(kUpdateServer);
  MaintenanceUpdateStatus("app", app_status);

  if (app_status != FAILED && spiffs_status != FAILED) {
    LedBlink(LED_UPDATE_COLOR, LED_UPDATE_PERIOD, 3);

    // Si un des composants a été mise à jour on redémarre
    if (app_status == SUCCESS || spiffs_status == SUCCESS) {
      delay(1600);
      ESP.restart();
    }
  }
  else {
    LedBlink(LED_ERROR_COLOR, LED_ERROR_PERIOD, 3);
  }
}

void MaintenanceBuildInfo() {
  MqttSend(kDeviceStatusTopic, kBuildCommit);
}

void MaintenanceTimeInfo() {
  uint32_t rtc_period = system_rtc_clock_cali_proc();
  uint32_t rtc_period_int_part = rtc_period >> 12;
  uint32_t rtc_period_dec_part = rtc_period & 0x00000FFF;
  uint32_t rtc_count = system_get_rtc_time();
  uint32_t up_time = system_get_time();
  String rst_reason = ESP.getResetReason();
  MqttSend(kDeviceStatusTopic, "Last reset reason : " + rst_reason);
  MqttSend(kDeviceStatusTopic, "Up time : " + String(up_time) + "us");
  MqttSend(kDeviceStatusTopic, "RTC period : " + String(rtc_period_int_part) + "." + String(rtc_period_dec_part) + "us");
  MqttSend(kDeviceStatusTopic, "RTC count : " + String(rtc_count));
}

void MaintenanceMemoryInfo() {
  uint8_t flash_mode = ESP.getFlashChipMode();
  uint32_t flash_speed = ESP.getFlashChipSpeed() / 1000000;
  uint32_t flash_size = ESP.getFlashChipRealSize() / 1024;

  MqttSend(kDeviceStatusTopic, "Flash size : " + String(flash_size) + " Kb");
  MqttSend(kDeviceStatusTopic, "Flash speed : " + String(flash_speed) + "MHz");

  switch (flash_mode) {
    case 0x00:
      MqttSend(kDeviceStatusTopic, "Flash mode : QIO");
      break;
    case 0x01:
      MqttSend(kDeviceStatusTopic, "Flash mode : QOUT");
      break;
    case 0x02:
      MqttSend(kDeviceStatusTopic, "Flash mode : DIO");
      break;
    case 0x03:
      MqttSend(kDeviceStatusTopic, "Flash mode : DOUT");
      break;
    default:
      MqttSend(kDeviceStatusTopic, "Flash mode : Unknow");
  }

  MqttSend(kDeviceStatusTopic, "Free program memory : " + String(ESP.getFreeSketchSpace()) + " bytes");
  MqttSend(kDeviceStatusTopic, "Free heap : " + String(ESP.getFreeHeap()) + " bytes");
}

void MaintenanceNetwork() {
  MqttSend(kDeviceStatusTopic, "SSID : " + WiFi.SSID());
  MqttSend(kDeviceStatusTopic, "RSSI : " + String(WiFi.RSSI()) + "dB");
  MqttSend(kDeviceStatusTopic, "Channel : " + String(WiFi.channel()));
  MqttSend(kDeviceStatusTopic, "MAC Address : " + WiFi.macAddress());
}

void MaintenanceInboxMessage(String message) {
  if (message == "0") {
    MaintenanceUpdate();
  }
  else if (message == "1") {
    MaintenanceBuildInfo();
  }
  else if (message == "2") {
    MaintenanceTimeInfo();
  }
  else if (message == "3") {
    MaintenanceMemoryInfo();
  }
  else if (message == "4") {
    MaintenanceNetwork();
  }
}

//-----------------
// Mqtt Callback section
//-----------------
void MqttConnectionCallBack() {
  Serial.println("Connected");
  LedOff();
  MqttSubscribe(kDeviceTopic);
  MqttSubscribe(kMaintenanceTopic);
  MqttSend(kBuildTopic, kId + "/" + kBuildCommit);

  if (!SPIFFS.exists(kPairingFile.c_str())) {
    MqttSend(kWelcomeTopic, kId);
  } else {
    File f = SPIFFS.open(kPairingFile.c_str(), "r");
    if (f) {
      pair_topic = f.readStringUntil('\n');
      pair_topic.remove(pair_topic.length() - 1);
      MqttSend(kWelcomeTopic, kId + "-" + pair_topic);
      pair_id = pair_topic.substring(pair_topic.lastIndexOf('/') + 1);
    }
  }
}

void MqttReceivedCallBack(String topic, byte* payload, uint32_t length) {
  Serial.println("Message received");
  if (length > 0) {
    char* message = new char[length];
    memcpy(message, payload, length);
    message[length] = 0;

    if (topic == kDeviceTopic) {
      DeviceInboxMessage(message);
    }
    else if (topic == kMaintenanceTopic) {
      MaintenanceInboxMessage(message);
    }

    delete message;
  }
}

void MqttDisconnectedCallBack() {
  Serial.println("Mqtt connection lost");
  LedBlink(LED_ERROR_COLOR, LED_ERROR_PERIOD);
}

//-----------------
// Button Callback section
//-----------------
void ButtonClickCallBack() {
  if (wait_user == true) {
    wait_user = false;
    LedOff();
  }
  else if (pair_id != "null") {
    // Send a message to the pair
    MqttSend(pair_topic, kId);
    wait_ack = true;
    ack_timeout = millis() + ACKNOWLEDGE_TIMEOUT;
  } else {
    // Led turn on for 1 second
    LedBlink(LED_ERROR_COLOR, 1000 ,1);
  }
}

void ButtonDoubleClickCallBack() {
  wait_user = false;

  if (pair_id != "null") {
    // Send a message to the pair
    MqttSend(pair_topic, kId);
    wait_ack = true;
    ack_timeout = millis() + ACKNOWLEDGE_TIMEOUT;
  } else {
    // Led turn on for 1 second
    LedBlink(LED_ERROR_COLOR, 2000 ,1);
  }
}

void ButtonLongPressCallBack() {
  MaintenanceUpdate();
}

void ButtonResetCallBack() {
  WiFi.disconnect(true);
  SPIFFS.begin();
  if (SPIFFS.exists(kPairingFile.c_str())) {
    SPIFFS.remove(kPairingFile.c_str());
  }
}

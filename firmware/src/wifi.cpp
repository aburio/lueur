#include "wifi.h"

//-----------------
// Wifi functions
//-----------------
void WifiWaitingForConnection() {
  WifiWaitingConnectionCallBack();

  while (WiFi.status() != WL_CONNECTED) {
    yield();
  }

  WifiConnectedCallBack();
}

void WifiInit() {
  WiFi.hostname(kDeviceId);

  // If Wifi as been previously set
  if ((WiFi.SSID() != "")) {
    WiFi.mode(WIFI_STA);
    WiFi.begin();
  }
  // Else create a captive portal
  else {
    CaptivePortal captiveportal;

    WifiCaptivePortalStartCallBack();

    WiFi.mode(WIFI_AP_STA);
    WiFi.softAPConfig(kApIp, kApIp, kApMask);

    captiveportal.start();

    WiFi.softAPdisconnect();
    WiFi.mode(WIFI_STA);

    WifiCaptivePortalEndCallBack();
  }

  WifiWaitingForConnection();
}

__attribute__ ((weak)) void WifiCaptivePortalStartCallBack() {
  // To be defined by user
}

__attribute__ ((weak)) void WifiCaptivePortalEndCallBack() {
  // To be defined by user
}


__attribute__ ((weak)) void WifiWaitingConnectionCallBack() {
  // To be defined by user
}

__attribute__ ((weak)) void WifiConnectedCallBack() {
  // To be defined by user
}

void WifiProcess() {
  if (WiFi.status() != WL_CONNECTED) {
    WifiInit();
  }
  else {
    WifiProcessCallBack();
  }
}

__attribute__ ((weak)) void WifiProcessCallBack() {
  // To be defined by user
}

#ifndef __CAPTIVEPORTAL_INCLUDED__
#define __CAPTIVEPORTAL_INCLUDED__

#include "data.h"
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <FS.h>

extern "C" {
#include "c_types.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "mem.h"
#include "user_interface.h"
#include "smartconfig.h"
#include "lwip/err.h"
#include "lwip/dns.h"
}

#define DNS_PORT 53
#define HTTP_PORT 80

class CaptivePortal {
public:
  CaptivePortal();
  ~CaptivePortal();

  void start();

private:
  void handleStatus(AsyncWebServerRequest *request);
  void handleWifi(AsyncWebServerRequest *request);
  void handleScan(AsyncWebServerRequest *request);
  void handleNotFound(AsyncWebServerRequest *request);

  void updateStatus();

  void sendResponse(AsyncWebServerRequest *request, const uint32_t code, const String type, const String content);

  DNSServer *dns_server_;
  AsyncWebServer *server_;
  uint8_t status_;
  bool finish_;
  uint32_t time_;
};

#endif

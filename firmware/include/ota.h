#ifndef LUEUR_FW_OTA_H_
#define LUEUR_FW_OTA_H_

#include "data.h"
#include "mqtt.h"
#include <ESP8266httpUpdate.h>
#include <ESP8266HTTPClient.h>
#include <FS.h>

enum OtaStatus {
  FAILED = 0,
  SUCCESS = 1,
  UP_TO_DATE = 2
};

OtaStatus OtaAppUpdate(const String server_domain);
OtaStatus OtaSpiffsUpdate(const String server_domain);

#endif

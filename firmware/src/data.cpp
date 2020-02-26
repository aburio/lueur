#include "data.h"

const String kBuildCommit = BUILD_COMMIT;

const String kName = "Lueur";
const String kId = String(ESP.getChipId());
const String kDeviceId = kName + "_" + kId;

const IPAddress kApIp(10, 0, 0, 1);
const IPAddress kApMask(255, 255, 255, 0);
const String kCaptivePortalDomain = "setup.lueur.lan";

const String kUpdateServer = " ";

const char *kMqttDomain = " ";
const uint16_t kMqttPort = 8883;

const String kBaseTopic = "lueur";
const String kDeviceTopic = kBaseTopic + "/" + kId;
const String kWelcomeTopic = kBaseTopic + "/welcome";
const String kBuildTopic = kBaseTopic + "/build";
const String kDeviceStatusTopic = kBaseTopic + "/" + kId + "/status";
const String kMaintenanceTopic = kBaseTopic + "/maintenance";

const String kPairingFile = "/pair.conf";

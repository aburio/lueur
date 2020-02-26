#ifndef LUEUR_FW_WIFI_H_
#define LUEUR_FW_WIFI_H_

#include "data.h"
#include "captiveportal.h"

void WifiInit();
void WifiCaptivePortalStartCallBack(); // Weak function
void WifiCaptivePortalEndCallBack(); // Weak function
void WifiWaitingConnectionCallBack(); // Weak function
void WifiConnectedCallBack(); // Weak function
void WifiProcess();
void WifiProcessCallBack(); // Weak function

#endif

#include "captiveportal.h"

WiFiEventHandler connectedEventHandler;
WiFiEventHandler disconnectedEventHandler;
uint32_t timeout_ = 10000;

CaptivePortal::CaptivePortal() {
  dns_server_ = new DNSServer();
  server_ = new AsyncWebServer(80);
  status_ = wifi_station_get_connect_status();
  finish_ = false;
}

CaptivePortal::~CaptivePortal() {
  delete dns_server_;
  delete server_;
}

void CaptivePortal::start() {
  // Redirection des requêtes vers le portail
  dns_server_->setErrorReplyCode(DNSReplyCode::NoError);
  dns_server_->start(DNS_PORT, "*", kApIp);

  connectedEventHandler = WiFi.onStationModeGotIP([&](const WiFiEventStationModeGotIP& event)
  {
    status_ = STATION_GOT_IP;
  });

  disconnectedEventHandler = WiFi.onStationModeDisconnected([&](const WiFiEventStationModeDisconnected& event)
  {
    WiFi.disconnect();
    switch(event.reason) {
      case 201:
        status_ = STATION_NO_AP_FOUND;
        break;
      case 202:
        status_ = STATION_WRONG_PASSWORD;
        break;
      default:
        status_ = STATION_CONNECT_FAIL;
    }
  });

  // Initialisation du server
  server_->serveStatic("/", SPIFFS, "/www/").setCacheControl("max-age:120");
  server_->on("/wifi", HTTP_POST, std::bind(&CaptivePortal::handleWifi, this, std::placeholders::_1));
  server_->on("/scan", HTTP_GET, std::bind(&CaptivePortal::handleScan, this, std::placeholders::_1));
  server_->on("/status", HTTP_GET, std::bind(&CaptivePortal::handleStatus, this, std::placeholders::_1));
  server_->onNotFound(std::bind(&CaptivePortal::handleNotFound, this, std::placeholders::_1));
  server_->begin();

  WiFi.scanNetworks();
  WiFi.softAP(kDeviceId.c_str(), NULL, 1, 0, 1);

  while (finish_ == false) {
    if ((status_ == STATION_GOT_IP) && (time_ + timeout_ < millis())) {
      finish_ = true;
    }
    dns_server_->processNextRequest();
    yield();
  }

  dns_server_->stop();
}

void CaptivePortal::handleStatus(AsyncWebServerRequest *request) {
  switch(status_) {
    case STATION_GOT_IP:
      sendResponse(request, 200, "text/plain", "0");
      time_ = millis();
      break;
    case STATION_CONNECTING:
      sendResponse(request, 200, "text/plain", "1");
      break;
    case STATION_WRONG_PASSWORD:
      sendResponse(request, 200, "text/plain", "2");
      break;
    case STATION_NO_AP_FOUND:
      sendResponse(request, 200, "text/plain", "3");
      break;
    case STATION_CONNECT_FAIL:
      sendResponse(request, 200, "text/plain", "4");
      break;
    case STATION_IDLE:
    default:
      sendResponse(request, 200, "text/plain", "5");
  }
}

void CaptivePortal::handleWifi(AsyncWebServerRequest *request) {
  if (request->hasArg("s") && request->hasArg("p")) {
    String ssid = request->arg("s");
    String pass = request->arg("p");
    status_ = STATION_CONNECTING;
    request->redirect("/connexion.html");
    WiFi.disconnect();
    WiFi.hostname(kDeviceId);
    WiFi.begin(ssid.c_str(),pass.c_str());
  }
  else {
    request->redirect("/index.html");
  }
}

void CaptivePortal::handleScan(AsyncWebServerRequest *request) {
  String result = "[";
  int32_t n = WiFi.scanComplete();

  if(n == -2) {
    WiFi.scanNetworks(true);
  } else if(n >= 0) {
    for (int32_t i = 0; i < n; ++i) {
      String secure = "1";
      if (WiFi.encryptionType(i) == ENC_TYPE_NONE) {
        secure = "0";
      }
      result += "{\"name\":\"" + WiFi.SSID(i) + "\",\"strength\":" + WiFi.RSSI(i) + ",\"secure\":" + secure + "}";
      if (i < n - 1) {
        result += ",";
      }
    }
    WiFi.scanDelete();
    if (WiFi.scanComplete() == -2) {
      WiFi.scanNetworks(true);
    }
  }
  result += "]";
  sendResponse(request, 200, "text/json", result);
}

void CaptivePortal::handleNotFound(AsyncWebServerRequest *request) {
  request->redirect("http://" + kCaptivePortalDomain + "/index.html");
}

void CaptivePortal::sendResponse(AsyncWebServerRequest *request, const uint32_t code, const String type, const String content) {
  AsyncWebServerResponse *response = request->beginResponse(code, type, content);

  if (type.startsWith("text/")) {
    response->addHeader( "Cache-Control", "no-cache, no-store, must-revalidate" );
    response->addHeader( "Pragma", "no-cache" );
    response->addHeader( "Expires", "0" );
  }
  else {
    response->addHeader( "Cache-Control", "public, max-age=120" );
    response->addHeader( "Pragma", "cache" );
  }

  request->send(response);
}

#include "ota.h"

bool OtaDownloadFile(HTTPClient& http_client, const String file_url, const String file_path, const String file_md5);
const String OtaComputeMD5(File& file_path);
const String OtaComputeMD5(const String file_path);

 OtaStatus OtaAppUpdate(const String server_domain) {
  OtaStatus result = FAILED;
  String build = kBuildCommit;

  ESPhttpUpdate.rebootOnUpdate(false);
  t_httpUpdate_return app_ota = ESPhttpUpdate.update("http://" + server_domain + "/app.php", build);

  if (app_ota == HTTP_UPDATE_NO_UPDATES) {
    result = UP_TO_DATE;
  }
  else if (app_ota == HTTP_UPDATE_OK) {
    result = SUCCESS;
  }

  return result;
}

OtaStatus OtaSpiffsUpdate(const String server_domain) {
  OtaStatus result = UP_TO_DATE;
  uint32_t http_result = 0;

  HTTPClient *http_client = new HTTPClient();
  const char* headerkeys[] = { "Content-Disposition", "MD5" };
  size_t headerkeyssize = sizeof(headerkeys) / sizeof(char*);

  http_client->begin("http://" + server_domain + "/spiffs.php");
  http_client->setUserAgent("Lueur-Spiffs-Update");
  http_client->collectHeaders(headerkeys, headerkeyssize);
  http_result = http_client->GET();

  if (http_result == HTTP_CODE_OK && http_client->hasHeader("Content-Disposition")) {
    String file_md5 = http_client->header("MD5");
    String file_name = "/" + http_client->header("Content-Disposition");
    file_name.remove(0, file_name.indexOf('=') + 1);

    if (SPIFFS.exists(file_name)) {
      SPIFFS.remove(file_name);
    }

    File file_update_list = SPIFFS.open(file_name,"w+");

    if (file_update_list) {
      uint32_t pos = 0;
      uint32_t bytes_written = http_client->writeToStream(&file_update_list);
      http_client->end();

      if(file_md5 == OtaComputeMD5(file_update_list)) {
        file_update_list.seek(0);

        while (pos < bytes_written && result != FAILED) {
          String next_file_md5 = file_update_list.readStringUntil(' ');
          file_update_list.findUntil("spiffs","\n");
          String next_file_path = file_update_list.readStringUntil('\n');

          String url = "http://" + server_domain + "/spiffs" + next_file_path;

          if(SPIFFS.exists(next_file_path)) {
            if (next_file_md5 != OtaComputeMD5(next_file_path)) {
              bool file_update = OtaDownloadFile(*http_client, url, next_file_path, next_file_md5);

              if (!file_update) {
                result = FAILED;
              }
              else {
                result = SUCCESS;
              }
            }
          }
          else {
            bool file_update = OtaDownloadFile(*http_client, url, next_file_path, next_file_md5);

            if (!file_update) {
              result = FAILED;
            }
            else {
              result = SUCCESS;
            }
          }

          pos = file_update_list.position();
          yield();
        }
      }
      else {
        result = FAILED;
      }

      file_update_list.close();
      SPIFFS.remove(file_name);
    }
  }
  else {
    http_client->end();
    result = FAILED;
  }

  delete http_client;
  return result;
}

bool OtaDownloadFile(HTTPClient& http_client, const String file_url, const String file_path, const String file_md5) {
  bool result = false;
  uint32_t http_result = 0;
  String temp_file_path = "/temp_file.tmp";
  File temp_file = SPIFFS.open(temp_file_path,"w+");

  http_client.begin(file_url);
  http_result = http_client.GET();

  if (http_result == HTTP_CODE_OK && temp_file) {
    http_client.writeToStream(&temp_file);
    http_client.end();

    if (file_md5 == OtaComputeMD5(temp_file)) {
      temp_file.close();

      if (SPIFFS.exists(file_path)) {
        SPIFFS.remove(file_path);
      }

      if (SPIFFS.rename(temp_file_path, file_path)) {
        result = true;
      }
    }
  }
  else {
    http_client.end();
    temp_file.close();
  }

  SPIFFS.remove(temp_file_path);

  return result;
}

const String OtaComputeMD5(File& file) {
  MD5Builder compute_md5;
  String result = "0";

  if (file) {
    file.seek(0);
    compute_md5.begin();
    compute_md5.addStream(file, file.size());
    compute_md5.calculate();
    result = compute_md5.toString();
  }

  return result;
}

const String OtaComputeMD5(const String file_path) {
  File file = SPIFFS.open(file_path, "r");

  return OtaComputeMD5(file);
}

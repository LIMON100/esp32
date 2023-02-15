/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-web-server-microsd-card/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/
#define FW_VERSION "0.0.7"
#include <ArduinoOTA.h>

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <SPIFFSEditor.h>
#include <EEPROM.h>
#include "SPIFFS.h"
#include <ArduinoJson.h>
// #include "BluetoothSerial.h"
#include<unity.h>
#include<iostream>


#define BUFFERSIZE 4096
// SKETCH BEGIN
bool restartRequired = false;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");
AsyncWebSocket ws_k210("/serial_k210");
AsyncWebSocket ws_esp32("/serial_esp32");
String boot_py = "";
String k210FlashFiles = "";
#define FFS SPIFFS

const char *ssid = "TTLink_2.4G";
const char *password = "12345678";
const char *hostName = "Llama-Eye";
const char *http_username = "admin";
const char *http_password = "admin";
// BluetoothSerial SerialBT;
uint8_t flash_with_bt = 1;
uint8_t buffer[BUFFERSIZE];
uint16_t buffer_pointer = 0;
#define SCK 18
#define MISO 23
#define MOSI 14
#define CS 19
SPIClass spi = SPIClass(VSPI);
DynamicJsonDocument esp_status_js(1024);

#define RESET_K210 21
#define HEARTBIT_LED 32
#define START_CAP_K210 33
#define K210_READY 36
String _ssid = "";
String _pass = "";
String _ip = "";
String _gw = "";
uint8_t heartbit_led_state = 0;
uint8_t read_boot_py_finish = 0;
uint32_t read_boot_py_time = 0;
uint32_t heartbit_millis = 0;
uint8_t k210_log_state = 1;
uint8_t esp32_log_state = 1;
uint8_t sdcard_state = 0;
uint32_t sdcard_config_timer = 0;
volatile int interruptCounter;
int totalInterruptCounter;
hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
String k210_app_version = "";
String k210_fw_version = "";
void appendFile(fs::FS &fs, const char *path, String message);
String write_app_index(String index);

String passed = "PASSED";
String failed = "FAILED";

#define APPS_NUM 3
uint8_t app_index = 0;


std::vector<String> vecOfstr{"hello"};

typedef enum FS_WRITE{
    OPEN_FILE_FAILED   = 0,
    WRITE_FILE_DONE    = 1,
    WRITE_FILE_FAILED  = 2,
    INIT_FAILED        = 3
}FS_WRITE_STATUS;

typedef enum FS_READ{
    FAILED_FILE_PATH  = 0,
    READ_FILE_DONE    = 1,
    READ_FILE_FAILED = 2,
    FILE_COMPARE = 3,
    FAILED_FILE_COMPARE = 4
}FS_READ_STATUS;

typedef enum FS_DELETE{
    FILE_DELETE_FAILED  = 0,
    FILE_DELETE_SUCCESS = 1
}FS_DELETE_STATUS;



// std::map<std::string, int> output_oneventk210{
//   {"run_main_app", 0},
//   {"stop_app" , 0},
//   {"start_capture" , 0},
//   {"clear_log" , 0}
// };

int test_a = 0;
int output_run_main_app = 0;
int output_stop_app = 0;
int output_start_capture = 0;
int output_clear_log = 0;
int output_reset_main_app = 0;

void wifi_ssids_scan()
{
  Serial.println("scan start");
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
  {
    Serial.println("no networks found");
  }
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? " " : "*");
      // create a json list of ssid
      esp_status_js["ssids"][i] = WiFi.SSID(i);
      // doc["ssid"][i] = WiFi.SSID(i) + " (" + WiFi.RSSI(i) + ")";
    }
  }
}
String processor(const String &var)
{
  if (var == "SSID")
  {
    return _ssid;
  }
  else if (var == "PASS")
  {
    return _pass;
  }
  else if (var == "IP")
  {
    return _ip;
  }
  else if (var == "[WebTool]")
  {
    return String(FW_VERSION) + " " + String(__DATE__) + " " + String(__TIME__);
  }
  return String();
}
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  if (type == WS_EVT_CONNECT)
  {
    Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
    client->printf("Hello Client %u :)", client->id());
    client->ping();
  }
  else if (type == WS_EVT_DISCONNECT)
  {
    Serial.printf("ws[%s][%u] disconnect\n", server->url(), client->id());
  }
  else if (type == WS_EVT_ERROR)
  {
    Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t *)arg), (char *)data);
  }
  else if (type == WS_EVT_PONG)
  {
    Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len) ? (char *)data : "");
  }
  else if (type == WS_EVT_DATA)
  {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    String msg = "";
    if (info->final && info->index == 0 && info->len == len)
    {
      // the whole message is in a single frame and we got all of it's data
      Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT) ? "text" : "binary", info->len);

      if (info->opcode == WS_TEXT)
      {
        for (size_t i = 0; i < info->len; i++)
        {
          msg += (char)data[i];
        }
      }
      else
      {
        char buff[3];
        for (size_t i = 0; i < info->len; i++)
        {
          sprintf(buff, "%02x ", (uint8_t)data[i]);
          msg += buff;
        }
      }
      Serial.printf("%s\n", msg.c_str());

      if (info->opcode == WS_TEXT)
        client->text("I got your text message");
      else
        client->binary("I got your binary message");
    }
    else
    {
      // message is comprised of multiple frames or the frame is split into multiple packets
      if (info->index == 0)
      {
        if (info->num == 0)
          Serial.printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
        Serial.printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
      }

      Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT) ? "text" : "binary", info->index, info->index + len);

      if (info->opcode == WS_TEXT)
      {
        for (size_t i = 0; i < len; i++)
        {
          msg += (char)data[i];
        }
      }
      else
      {
        char buff[3];
        for (size_t i = 0; i < len; i++)
        {
          sprintf(buff, "%02x ", (uint8_t)data[i]);
          msg += buff;
        }
      }
      Serial.printf("%s\n", msg.c_str());

      if ((info->index + len) == info->len)
      {
        Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
        if (info->final)
        {
          Serial.printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT) ? "text" : "binary");
          if (info->message_opcode == WS_TEXT)
            client->text("I got your text message");
          else
            client->binary("I got your binary message");
        }
      }
    }
  }
}

FS_READ_STATUS send_file_to_k210(fs::FS &fs, const char *path)
{
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file)
  {
    Serial.println("Failed to open file for reading");
    return FAILED_FILE_PATH;
  }

  Serial.print("Star Read from file ");
  std::vector<String> v;
  while (file.available())
  {
    boot_py += (char)file.read();
    v.push_back(file.readStringUntil('\n')); 
  }
  Serial.print("End Read from file: ");

  for (String s : v) {
    Serial.println(s);
  }

  if (v == vecOfstr)
  {
      file.close();
      return READ_FILE_DONE, FILE_COMPARE;
  }

  if (v != vecOfstr)
  {
      file.close();
      return READ_FILE_DONE, FAILED_FILE_COMPARE;
  }

}

void exec_python_file(String file_name)
{
  Serial2.write(0x05); // write file by line
  Serial2.print("def run(runfile):\n"
                "  with open(runfile,\"r\") as rnf:\n"
                "    fileread = rnf.read()\n"
                "    print(fileread)   \n"
                "    compiled_code = compile(fileread, \"<string>\", \"exec\")\n"
                "    exec(compiled_code)   \n"
                "run(\"" +
                file_name + "\")\n");
  Serial2.write(0x04);
}
uint8_t initSDCard()
{
  Serial.println("Initializing SD card...");
  spi.begin(SCK, MISO, MOSI, CS);

  if (!SD.begin(CS, spi, 80000000))
  {
    Serial.println("Card Mount Failed");
    delay(1000);
    ESP.restart();
    return 0;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE)
  {
    Serial.println("No SD card attached");
    return 0;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC)
  {
    Serial.println("MMC");
  }
  else if (cardType == CARD_SD)
  {
    Serial.println("SDSC");
  }
  else if (cardType == CARD_SDHC)
  {
    Serial.println("SDHC");
  }
  else
  {
    Serial.println("UNKNOWN");
  }
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  return 1;
}
void IRAM_ATTR onTimer()
{
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  portEXIT_CRITICAL_ISR(&timerMux);
}
void initWiFi()
{
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(hostName, password);

  if (EEPROM.readString(0) == "C:Y")
  {
    _ssid = EEPROM.readString(10);
    _pass = EEPROM.readString(50);
    _ip = EEPROM.readString(90);
    uint8_t ip[4];
    sscanf(_ip.c_str(), "%u.%u.%u.%u", &ip[0], &ip[1], &ip[2], &ip[3]);
    IPAddress save_ip = IPAddress(ip);

    save_ip.printTo(Serial);
    // set ip add
    IPAddress gw = save_ip;
    IPAddress subnet = IPAddress(255, 255, 255, 0);
    gw[3] = 1;

    printf("Connecting to WiFi configured...\r\n");
    printf("SSID: %s, Pass: %s, ip: %s\r\n", _ssid.c_str(), _pass.c_str(), _ip.c_str());
    printf("local dsn: http://llama-eye.local/\r\n");
    WiFi.config(save_ip, gw, subnet);

    WiFi.begin(_ssid.c_str(), _pass.c_str());

    if (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
      Serial.printf("STA: Failed!\n");
      WiFi.disconnect(false);
      delay(1000);
      WiFi.begin(ssid, password);
    }
  }
  else
  {
    printf("Connecting to WiFi not configured yet...\r\n");
    printf("conneted to AP:%s, pass:%s \r\n", hostName, password);
    Serial.print("local ip:" + WiFi.localIP().toString());
  }
}
FS_DELETE_STATUS deleteFile(fs::FS &fs, const char *path)
{
  FS_DELETE_STATUS delete_file_status;

  Serial.printf("Deleting file: %s\r\n", path);
  if (fs.remove(path))
  {
    Serial.println("- file deleted");
    delete_file_status = FILE_DELETE_SUCCESS;
  }
  else
  {
    Serial.println("- delete failed");
    delete_file_status = FILE_DELETE_FAILED;
  }
  return delete_file_status;
}

FS_WRITE_STATUS writeFile(fs::FS &fs, const char *path, const char *message)
{
  
  FS_WRITE_STATUS write_file_status = OPEN_FILE_FAILED; 
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file)
  {
    Serial.println("- failed to open file for writing");
    return write_file_status;
  }
  if (file.print(message))
  {
    Serial.println("- file written");
    write_file_status = WRITE_FILE_DONE;
  }
  else
  {
    Serial.println("- write failed");
    write_file_status = WRITE_FILE_FAILED;
  }
  file.close();
 
  return write_file_status; 

}

void onWsEventESP32(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  String dataStr;
  for (size_t i = 0; i < len; i++)
  {
    dataStr += (char)data[i];
  }
  Serial.println(dataStr);

  if (type == WS_EVT_CONNECT)
  {
    Serial.println("Websocket client connection received k210");
    // sendDataWebsocket();
  }
  else if (type == WS_EVT_DISCONNECT)
  {
    Serial.println("Client disconnected");
  }

  if (dataStr.indexOf("cmd") >= 0)
  {
    String cmd = dataStr.substring(dataStr.indexOf("[") + 1, dataStr.indexOf("]"));
    if (cmd == "stop_serial")
    {
      esp32_log_state = 0;
    }
    else if (cmd == "start_serial")
    {
      esp32_log_state = 1;
    }
    else if (cmd == "clear_log")
    {
      deleteFile(SPIFFS, "/log_esp32.txt");
      writeFile(SPIFFS, "/log_esp32.txt", "");
    }
    else if (cmd == "restart_app")
    {
      Serial.println("restart_app");
      writeFile(SPIFFS, "/log_esp32.txt", "ESP32 reset");
      restartRequired = true;
    }
  }
}
void run_folder(String dir)
{
  String s = "import os; os.listdir(" + dir + ")";
  Serial2.println(s);
}

void set_start_app(String index)
{
  Serial2.write(0x05); // write file by line
  Serial2.print(" " + index + "'\n"
                              "with open('/flash/app.conf', 'r+b') as f:\n"
                              "   f.write(app_index)\n");
  Serial2.write(0x04);
}

void copy_file(String file_name)
{
  Serial.println("copy file");
  String s = "file_name = '/sd" + file_name + "'\n"
              "file_dst = '/flash" + file_name+"'\n";

  writeFile(SD, "/files_path.py",s.c_str());

  write_app_index("3");
  delay(1000);
}
void run_file(String file_name)
{
  Serial.println("run file");
  Serial2.print("file_name=\"" + file_name + "\"\n"
                                              "with open(\"__download.py\") as f:\n"
                                              "    exec(f.read(), globals())\n"
                                              "###");
  // "####1#1#1#1#1#1#1#1#1#1#1#1#1#1#1#1#1#1#1#1#1");
}




void onWsEventK210(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{

  // output_oneventk210["run_main_app"] = 0;
  // output_oneventk210["stop_app"] = 0;
  // output_oneventk210["start_capture"] = 0;
  // output_oneventk210["clear_log"] = 0;


  String dataStr;
  for (size_t i = 0; i < len; i++)
  {
    dataStr += (char)data[i];
  }
  Serial.println(dataStr);

  if (type == WS_EVT_CONNECT)
  {
    Serial.println("Websocket client connection received k210");
    // sendDataWebsocket();
  }
  else if (type == WS_EVT_DISCONNECT)
  {
    Serial.println("Client disconnected");
  }

  if (dataStr.indexOf("cmd") >= 0)
  {
    String cmd = dataStr.substring(dataStr.indexOf("[") + 1, dataStr.indexOf("]"));
    if (cmd == "restart_app")
    {
      Serial.println("restart_app");
      output_reset_main_app = 1;

      digitalWrite(RESET_K210, LOW);
      delay(100);
      digitalWrite(RESET_K210, HIGH);
    }
    else if (cmd == "run_app")
    {                           
               //----on
      output_run_main_app = 1;
      FS_READ_STATUS check_output, file_compare = send_file_to_k210(SD, "/boot.py"); // k210 rest
      // onn

      read_boot_py_finish = 1;
      read_boot_py_time = millis();

    }
    else if (cmd == "stop_app")
    {
      output_stop_app = 1;
      Serial2.write(0x03);
      digitalWrite(START_CAP_K210, HIGH);

      // if (digitalRead(START_CAP_K210) == LOW) {
      //   Serial.println();
      //   Serial.printf("CHECK STOP BUTTON WORKING: PASSED");
      //   Serial.println();
      // }
    }

    else if (cmd == "capture")
    {
      output_start_capture = 1;
      digitalWrite(START_CAP_K210, LOW);
      delay(100);
      digitalWrite(START_CAP_K210, HIGH);
    }
    else if (cmd == "stop_serial")
    {
      k210_log_state = 0;
    }
    else if (cmd == "start_serial")
    {
      k210_log_state = 1;
    }
    else if (cmd == "clear_log")
    {
      output_clear_log = 1;
      FS_DELETE_STATUS check_delete = deleteFile(SPIFFS, "/log_k210.txt");
      
      const char* path= "/log_k210.txt";
      send_file_to_k210(SD, path);

      writeFile(SPIFFS, "/log_k210.txt", "");

    }
  }
  else if (dataStr.indexOf("rpel") >= 0)
  {
    String cmd = dataStr.substring(dataStr.indexOf("[") + 1, dataStr.indexOf("]"));
    Serial2.println(cmd);
  }
  else if (dataStr.indexOf("run") >= 0)
  {
    String app = dataStr.substring(dataStr.indexOf("[") + 1, dataStr.indexOf("]"));

    // send_file_to_k210(SD, app.c_str()); // k210 rest
    exec_python_file(app);
    // onn
    // read_boot_py_finish = 1;
    // read_boot_py_time = millis();
  }
  else if (dataStr.indexOf("lf") >= 0)
  {
    String dir_name = dataStr.substring(dataStr.indexOf("[") + 1, dataStr.indexOf("]"));
    run_folder("'" + dir_name + "'");
  }
}

String write_app_index(String index)
{

  File file = SD.open("/app_index.json");

  if (!file)
  {
    Serial.println("Failed to open file for reading");
    return "0";
  }

  Serial.print("Read from file: ");
  String json_file = "";
  while (file.available())
  {
    json_file += (char)file.read();
  }
  file.close();
  Serial.write(json_file.c_str(), json_file.length());
  json_file[16] = index[0];
  app_index = index.toInt();
  Serial.println("");
  Serial.write(json_file.c_str(), json_file.length());
  writeFile(SD, "/app_index.json", json_file.c_str());

  return "1";
}

std::map<std::string, int> output_test;

std::map<std::string, int> web_server_setup()
{

  
  // Send OTA events to the browser
  ArduinoOTA.onStart([]()
                     { events.send("Update Start", "ota"); });
  ArduinoOTA.onEnd([]()
                   { events.send("Update End", "ota"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                        {
    char p[32];
    sprintf(p, "Progress: %u%%\n", (progress/(total/100)));
    events.send(p, "ota"); });
  ArduinoOTA.onError([](ota_error_t error)
                     {
    if(error == OTA_AUTH_ERROR) events.send("Auth Failed", "ota");
    else if(error == OTA_BEGIN_ERROR) events.send("Begin Failed", "ota");
    else if(error == OTA_CONNECT_ERROR) events.send("Connect Failed", "ota");
    else if(error == OTA_RECEIVE_ERROR) events.send("Recieve Failed", "ota");
    else if(error == OTA_END_ERROR) events.send("End Failed", "ota"); });
  ArduinoOTA.setHostname(hostName);
  ArduinoOTA.begin();

  MDNS.addService("http", "tcp", 80);

  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  events.onConnect([](AsyncEventSourceClient *client)
                   { client->send("hello!", NULL, millis(), 1000); });
  server.addHandler(&events);

  server.addHandler(new SPIFFSEditor(SD, http_username, http_password));

  server.on("/heap", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/plain", String(ESP.getFreeHeap())); });

  server.serveStatic("/", FFS, "/").setDefaultFile("index.html");

  server.onNotFound([](AsyncWebServerRequest *request)
                    {
    Serial.printf("NOT_FOUND: ");
    if(request->method() == HTTP_GET)
      Serial.printf("GET");
    else if(request->method() == HTTP_POST)
      Serial.printf("POST");
    else if(request->method() == HTTP_DELETE)
      Serial.printf("DELETE");
    else if(request->method() == HTTP_PUT)
      Serial.printf("PUT");
    else if(request->method() == HTTP_PATCH)
      Serial.printf("PATCH");
    else if(request->method() == HTTP_HEAD)
      Serial.printf("HEAD");
    else if(request->method() == HTTP_OPTIONS)
      Serial.printf("OPTIONS");
    else
      Serial.printf("UNKNOWN");
    Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());

    if(request->contentLength()){
      Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
      Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
    }

    int headers = request->headers();
    int i;
    for(i=0;i<headers;i++){
      AsyncWebHeader* h = request->getHeader(i);
      Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
    }

    int params = request->params();
    for(i=0;i<params;i++){
      
      Serial.println("INSIDE ..ARAMSS.. FOR SD");
      AsyncWebParameter* p = request->getParam(i);
      if(p->isFile()){
        output_test["detect_sd_files"] = 1;
        Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
      } else if(p->isPost()){
        Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
      } else {
        Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
      }
    }
    output_test["detect_sd_files"] = 0;

    request->send(404); });

  output_test["upload_files"] = 0;
  server.onFileUpload([](AsyncWebServerRequest *request, const String &filename, size_t index, uint8_t *data, size_t len, bool final)
    {

    output_test["upload_files"] = 1;
    if(!index)
      Serial.printf("UploadStart: %s\n", filename.c_str());
    Serial.printf("%s", (const char*)data);
    if(final)
      Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index+len); });

  server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
                       {
    if(!index)
      Serial.printf("BodyStart: %u\n", total);
    Serial.printf("%s", (const char*)data);
    if(index + len == total)
      Serial.printf("BodyEnd: %u\n", total); });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(FFS, "/index.html", String(), false, processor); });

  server.on("/wifi-confg", HTTP_POST, [](AsyncWebServerRequest *request)
            {
              int params = request->params();

              uint8_t config_steps = 0;
              for (int i = 0; i < params; i++)
              {
                AsyncWebParameter *p = request->getParam(i);
                if (p->name().equals("ssid"))
                {

                  String _ssid = p->value();
                  EEPROM.writeString(10, _ssid);
                  EEPROM.commit();
                  Serial.println("ssid: " + _ssid); 
                  config_steps = 1;
                }else if (p->name().equals("pass"))
                {

                  String _pass = p->value();

                  EEPROM.writeString(50, _pass);
                  EEPROM.commit();
                  Serial.println("pass: " + _pass);
                     config_steps = 2;
                }else                 if (p->name().equals("ip"))
                {

                  String ip = p->value();

                  EEPROM.writeString(90, ip);
                  EEPROM.commit();
                  Serial.println("ip: " + ip);
                     config_steps = 3;
                }

                if (config_steps == 3)
                {
                     EEPROM.writeString(0, "C:Y"  );
                      EEPROM.commit();
                      Serial.println("Config Done");
                }
              
              }
            

           		request->send(FFS, "/settings.html", String(), false, processor); });

  server.on("/list-flash", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              DynamicJsonDocument doc(1024);
              doc["files"] = k210FlashFiles;
              run_folder("/flash");
           

              String output;
              serializeJson(doc, output);
              request->send(200, "application/json", output); });

  server.on("/esp-dash", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              DynamicJsonDocument doc(1124);
              doc["ram"] = ESP.getFreeHeap() / 1024;
              doc["flash"] = ESP.getSketchSize() / 1024;
              doc["mac"] = WiFi.macAddress();
              doc["dt"] = __DATE__;
              doc["time"] = __TIME__;
              doc["rssi"] = WiFi.RSSI();
              doc["fw"] = String(FW_VERSION);          
              doc["app_index"] =    app_index;
              doc["uptime"] = millis() / 1000;
              
              Serial.printf("FINDING APP_INDEX>>############");
              Serial.println(app_index);
              Serial.println();
                             
              String output;
              serializeJson(doc, output);
              request->send(200, "application/json", output); });

  server.on("/esp-status", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              esp_status_js["ram"] = ESP.getFreeHeap() / 1024;
              esp_status_js["flash"] = ESP.getSketchSize();
              esp_status_js["mac"] = WiFi.macAddress();
              esp_status_js["dt"] = __DATE__;
              esp_status_js["rssi"] = WiFi.RSSI();
              esp_status_js["ssid"] = WiFi.SSID();
              esp_status_js["ip"] = WiFi.localIP().toString();
              esp_status_js["gw"] = WiFi.gatewayIP().toString();
              esp_status_js["netmask"] = WiFi.subnetMask().toString();
              esp_status_js["dns"] = WiFi.dnsIP().toString();
              esp_status_js["uptime"] = millis() / 1000;
              esp_status_js["flash"] = ESP.getFlashChipSize() / 1024;
              esp_status_js["fw"] = String(FW_VERSION);  

              String output;
            
              serializeJson(esp_status_js, output);
              request->send(200, "application/json", output); });

  server.on("/settings.html", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(FFS, "/settings.html", String(), false, processor); });


  output_test["esp_firmware"] = 0;
  server.on(
      "/update", HTTP_POST, [&](AsyncWebServerRequest *request)
      {
        Serial.println();

        Serial.print("NINA FIRMWARE UPDATE");
        Serial.print("Update ");
        output_test["esp_firmware"] = 1;

        AsyncWebServerResponse *response = request->beginResponse((Update.hasError()) ? 500 : 200, "text/plain",
                                                                  (Update.hasError()) ? "FAIL" : "OK");
        response->addHeader("Connection", "close");
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
        restartRequired = true; },

      [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
      {
        if (!index) {

          // size_t content_len = request->contentLength();

            int cmd = (filename.indexOf("spiffs") > -1) ? U_SPIFFS : U_FLASH;
            if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd)) { // Start with max available size
                Update.printError(Serial);
            }
        }

        if (Update.write(data, len) != len) {
            Update.printError(Serial);
            output_test["esp_firmware"] = 0;
        }

        if (final) { 
            if (Update.end(true)) { 

            }
        } });

  // list avaliable ssid
  server.on("/app-info", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "application/json", k210_app_version); });

  server.on("/fw-info", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "application/json", k210_fw_version); });

  server.on("/start_app", HTTP_GET, [](AsyncWebServerRequest *request)
            {

              Serial.println("start_app");
              String index = request->getParam("start_app")->value();   
                      //  set_start_app(index);
              String check_index_update = write_app_index(index);
              
              Serial.println();
              if (check_index_update == "0")
              {
                // Serial.printf("switch from main application to data collection application: FAILED");
                output_test["change_python_script"] = 1;
              }
              Serial.println();
              if (check_index_update == "1")
              {
                // Serial.printf("switch from main application to data collection application: PASSED");
                output_test["change_python_script"] = 0;
              }

         request->send(FFS, "/settings.html", String(), false, processor); });

  server.on("/to-flash", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              //get path
              String path = request->getParam("path")->value();
            //print path
              copy_file(path );
              Serial.println("path: " + path);
         
             request->send(FFS, "/edit.html", String(), false, processor); });

  server.on("/enable-bt", HTTP_GET, [](AsyncWebServerRequest *request)
            {
                flash_with_bt = 1;
                  flash_with_bt = EEPROM.writeByte(256, flash_with_bt);
                  EEPROM.commit();
                  Serial.println("flash_with_bt: " + String(flash_with_bt));
                  Serial.println("bluetooth enabled");
         
             request->send(FFS, "/setting.html", String(), false, processor);
             restartRequired = true; });

  ws_k210.onEvent(onWsEventK210);
  ws_esp32.onEvent(onWsEventESP32);
  server.addHandler(&ws_k210);
  server.addHandler(&ws_esp32);
  server.serveStatic("/", FFS, "/");
  // server.serveStatic("/", SD, "/");
  server.serveStatic("/img", SD, "/img");
  server.serveStatic("/sd", SD, "/");

  server.begin();

  return output_test;
}

void readFileTok210(fs::FS &fs, const char *path)
{
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file)
  {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while (file.available())
  {
    Serial2.write(file.read());
  }
  file.close();
}

void appendFile(fs::FS &fs, const char *path, String message)
{
  Serial.printf("Appending to file: %s\r\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file)
  {
    Serial.println("- failed to open file for appending");
    return;
  }
  if (file.print(message))
  {
    Serial.println("- message appended");
  }
  else
  {
    Serial.println("- append failed");
  }
  file.close();
}

void serial_esp32(void *arg)
{
  for (;;)
  {
    if (esp32_log_state)
    {
      while (Serial.available())
      {
        String data = Serial.readString();
        appendFile(SPIFFS, "/log_esp32.txt", data);
      }
    }
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}

void serial_k210(void *arg)
{
  for (;;)
  {
    if (k210_log_state)
    {
      while (Serial2.available())
      {
        String data = Serial2.readString();
        appendFile(SPIFFS, "/log_k210.txt", data);
        Serial.println(data);
      }
    }

    if ((read_boot_py_finish == 1) && ((millis() - read_boot_py_time) > 4000) && (Serial.available() == 0)) //&& digitalRead(K210_READY) == HIGH
    {
      Serial.println("send popython file len: " + String(boot_py.length()));
      read_boot_py_finish = 0;
      Serial2.write(0x05); // write file by line
                           // readFileTok210(SPIFFS, "/temp_buf_file.py");

      Serial2.write(boot_py.c_str(), boot_py.length());
      delay(100);

      Serial2.write(0x04);
      //  Serial.println(boot_py);
      boot_py = "";
      deleteFile(SPIFFS, "/temp_buf_file.py");

      writeFile(SPIFFS, "/temp_buf_file.py", "");
    }
    vTaskDelay(1 / portTICK_PERIOD_MS);
  }
}
// void bt_task_loop(void *arg)
// {
//   for (;;)
//   {
//     if (SerialBT.available())
//     {
//       uint8_t b = SerialBT.read();
//       Serial2.write(b);
//     }
//     if (Serial2.available())
//     {
//       uint8_t b = Serial2.read();
//       SerialBT.write(b);
//     }
//   }
//   vTaskDelay(1 / portTICK_PERIOD_MS);
// }
// void init_bt()
// {
//   if (!SerialBT.begin("ESP32"))
//   {
//     Serial.println("An error occurred initializing Bluetooth");
//   }
//   else
//   {
//     Serial.println("Bluetooth initialized");
//     Serial.println("The device started, now you can pair it with bluetooth!");
//     Serial.println("To stop it, disconnect the board from power and connect it again");
//   }
// }

void read_app_index()
{
  File file = SD.open("/app_index.json");
  if (!file)
  {
    Serial.println("Failed to open file for reading");
  }

  Serial.print("Read from file: ");
  String json_file = "";
  while (file.available())
  {
    json_file += (char)file.read();
  }
  file.close();

  app_index = json_file[16] - '0';
}

void read_k210_fw_version()
{
  File file = SD.open("/fw_info.json");
  if (!file)
  {
    Serial.println("Failed to open file for reading");
  }

  Serial.print("Read from file: ");

  while (file.available())
  {
    k210_fw_version += (char)file.read();
  }
  file.close();
}

void read_k210_app_version()
{
  File file = SD.open("/current_app_ver.json");
  if (!file)
  {
    Serial.println("Failed to open file for reading");
  }

  Serial.print("Read from file: ");

  while (file.available())
  {
    k210_app_version += (char)file.read();
  }
  file.close();
}

void test_write_txt_file()
{
  const char* path= "/hello.txt";
  const char* message = "Hello";

  
  FS_WRITE_STATUS write_expected =  writeFile(SD, path, message);
  // FS_READ_STATUS read_expected, read_expected_data = send_file_to_k210(SD, path);

  // Write file
  TEST_ASSERT_EQUAL_UINT8 (write_expected, 1);
  
}

void test_read_txt_file()
{
  const char* path= "/hello.txt";
 
  FS_READ_STATUS read_expected = send_file_to_k210(SD, path);

  // Read the file
  TEST_ASSERT_EQUAL_UINT8 (read_expected, 0);
}


void test_run_main_app()
{
  TEST_ASSERT_EQUAL_UINT8 (output_run_main_app, 1);
}

void test_start_capture()
{
  TEST_ASSERT_EQUAL_UINT8 (output_stop_app, 1);
}

void test_stop_app()
{
  TEST_ASSERT_EQUAL_UINT8 (output_start_capture, 1);
}

void test_clear_log()
{
  TEST_ASSERT_EQUAL_UINT8 (output_clear_log, 1);
}

void test_restart_app()
{
  TEST_ASSERT_EQUAL_UINT8 (output_reset_main_app, 1);
}


void test_setup_webserver_list_sd()
{
  std::map<std::string, int> output;
	output = web_server_setup();

  std::map<std::string, int>::iterator it1 = output.begin();

  while (it1 != output.end()) 
  {
    
    if (it1->first == "detect_sd_files")
    {
      TEST_ASSERT_EQUAL(1, it1->second);
    }
    ++it1;
    
  }
}

void test_upload_files_to_sd()
{
  std::map<std::string, int> output;
	output = web_server_setup();

  std::map<std::string, int>::iterator it1 = output.begin();

  while (it1 != output.end()) 
  {
    
    if (it1->first == "upload_files")
    {
      TEST_ASSERT_EQUAL(1, it1->second);
    }
    ++it1;
    
  }
}

void test_setup_webserver_change_script()
{
  std::map<std::string, int> output;
	output = web_server_setup();

  std::map<std::string, int>::iterator it1 = output.begin();

  while (it1 != output.end()) 
  {
     std::cout << it1->first << ", return: " << it1->second << std:: endl;
    if (it1->first == "change_python_script")
    {
      TEST_ASSERT_EQUAL(1, it1->second);
    }
    ++it1;
    
  }
}



void test_esp_firmware()
{
  std::map<std::string, int> output;
	output = web_server_setup();

  std::map<std::string, int>::iterator it1 = output.begin();

  while (it1 != output.end()) 
  {
     std::cout << it1->first << ", return: " << it1->second << std:: endl;
    if (it1->first == "esp_firmware")
    {
      TEST_ASSERT_EQUAL(1, it1->second);
    }
    ++it1;
    
  }
}

void setup()
{
  Serial.begin(115200);
  Serial2.begin(115200);

  Serial2.setRxBufferSize(1024);
  EEPROM.begin(512);

  pinMode(RESET_K210, OUTPUT);
  pinMode(START_CAP_K210, OUTPUT);
  pinMode(HEARTBIT_LED, OUTPUT);
  digitalWrite(RESET_K210, LOW);
  digitalWrite(START_CAP_K210, HIGH);
  delay(1000);
  sdcard_state = initSDCard();
  read_app_index();
  read_k210_fw_version();

  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  if (!MDNS.begin("llama-eye"))
  {
    Serial.println("Error setting up MDNS responder!");
    while (1)
    {
      delay(1000);
    }
  }

  flash_with_bt = EEPROM.read(256);
  Serial.println("flash_with_bt: " + String(flash_with_bt));
  if (flash_with_bt == 1)
  {
    flash_with_bt = 0;
    flash_with_bt = EEPROM.writeByte(256, flash_with_bt);
    EEPROM.commit();
    // init_bt();
    // xTaskCreatePinnedToCore(bt_task_loop, "bt_task_loop", 10000, NULL, 1, NULL, 1);
  }
  else
  {
    initWiFi();
    writeFile(SPIFFS, "/temp_buf_file.py", "");
    writeFile(SPIFFS, "/log_k210.txt", "");
    writeFile(SPIFFS, "/log_esp32.txt", "");

    web_server_setup();
    delay(2000);
    xTaskCreatePinnedToCore(serial_k210, "taskK210", 10000, NULL, 1, NULL, 1);
    xTaskCreatePinnedToCore(serial_esp32, "taskEsp32", 10000, NULL, 1, NULL, 1);
  }
  wifi_ssids_scan();
  Serial.printf("TESTING START...........");
  Serial.println();
  Serial.println(test_a);
  delay (1000);

  test_a = test_a + 1;

 
  test_a = test_a + 2;
  UNITY_BEGIN();

  Serial.println();
  Serial.println();

  RUN_TEST(test_write_txt_file);

  RUN_TEST(test_read_txt_file);

  RUN_TEST(test_setup_webserver_list_sd);

  RUN_TEST(test_upload_files_to_sd);

  RUN_TEST(test_setup_webserver_change_script);

  RUN_TEST(test_run_main_app);

  RUN_TEST(test_start_capture);

  RUN_TEST(test_stop_app);

  RUN_TEST(test_clear_log);

  RUN_TEST(test_restart_app);

  RUN_TEST(test_esp_firmware);

  Serial.println();
  Serial.println();

  UNITY_END();
  
}

void loop()
{

  if (millis() - heartbit_millis >= 500)
  {
    digitalWrite(HEARTBIT_LED, heartbit_led_state);
    heartbit_millis = millis();
    heartbit_led_state = !heartbit_led_state;
  }

  if (restartRequired)
  {
    ESP.restart();
  }

  if (sdcard_state == 0)
  {
    if (millis() - sdcard_config_timer > 5000)
    {
      sdcard_config_timer = millis();
      sdcard_state = initSDCard();
      // UNITY_BEGIN();
      // Serial.println();
      // Serial.println();
      // RUN_TEST(test_write_txt_file);

      // RUN_TEST(test_read_txt_file);

      // RUN_TEST(test_setup_webserver_list_sd);

      // RUN_TEST(test_upload_files_to_sd);

      // RUN_TEST(test_setup_webserver_change_script);

      // RUN_TEST(test_run_main_app);

      // RUN_TEST(test_start_capture);

      // RUN_TEST(test_stop_app);

      // RUN_TEST(test_clear_log);

      // RUN_TEST(test_restart_app);

      // RUN_TEST(test_esp_firmware);

      // Serial.println();
      // Serial.println();
      // UNITY_END();
    }
  }

  ArduinoOTA.handle();
  ws.cleanupClients();
  vTaskDelay(10 / portTICK_PERIOD_MS);
}

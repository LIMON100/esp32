#ifndef MAIN_APP_H
#define MAIN_APP_H

#include <ArduinoOTA.h>
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>

#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <EEPROM.h>
#include <ArduinoJson.h>


#define BUFFERSIZE 4096
// SKETCH BEGIN
bool restartRequired = false;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
AsyncEventSource events("/events");
AsyncWebSocket ws_k210("/serial_k210");
AsyncWebSocket ws_esp32("/serial_esp32");
String boot_py = "";
#define FFS SPIFFS

const char *ssid = "TTLink_2.4G";
const char *password = "12345678";
const char *hostName = "Llama-Eye";
const char *http_username = "admin";
const char *http_password = "admin";
uint8_t buffer[BUFFERSIZE];
uint16_t buffer_pointer = 0;

#define RESET_K210 21
#define START_CAP_K210 33
String _ssid = "";
String _pass = "";
String _ip = "";
String _gw = "";
uint8_t read_boot_py_finish = 0;
uint32_t read_boot_py_time = 0;
uint8_t k210_log_state = 1;

String processor(const String &var);
void web_server_setup();
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
void onWsEventESP32(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
void onWsEventK210(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);


#endif
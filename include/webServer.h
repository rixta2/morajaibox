#pragma once

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

extern AsyncWebServer server;
extern String boxId;

void initWiFi(const char* ssid, const char* password);
void initWebServer();
void registerWithServer();
void broadcastStatus();
String getCurrentStatus();
void handlePuzzleUpdate(const String& puzzleData);
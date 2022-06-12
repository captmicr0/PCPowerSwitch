#ifndef _WIFI_H_
#define _WIFI_H_

#include <Arduino.h>

#include "DebugPrint.h"
#include "settings.h"
#include "times.h"

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESP8266mDNS.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPmDNS.h>
#endif

#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

#define WIFIMANAGER_SOFTAPSSID "ESP-WIFI-MANAGER"

typedef void (*wifi_pagesFn)(AsyncWebServer *server, bool wifimanager);

bool wifi_setup(Settings *settings, wifi_pagesFn pagesFn = NULL);
void wifi_loop(void);
void wifi_initPages(AsyncWebServer *server, bool wifimanager);
bool wifi_initSTA(void);
void wifi_scanSTA(void);

#endif // _WIFI_H_

#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <Arduino.h>

#include "DebugPrint.h"
#include "times.h"

#include <LittleFS.h>

typedef struct WiFiData {
	char ssid[32];
	char password[64];
    char ipaddress[16];
    char gateway[16];
    char subnet[16];
    char dns[16];
    char altdns[16];
    char mdns[32];
} WiFiData;

typedef struct Settings {
    uint8_t _magic1;
    uint8_t _magic2;
    WiFiData wifi;
} Settings;

#define sizeof_member(type, member) sizeof(((type*)0)->member)
#define SETTINGS_SIZE sizeof(Settings)
#define SETTINGS_FILE "/settings.struct"

//automatically saves any changed settings
//#define SETTINGS_SAVELOOP

bool settings_save(Settings *data);
bool settings_load(Settings *data);
void settings_loop(Settings *data);
bool settings_compare(Settings *data1, Settings *data2);
void settings_print(Settings *data);

#endif // _SETTINGS_H_

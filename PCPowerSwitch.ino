#include <Arduino.h>

/*
	requirements:
		ESPAsyncTCP
		ESPAsyncWebServer
*/

#include "DebugPrint.h"
#include "PinMap.h"
#include "leds.h"
#include "settings.h"
#include "wifi.h"
#include "pcpower.h"

Settings settings;

void setup(void);
void loop(void);
void pages(AsyncWebServer *server, bool wifimanager);

void setup(void)
{
	DebugPrint("\r\n====[PCPowerSwitch-Version2]====================================================\r\n\r\n");

	//scan WiFi
	//wifi_scanSTA();

    //check defined pins
	pinmap_check();

	//setup everything
	DebugPrint(
		"leds_setup: %s\r\n"
		"pcpower_setup: %s\r\n"
		"\r\n",
		leds_setup() ? "SUCCESS":"ERROR",
		pcpower_setup() ? "SUCCESS":"ERROR"
	);

	//load saved settings
	if (settings_load(&settings) == false)
	{
		DebugPrint("failed to load saved settings, using defaults\r\n");

		//set defaults
		strcpy(settings.wifi.ssid, "");
		strcpy(settings.wifi.password, "");
		strcpy(settings.wifi.ipaddress, "");
		strcpy(settings.wifi.gateway, "");
		strcpy(settings.wifi.subnet, "255.255.255.0");
		strcpy(settings.wifi.dns, "1.1.1.1");
		strcpy(settings.wifi.altdns, "1.0.0.1");
		strcpy(settings.wifi.mdns, "");

		//save defaults
		settings_save(&settings);
	}
	else
	{
		DebugPrint("successfully loaded saved settings\r\n");
	}

	//print settings
	settings_print(&settings);

	//setup the rest (MUST load settings first)
	DebugPrint(
		"wifi_setup: %s\r\n"
		"\r\n",
		wifi_setup(&settings, pages) ? "SUCCESS":"ERROR"
	);
}

void loop(void)
{
	leds_loop();
	pcpower_loop();

	wifi_loop();
	settings_loop(&settings);
}

void pages(AsyncWebServer *server, bool wifimanager)
{
	//press.fn
    server->on("/press.fn", HTTP_GET, [](AsyncWebServerRequest *request){
		unsigned long time = 0;

        int paramcnt = request->params();
        for (int i = 0; i < paramcnt; i++)
        {
            AsyncWebParameter *param = request->getParam(i);
			if (strcmp(param->name().c_str(), "time") == 0)
			{
				time = strtoul(param->value().c_str(), NULL, 10);
			}
			DebugPrint("press.fn GET[%s]: %s\r\n",
				param->name().c_str(),
				param->value().c_str());
        }

        request->send(200, "text/plain", "OK");

		if (time > 0)
		{
        	DebugPrint("press.fn pcpower_press(%d)...\r\n", time);
			pcpower_press(time);
        }
    });

	//led
	server->on("/led.fn", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", pcpower_getled() ? "ON": "OFF");
    });
}

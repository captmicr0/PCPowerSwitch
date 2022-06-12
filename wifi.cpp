#include "wifi.h"

static Settings *_settings = NULL;

static IPAddress _ipaddress;
static IPAddress _gateway;
static IPAddress _subnet;
static IPAddress _dns;
static IPAddress _altdns;

static bool _wifi_mdns = false;
static volatile bool _wifimanager_reboot = false;

AsyncWebServer *_server = NULL;

const char *encMap[] = {
    #if defined(ESP32)
    "OPEN",
    "WEP",
    "WPA_PSK",
    "WPA2_PSK",
    "WPA_WPA2_PSK",
    "WPA2_ENTERPRISE",
    "MAX"
    #elif defined(ESP8266)
    "???",
    "???",
    "TKIP [WPA_PSK]",
    "???",
    "CCMP [WPA2_PSK]",
    "WEP [WEP]",
    "???",
    "NONE [OPEN]",
    "AUTO [WPA_WPA2_PSK]",
    #endif
};

bool wifi_setup(Settings *settings, wifi_pagesFn pagesFn)
{
    //make sure settings pointer is valid
	if (settings == NULL) return false;

    //store pointer to settings
    _settings = settings;

    //make sure settings pointer is valid
	if (_settings == NULL) return false;

    //initialize LittleFS
    if (!LittleFS.begin()) return false;

    //initialize AsyncWebServer
    _server = new AsyncWebServer(80);
    if (_server == NULL) return false;

    //setup WiFi
    bool wifiSTA = wifi_initSTA();

    if (!wifiSTA)
    {
        //failed to connect, create softAP
        if (!WiFi.softAP(WIFIMANAGER_SOFTAPSSID, NULL)) return false;
    }

    //print SSID & IP
    DebugPrint(
        "SSID: %s\r\n"
        "  IP: %s\r\n",
        (wifiSTA ? WiFi.SSID() : WiFi.softAPSSID()).c_str(),
        (wifiSTA ? WiFi.localIP() : WiFi.softAPIP()).toString().c_str()
    );

    //setup server pages
    wifi_initPages(_server, !wifiSTA);

    if (pagesFn != NULL)
    {
        pagesFn(_server, !wifiSTA);
    }

    //start server
    _server->begin();

    //start mDNS responder
    if (strlen(_settings->wifi.mdns) > 0)
    {
        _wifi_mdns = true;
        MDNS.begin(_settings->wifi.mdns);
    }

    return true;
}

void wifi_loop(void)
{
    if (_wifi_mdns)
    {
        MDNS.update();
    }

    if (_wifimanager_reboot)
    {
        _wifimanager_reboot = false;
        delay(WIFIMANAGER_REBOOT_WAIT);
        ESP.restart();
    }
}

void wifi_initPages(AsyncWebServer *server, bool wifimanager)
{
    if (wifimanager)
    {
        //redirect to wifimanager.html
        server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
            request->redirect("/wifimanager.html");
        });
    }
    else
    {
        //redirect to index.html
        server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
            request->redirect("/index.html");
        });
    }

    //favicon.png
    server->on("/favicon.png", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/favicon.png", "image/png");
    });

    //style.css
    server->on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/style.css", "text/css");
    });

    //index.html
    server->on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/index.html", "text/html");
    });

    //wifimanager.html
    server->on("/wifimanager.html", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/wifimanager.html", "text/html");
    });

    //wifimanager.cfg GET
    server->on("/wifimanager.cfg", HTTP_GET, [](AsyncWebServerRequest *request){
        AsyncWebParameter *param = request->getParam("name");
#define wifimgrcfgGETparam(paramname)                                   \
        if (strcmp(param->value().c_str(), #paramname) == 0)            \
        {                                                               \
            request->send(200, "text/plain", _settings->wifi.paramname);\
        }

        wifimgrcfgGETparam(ssid);
        wifimgrcfgGETparam(password);
        wifimgrcfgGETparam(ipaddress);
        wifimgrcfgGETparam(gateway);
        wifimgrcfgGETparam(subnet);
        wifimgrcfgGETparam(dns);
        wifimgrcfgGETparam(altdns);
        wifimgrcfgGETparam(mdns);
    });

    //wifimanager.cfg POST
    server->on("/wifimanager.cfg", HTTP_POST, [](AsyncWebServerRequest *request){
        int paramcnt = request->params();
        for (int i = 0; i < paramcnt; i++)
        {
            AsyncWebParameter *param = request->getParam(i);
            if (param->isPost())
            {
#define wifimgrcfgPOSTparam(paramname)                                          \
                if (strcmp(param->name().c_str(), #paramname) == 0)             \
                {                                                               \
                    int paramsz = strlen(param->value().c_str());               \
                    size_t membersz = sizeof_member(WiFiData, paramname) - 1;   \
                    memset(_settings->wifi.paramname, 0, membersz + 1);         \
                    strncpy(_settings->wifi.paramname, param->value().c_str(),  \
                        (paramsz <= membersz) ? paramsz : membersz);            \
                }

                wifimgrcfgPOSTparam(ssid);
                wifimgrcfgPOSTparam(password);
                wifimgrcfgPOSTparam(ipaddress);
                wifimgrcfgPOSTparam(gateway);
                wifimgrcfgPOSTparam(subnet);
                wifimgrcfgPOSTparam(dns);
                wifimgrcfgPOSTparam(altdns);
                wifimgrcfgPOSTparam(mdns);

                DebugPrint("wifimanager.html POST[%s]: %s\r\n",
                    param->name().c_str(),
                    param->value().c_str());
            }
        }

        request->send(200, "text/plain", "Saving & Rebooting...");

        DebugPrint("wifimanager.html saving settings...\r\n");
        settings_save(_settings);

        DebugPrint("wifimanager.html _wifimanager_reboot = true...\r\n");
        _wifimanager_reboot = true;
    });
}

bool wifi_initSTA(void)
{
    //make sure settings pointer is valid
	if (_settings == NULL) return false;

    bool has_ssid = (strlen(_settings->wifi.ssid) > 0);
    bool has_password = (strlen(_settings->wifi.password) > 0);
    bool has_ipaddress = (strlen(_settings->wifi.ipaddress) > 0);
    bool has_gateway = (strlen(_settings->wifi.gateway) > 0);
    bool has_subnet = (strlen(_settings->wifi.subnet) > 0);
    bool has_dns = (strlen(_settings->wifi.dns) > 0);
    bool has_altdns = (strlen(_settings->wifi.altdns) > 0);

    if (!has_ssid) return false;

    //load IPs
    _ipaddress.fromString(_settings->wifi.ipaddress);
    _gateway.fromString(_settings->wifi.gateway);
    _subnet.fromString(_settings->wifi.subnet);
    _dns.fromString(_settings->wifi.dns);
    _altdns.fromString(_settings->wifi.altdns);

    //station mode
    WiFi.mode(WIFI_STA);
    
    //configure
    bool config = WiFi.config(
        (has_ipaddress && has_gateway && has_subnet) ? _ipaddress : (IPAddress)0,
        (has_ipaddress && has_gateway && has_subnet) ? _gateway : (IPAddress)0,
        (has_ipaddress && has_gateway && has_subnet) ? _subnet : (IPAddress)0,
        (has_dns) ? _dns : (IPAddress)0,
        (has_dns && has_altdns) ? _altdns : (IPAddress)0
    );
    if (!config) return false;

    //connect
    WiFi.begin(_settings->wifi.ssid, has_password ? _settings->wifi.password : 0);

    unsigned long prev = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        yield(); //fixes soft WDT reset / crashing
        if ((millis() - prev) > WIFI_CONNECT_TIMEOUT) return false;
    }

    return true;
}

void wifi_scanSTA(void)
{
    DebugPrint("wifi_scan\r\n");

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    DebugPrint("scanning...");
    int n = WiFi.scanNetworks();
    DebugPrint(" done.\r\n");

    DebugPrint("%d networks found\r\n", n);
    
    for (int i = 0; i < n; i++)
    {
        uint8_t encType = WiFi.encryptionType(i);
        if (encType == -1) encType = 0;
        DebugPrint(
            "[%02d] %-30s: (%d) (%s)\r\n",
            i,
            WiFi.SSID(i).c_str(),
            WiFi.RSSI(i),
            encMap[encType]);
    }
    
    //WiFi.scanDelete();
    DebugPrint("done\r\n\r\n");
}

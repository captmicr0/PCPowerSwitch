#include "settings.h"

static Settings _lastdata;

bool settings_save(Settings *data)
{
    if (LittleFS.begin())
    {
        if (LittleFS.exists(SETTINGS_FILE))
        {
            //if file already exists, only save if data was changed
            if (settings_compare(data, &_lastdata)) return false;
        }
    }
    
    //write
    data->_magic1 = 0xDE; data->_magic2 = 0xAD; //set magic

    uint8_t *bytes = (uint8_t*)data;
    if (LittleFS.begin())
    {
        File f = LittleFS.open(SETTINGS_FILE, "w");
        if (f)
        {
            f.seek(0);
            f.write(bytes, SETTINGS_SIZE);
            f.close();
        } else { return false; }
    } else { return false; }

    //store data for compare
    _lastdata = *data;

    return true;
}

bool settings_load(Settings *data)
{
    //buffer to read into
    uint8_t bytes[SETTINGS_SIZE];

    //read
    if (LittleFS.begin())
    {
        if (LittleFS.exists(SETTINGS_FILE))
        {
            File f = LittleFS.open(SETTINGS_FILE, "r");
            if (f)
            {
                f.seek(0);
                f.read((uint8_t*)&bytes, SETTINGS_SIZE);
                f.close();
            } else { return false; }
        } else { return false; }
    } else { return false; }

    //only copy if magic is valid
    Settings *temp = (Settings*)bytes; //cast to struct
    if ((temp->_magic1 == 0xDE) && (temp->_magic2 == 0xAD)) //check magic
    {
        memcpy(data, temp, SETTINGS_SIZE);
        return true;
    }

    return false;
}

void settings_loop(Settings *data)
{
    #ifdef SETTINGS_SAVELOOP
    
    static unsigned long prev = millis();
	unsigned long curr = millis();

	if ((curr - prev) > SETTINGS_SAVE_INTERVAL)
	{
        if (data != NULL)
        {
            settings_save(data);
        }
        prev = curr;
	}

    #endif // SETTINGS_SAVELOOP
}

bool settings_compare(Settings *data1, Settings *data2)
{
    if ((data1 == NULL) || (data2 == NULL)) return false;
    
    //WiFiData
    if (strcmp(data1->wifi.ssid, data2->wifi.ssid) != 0) return false;
    if (strcmp(data1->wifi.password, data2->wifi.password) != 0) return false;
    if (strcmp(data1->wifi.ipaddress, data2->wifi.ipaddress) != 0) return false;
    if (strcmp(data1->wifi.gateway, data2->wifi.gateway) != 0) return false;
    if (strcmp(data1->wifi.subnet, data2->wifi.subnet) != 0) return false;
    if (strcmp(data1->wifi.dns, data2->wifi.dns) != 0) return false;
    if (strcmp(data1->wifi.altdns, data2->wifi.altdns) != 0) return false;
    if (strcmp(data1->wifi.mdns, data2->wifi.mdns) != 0) return false;

    return true;
}

void settings_print(Settings *data)
{
	DebugPrint(
        "settings:\r\n"
        "\t_magic1: %02X\r\n"
        "\t_magic2: %02X\r\n"

		"\tWiFiData:\r\n"
		"\t\t     ssid: %s\r\n"
		"\t\t password: %s\r\n"
		"\t\tipaddress: %s\r\n"
		"\t\t  gateway: %s\r\n"
		"\t\t   subnet: %s\r\n"
		"\t\t      dns: %s\r\n"
		"\t\t   altdns: %s\r\n"
		"\t\t     mdns: %s\r\n"
		"\r\n",
        
        data->_magic1,
        data->_magic2,

		data->wifi.ssid,
		data->wifi.password,
		data->wifi.ipaddress,
		data->wifi.gateway,
		data->wifi.subnet,
		data->wifi.dns,
		data->wifi.altdns,
		data->wifi.mdns
	);
}
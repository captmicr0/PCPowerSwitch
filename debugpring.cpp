#include "DebugPrint.h"

void DebugPrint(char *format, ...)
{
    #if defined(DEBUGPRINT_ENABLE)
    
    static bool first_call = true;
    if (first_call)
    {
        #if defined(ESP32)
        while (!Serial) delay(1);
        #endif // ESP32
        Serial.begin(SERIAL_BAUDRATE);
        #if defined(ESP8266)
        delay(1000);
        #endif // ESP8266
        first_call = false;
    }

    va_list args;
    char buffer[512];

    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);

    Serial.print(buffer);

    #endif // DEBUGPRINT_ENABLE
}

#include "PinMap.h"

void pinmap_print(char *def, bool available, int gpio)
{
    char gpiobuf[8] = "??";

    if (gpio >= 0) {
        gpiobuf[0] = '\0';
        sprintf(gpiobuf, "%02d", gpio);
    }

    DebugPrint(
        "%s%s available [GPIO%s]\r\n",
        def,
        (available)?"":" not",
        gpiobuf
    );
}

void pinmap_check(void)
{
    DebugPrint("====[PinMap check]==============================================================\r\n");

    //ALIVE_LED
    #if defined(ALIVE_LED)
    pinmap_print("ALIVE_LED", true, ALIVE_LED);
    #else
    pinmap_print("ALIVE_LED", false);
    #endif
    
    //PC_POWER_SW
    #if defined(PCPOWER_SW)
    pinmap_print("PCPOWER_SW", true, PCPOWER_SW);
    #else
    pinmap_print("PCPOWER_SW", false);
    #endif

    //PC_POWER_LED
    #if defined(PCPOWER_LED)
    pinmap_print("PCPOWER_LED", true, PCPOWER_LED);
    #else
    pinmap_print("PCPOWER_LED", false);
    #endif

    DebugPrint("================================================================================\r\n\r\n");
}

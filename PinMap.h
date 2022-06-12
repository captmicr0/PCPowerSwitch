#ifndef _PINMAP_H_
#define _PINMAP_H_

#include <Arduino.h>

#include "DebugPrint.h"

void pinmap_print(char *def, bool available, int gpio=-1);
void pinmap_check(void);

// Alive LED
#define ALIVE_LED       LED_BUILTIN
#define ALIVE_LED_ON    0
#define ALIVE_LED_OFF   1

// Tied to the PC power switch
#define PCPOWER_SW     14

// Tied to the PC power led
#define PCPOWER_LED    16

// Other
#define NULL 0

#endif // _PINMAP_H_
#ifndef _DEBUGPRINT_H_
#define _DEBUGPRINT_H_

#include <Arduino.h>

#define DEBUGPRINT_ENABLE
#define SERIAL_BAUDRATE 115200

void DebugPrint(char *format, ...);

#endif // _DEBUGPRINT_H_
#ifndef _PCPOWER_H_
#define _PCPOWER_H_

#include <Arduino.h>

#include "DebugPrint.h"
#include "PinMap.h"

bool pcpower_setup(void);
void pcpower_loop(void);
void pcpower_press(unsigned long time);
bool pcpower_getled(void);

#endif // _PCPOWER_H_

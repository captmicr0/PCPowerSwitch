#include "leds.h"

bool leds_setup(void)
{
	#if defined(ALIVE_LED)
	pinMode(ALIVE_LED, OUTPUT);
	digitalWrite(ALIVE_LED, ALIVE_LED_OFF);
	#endif // ALIVE_LED

	return true;
}

void leds_loop(void)
{
	#if defined(ALIVE_LED)
	static unsigned long prev = millis();
	unsigned long curr = millis();

	if ((curr - prev) > ALIVE_LED_INTERVAL)
	{
		digitalWrite(ALIVE_LED, !digitalRead(ALIVE_LED));
		prev = curr;
	}
	#endif // ALIVE_LED
}


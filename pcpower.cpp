#include "pcpower.h"

static volatile unsigned long _pcpower_press_time = 0;

bool pcpower_setup(void)
{
    #if defined(PCPOWER_SW)
    pinMode(PCPOWER_SW, INPUT);
    #endif //PCPOWER_SW

    #if defined(PCPOWER_LED)
    //pinMode(PCPOWER_LED, INPUT_PULLDOWN_16); //there is an external pull-down now
    pinMode(PCPOWER_LED, INPUT);
    #endif // PCPOWER_LED

    return true;
}

void pcpower_loop(void)
{
    static bool pressed = false;
    static unsigned long pressedat = 0;

    if (_pcpower_press_time > 0)
    {
        if (pressedat == 0)
        {
            //change to output and pull down to simulate press
            #if defined(PCPOWER_SW)
            pinMode(PCPOWER_SW, OUTPUT);
            digitalWrite(PCPOWER_SW, LOW);
            #endif //PCPOWER_SW
            
            pressedat = millis();
        }
        else
        {
            unsigned long curr = millis();

            if ((curr - pressedat) > _pcpower_press_time)
            {
                //change to input to release
                #if defined(PCPOWER_SW)
                pinMode(PCPOWER_SW, INPUT);
                #endif //PCPOWER_SW

                _pcpower_press_time = 0;
                pressedat = 0;
            }
        }
    }
}

void pcpower_press(unsigned long time)
{
    if (_pcpower_press_time == 0)
    {
        _pcpower_press_time = time;
    }
}

bool pcpower_getled(void)
{
    #if defined(PCPOWER_LED)
    return (digitalRead(PCPOWER_LED) == HIGH) ? true : false;
    #else
    return false;
    #endif  // PCPOWER_LED
}

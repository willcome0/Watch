#include "key.h"

#define PIN_NUM_BAT_CHAR  15
#define PIN_NUM_BAT_STDBY 16

static void key_init(void)
{
    digitalWrite(PIN_NUM_BAT_CHAR, HIGH);
    digitalWrite(PIN_NUM_BAT_STDBY, HIGH);

    pinMode(PIN_NUM_BAT_CHAR, INPUT);
    pinMode(PIN_NUM_BAT_STDBY, INPUT);
}

void task_key(void *pvParameters)
{
    (void)pvParameters;
    key_init();
    for (;;)
    {
        if (digitalRead(PIN_NUM_BAT_CHAR) == LOW)
        {
            vTaskDelay(10);
            if (digitalRead(PIN_NUM_BAT_CHAR) == LOW)
            Serial.print(1, DEC);
        }
        if (digitalRead(PIN_NUM_BAT_STDBY) == LOW)
        {
            vTaskDelay(10);
            if (digitalRead(PIN_NUM_BAT_STDBY) == LOW)
            Serial.print(2, DEC);
        }
    }
}
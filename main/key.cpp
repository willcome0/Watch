#include "key.h"
#include "ui.h"
#include "led.h"

#define PIN_NUM_KEY  34
#define PIN_NUM_BAT_CHAR  15
#define PIN_NUM_BAT_STDBY 16


static void key_init(void)
{
    digitalWrite(PIN_NUM_KEY, HIGH);
    digitalWrite(PIN_NUM_BAT_CHAR, HIGH);
    digitalWrite(PIN_NUM_BAT_STDBY, HIGH);
    
    pinMode(PIN_NUM_KEY, INPUT);
    pinMode(PIN_NUM_BAT_CHAR, INPUT);
    pinMode(PIN_NUM_BAT_STDBY, INPUT);
}

void task_key(void *pvParameters)
{
    (void)pvParameters;
    key_init();
    for (;;)
    {
        vTaskDelay(50);
        if (digitalRead(PIN_NUM_KEY) == LOW)
        {
            set_motor(1);
            vTaskDelay(10);
            if (digitalRead(PIN_NUM_KEY) == LOW)
            {
                g_main_ui_case++;
                Serial.printf("按键按下");
                vTaskDelay(100);
                set_motor(0);
                while (digitalRead(PIN_NUM_KEY) == LOW);
            }
        }
        vTaskDelay(50);
        set_motor(0);

        if (digitalRead(PIN_NUM_BAT_CHAR) == LOW)
        {
            vTaskDelay(10);
            if (digitalRead(PIN_NUM_BAT_CHAR) == LOW)
            {
                g_bat_charge_flag = 1;
            }
        }
        else
        {
            g_bat_charge_flag = 0;
        }
            

        // if (digitalRead(PIN_NUM_BAT_STDBY) == LOW)
        // {
        //     vTaskDelay(10);
        //     if (digitalRead(PIN_NUM_BAT_STDBY) == LOW)
        //         Serial.printf("充满");
        // }
    }
}
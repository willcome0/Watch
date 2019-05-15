#include "task.h"
#include "Arduino.h"

#include "led.h"
#include "adc.h"
#include "key.h"
#include "ui.h"

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

struct Date_Time g_date_time = {19, 5, 16, 14, 01, 02, 4};

void task_data_time(void *pvParameters)
{
    (void)pvParameters;
    for (;;)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        if (++g_date_time.sec == 60) // 秒
        {
            g_date_time.sec = 0;
            if (++g_date_time.minute == 60) // 分
            {
                g_date_time.minute = 0;
                if (++g_date_time.hour == 24) // 时
                {
                    g_date_time.hour = 0;
                    if (++g_date_time.day == 31) // 日
                    {
                        g_date_time.day = 1;
                        if (++g_date_time.month == 12) // 月
                        {
                            g_date_time.month = 1;
                            g_date_time.year++;
                        }
                    }
                }
            }
        }
        Serial.printf("20%02d年%02d月%02d日 %02d时%02d分%02d秒\r\n", 
                                    g_date_time.year, g_date_time.month, g_date_time.day,
                                    g_date_time.hour, g_date_time.minute, g_date_time.sec);
    }
}

void creat_task(void)
{
    xTaskCreatePinnedToCore(
        task_data_time,
        "task_data_time",
        1024,
        NULL, configMAX_PRIORITIES - 1,
        NULL, ARDUINO_RUNNING_CORE);

    xTaskCreatePinnedToCore(
        task_led,
        "task_led",
        1024,
        NULL, 2,
        NULL, ARDUINO_RUNNING_CORE);

    xTaskCreatePinnedToCore(
        task_bat_adc,
        "task_bat_adc",
        2048,
        NULL, 2, // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        NULL, ARDUINO_RUNNING_CORE);
        
        xTaskCreatePinnedToCore(
        task_key,
        "task_key",
        2048,
        NULL, configMAX_PRIORITIES - 2, // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        NULL, ARDUINO_RUNNING_CORE);

        xTaskCreatePinnedToCore(
        task_ui,
        "task_ui",
        2048,
        NULL, 2,
        NULL, ARDUINO_RUNNING_CORE);
}

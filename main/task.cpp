#include "task.h"
#include "Arduino.h"

#include "led.h"
#include "adc.h"
#include "key.h"

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif




void creat_task(void)
{
    xTaskCreatePinnedToCore(
        task_led,
        "task_led",
        1024,
        NULL, 2, // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        NULL, ARDUINO_RUNNING_CORE);

    // xTaskCreatePinnedToCore(
    //     task_bat_adc,
    //     "task_bat_adc",
    //     1024,
    //     NULL, 2, // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    //     NULL, ARDUINO_RUNNING_CORE);
        
        xTaskCreatePinnedToCore(
        task_key,
        "task_key",
        1024,
        NULL, 2, // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        NULL, ARDUINO_RUNNING_CORE);
}

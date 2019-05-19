#ifndef __UI_H
#define __UI_H

#include "stdlib.h"
#include "Arduino.h"

#ifdef __cplusplus
 extern "C" {
#endif

extern int16_t g_main_ui_case;
extern uint8_t g_bat_charge_flag;
extern uint8_t g_wifi_connect_status;

void task_ui(void *pvParameters);





#ifdef __cplusplus
}
#endif

#endif

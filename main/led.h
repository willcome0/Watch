#ifndef __LED_H
#define __LED_H

#include "stdlib.h"
#include "Arduino.h"

#ifdef __cplusplus
 extern "C" {
#endif


void led_init(void);
void task_led(void *pvParameters);


#ifdef __cplusplus
}
#endif

#endif

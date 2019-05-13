#ifndef __LED_H
#define __LED_H

#include "stdlib.h"
#include "Arduino.h"

#ifdef __cplusplus
 extern "C" {
#endif


void led_init(void);
void rgb_set(uint8_t r_value, uint8_t g_value, uint8_t b_value, uint8_t bright_value);

void task_led(void *pvParameters);


#ifdef __cplusplus
}
#endif

#endif

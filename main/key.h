#ifndef __KEY_H
#define __KEY_H

#include "stdlib.h"
#include "Arduino.h"

#ifdef __cplusplus
 extern "C" {
#endif

void key_init(void);
void task_key(void *pvParameters);


#ifdef __cplusplus
}
#endif

#endif

#ifndef __BLINKER_H
#define __BLINKER_H

#include "stdlib.h"
#include "Arduino.h"

#ifdef __cplusplus
 extern "C" {
#endif


void blinker_init(void);
void blinker_run(void);

#ifdef __cplusplus
}
#endif

#endif

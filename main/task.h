#ifndef __TASK_H
#define __TASK_H

#include "stdlib.h"
#include "Arduino.h"

#ifdef __cplusplus
 extern "C" {
#endif

struct Date_Time
{
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t sec;
    uint8_t week;
};

extern struct Date_Time g_date_time;

void creat_task(void);

#ifdef __cplusplus
}
#endif

#endif

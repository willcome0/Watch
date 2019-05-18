#ifndef __FT6236_H
#define __FT6236_H

#include "stdlib.h"
#include <Arduino.h>


#ifdef __cplusplus
extern "C"
{
#endif

    struct TouchLocation
    {
        uint16_t x;
        uint16_t y;
    };
    struct TouchEvent
    {
        uint8_t type;
        TouchLocation location;
    };

    void iic_init(void);

    void ft6236_init(void);
    struct TouchEvent current_touch_event();
    void write_touch_register(uint8_t reg, uint8_t val);
    uint8_t read_touch_register(uint8_t reg);
    uint8_t read_touch_addr(uint8_t regAddr, uint8_t *pBuf, uint8_t len);
    uint8_t read_touch_location(struct TouchLocation *pLoc, uint8_t num);

    void task_touch(void *pvParameters);
    
#ifdef __cplusplus
}
#endif

#endif

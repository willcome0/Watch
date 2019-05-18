#include "ft6236.h"
#include "Wire.h"

#define PIN_NUM_SDA 21
#define PIN_NUM_SCL 22
#define IIC_FREQ 200000

#define FT6236_ADDR 0x38

const int MPU6050_addr = 0x68;
int16_t AccX, AccY, AccZ, Temp, GyroX, GyroY, GyroZ;

void iic_init(void)
{
    Wire.begin(PIN_NUM_SDA, PIN_NUM_SCL, IIC_FREQ);
}

void ft6236_init(void)
{
    write_touch_register(0, 0);       // device mode = Normal
    write_touch_register(0xA4, 0x00); // Interrupt polling mode

    // Wire.beginTransmission(0x68);
    // Wire.write(0x6B);
    // Wire.write(0);
    // Wire.endTransmission(true);
}

void task_touch(void *pvParameters)
{
    (void)pvParameters;

    for (;;)
    {
        vTaskDelay(50);

        struct TouchLocation tp[10];
        read_touch_location(tp, 1);

        // // 6050??
        // Wire.beginTransmission(0x68);
        // Wire.write(0x3B);
        // Wire.endTransmission(true);
        // Wire.requestFrom(0x68, 14, true);
        // AccX = Wire.read() << 8 | Wire.read();
        // AccY = Wire.read() << 8 | Wire.read();
        // AccZ = Wire.read() << 8 | Wire.read();
        // Temp = Wire.read() << 8 | Wire.read();
        // GyroX = Wire.read() << 8 | Wire.read();
        // GyroY = Wire.read() << 8 | Wire.read();
        // GyroZ = Wire.read() << 8 | Wire.read();

        Serial.printf("\r\nx: %d  y: %d", tp[0].x, tp[0].y); 
        // Serial.printf("\r\temp: %f", Temp / 340.00 + 36.53);
        // Serial.printf("\r\nax:%d ay:%d az:%d", AccX, AccY, AccZ);
        // Serial.printf("\r\ngx:%d gy:%d gz:%d\r\n", GyroX, GyroY, GyroZ);
    }
}
struct TouchEvent current_touch_event()
{
    // TouchEvent current;
    // current.type = TouchIDLE;
    // attention = digitalRead(_irq);

    // if (!attention && oldAttention)
    // {
    //     uint8_t count = readFT6236TouchLocation(touchLocations, 1);
    //     if (count)
    //     {
    //         /*for (uint8_t i = 0; i < count; i++) {} Ignore multitouch */
    //         current.type = TouchSTART;
    //         current.location.x = touchLocations[0].y;
    //         current.location.y = 320 - touchLocations[0].x;
    //     }
    //     else
    //     {
    //     }
    // }

    // oldAttention = attention;
    // return current;
}

void write_touch_register(uint8_t reg, uint8_t val)
{
    Wire.beginTransmission(FT6236_ADDR);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
}

uint8_t read_touch_register(uint8_t reg)
{
    Wire.beginTransmission(FT6236_ADDR);
    Wire.write(reg); // register 0
    uint8_t retVal = Wire.endTransmission();

    Wire.requestFrom(FT6236_ADDR, uint8_t(1)); // request 6 uint8_ts from slave device #2

    if (Wire.available())
    {
        retVal = Wire.read();
    }
    return retVal;
}

uint8_t read_touch_addr(uint8_t regAddr, uint8_t *pBuf, uint8_t len)
{
    Wire.beginTransmission(FT6236_ADDR);
    Wire.write(regAddr); // register 0
    Wire.endTransmission();

    Wire.requestFrom(FT6236_ADDR, len); // request 1 bytes from slave device #2

    uint8_t i;
    for (i = 0; (i < len) && Wire.available(); i++)
    {
        pBuf[i] = Wire.read();
    }

    return i;
}

uint8_t read_touch_location(struct TouchLocation *pLoc, uint8_t num)
{
    uint8_t retVal = 0;
    uint8_t i;
    uint8_t k;

    do
    {
        if (!pLoc)
            break; // must have a buffer
        if (!num)
            break; // must be able to take at least one

        uint8_t status = read_touch_register(2);

        static uint8_t tbuf[40];

        if ((status & 0x0f) == 0)
        {
            pLoc[k].x = 0;
            pLoc[k].y = 0;
            break; // no points detected
        }
            

        uint8_t hitPoints = status & 0x0f;

        read_touch_addr(0x03, tbuf, hitPoints * 6);

        for (k = 0, i = 0; (i < hitPoints * 6) && (k < num); k++, i += 6)
        {
            pLoc[k].x = (tbuf[i + 0] & 0x0f) << 8 | tbuf[i + 1];
            pLoc[k].y = (tbuf[i + 2] & 0x0f) << 8 | tbuf[i + 3];
        }

        retVal = k;

    } while (0);

    return retVal;
}


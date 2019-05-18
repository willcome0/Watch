// #include <Wire.h>
// const int MPU6050_addr = 0x68;
// int16_t AccX, AccY, AccZ, Temp, GyroX, GyroY, GyroZ;
// void setup()
// {
//     Wire.begin();
//     Wire.beginTransmission(MPU6050_addr);
//     Wire.write(0x6B);
//     Wire.write(0);
//     Wire.endTransmission(true);
//     Serial.begin(115200);
// }
// void loop()
// {
//     Wire.beginTransmission(MPU6050_addr);
//     Wire.write(0x3B);
//     Wire.endTransmission(false);
//     Wire.requestFrom(MPU6050_addr, 14, true);
//     AccX = Wire.read() << 8 | Wire.read();
//     AccY = Wire.read() << 8 | Wire.read();
//     AccZ = Wire.read() << 8 | Wire.read();
//     Temp = Wire.read() << 8 | Wire.read();
//     GyroX = Wire.read() << 8 | Wire.read();
//     GyroY = Wire.read() << 8 | Wire.read();
//     GyroZ = Wire.read() << 8 | Wire.read();
//     Serial.print("AccX = ");
//     Serial.print(AccX);
//     Serial.print(" || AccY = ");
//     Serial.print(AccY);
//     Serial.print(" || AccZ = ");
//     Serial.print(AccZ);
//     Serial.print(" || Temp = ");
//     Serial.print(Temp / 340.00 + 36.53);
//     Serial.print(" || GyroX = ");
//     Serial.print(GyroX);
//     Serial.print(" || GyroY = ");
//     Serial.print(GyroY);
//     Serial.print(" || GyroZ = ");
//     Serial.println(GyroZ);
//     delay(100);
// }

#define BLINKER_WIFI
#define BLINKER_ESP_SMARTCONFIG

#include <Blinker.h>
#include <string.h>
#include "lcd.h"
#include "task.h"
#include "led.h"


#include "led.h"
#include "adc.h"
#include "key.h"
#include "ui.h"
#include "ft6236.h"

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

char auth[] = "b914310a8ac1";
// char ssid[] = "will的iPhone";
// char pswd[] = "kangkang";

uint8_t RR, GG, BB, Bright;
uint8_t RR_Old, GG_Old, BB_Old;

// 新建组件对象
BlinkerBridge g_rgb_device("968276a8d249");
BlinkerButton g_rgb_button("rgb_button");
BlinkerRGB    g_rgb_led("rgb_led");

void blinker_callback(const String & data);

void rgb_device_callback(const String & data);
void rgb_button_callback(const String & state);
void rgb_led_callback(uint8_t r_value, uint8_t g_value, uint8_t b_value, uint8_t bright_value);

/*******************************/
void setup()
{
    Serial.begin(115200);
iic_init();
    ft6236_init();
    // creat_task();

    // BLINKER_DEBUG.stream(Serial);
    // Blinker.begin("b914310a8ac1");
    // Blinker.attachData(blinker_callback);

    // g_rgb_device.attach(rgb_device_callback);
    // g_rgb_button.attach(rgb_button_callback);
    // g_rgb_led.attach(rgb_led_callback);



    xTaskCreate(
        task_data_time,
        "task_data_time",
        1024,
        NULL, 3,
        NULL);

    // xTaskCreatePinnedToCore(
    //     task_key,
    //     "task_key",
    //     2048,
    //     NULL, 3, // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    //     NULL, ARDUINO_RUNNING_CORE);

    xTaskCreate(
        task_led,
        "task_led",
        1024,
        NULL, 1,
        NULL);

    xTaskCreate(
        task_ui,
        "task_ui",
        2048,
        NULL, 1,
        NULL);

    xTaskCreate(
        task_touch,
        "task_touch",
        2048,
        NULL, 2,
        NULL);
}

void loop()
{
        struct TouchLocation tp[10];
        read_touch_location(tp, 1);
        Serial.printf("\r\nxxxxx: %d  y: %d", tp[0].x, tp[0].y);
    // Blinker.run();
    // rgb_set(RR, GG, BB, Bright);
}

void blinker_callback(const String & data)
{
    // 不触发其他回调时也会来这
    BLINKER_LOG("设备回调", data);

    // must print Json data
    g_rgb_device.print("{\"hello\":\"bridge\"}");
}

void rgb_device_callback(const String & data)
{
    BLINKER_LOG("BridgeDevice1 readString: ", data);
}

void rgb_button_callback(const String & state)
{
    const String bnt_tap = "tap";
    static uint8_t rgb_on = 0;

    BLINKER_LOG("btn-ky按键回调", state);
    if (bnt_tap == state)
    {
        rgb_on = !rgb_on;
    }
    if (rgb_on)
    {
        RR = RR_Old;
        GG = GG_Old;
        BB = BB_Old;
        g_rgb_button.text("开", "开");
        g_rgb_button.print();
    }
    else
    {
        RR_Old = RR;
        GG_Old = GG;
        BB_Old = BB;
        RR = 0;
        GG = 0;
        BB = 0;
        g_rgb_button.text("关", "关");
        g_rgb_button.print();
    }
}

void rgb_led_callback(uint8_t r_value, uint8_t g_value, uint8_t b_value, uint8_t bright_value)
{
    RR = r_value;
    GG = g_value;
    BB = b_value;
    Bright = bright_value;
    BLINKER_LOG("R value: ", r_value);
    BLINKER_LOG("G value: ", g_value);
    BLINKER_LOG("B value: ", b_value);
    BLINKER_LOG("Rrightness value: ", bright_value);
}

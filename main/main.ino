#define BLINKER_WIFI
#define BLINKER_ESP_SMARTCONFIG

#include <Blinker.h>
#include <string.h>
#include "lcd.h"
#include "task.h"
#include "led.h"

uint8_t RR, GG, BB, Bright;
uint8_t RR_Old, GG_Old, BB_Old;

// 新建组件对象
BlinkerBridge g_rgb_device("94bc03e0c17c");
BlinkerButton g_rgb_button("rgb_button");
BlinkerRGB    g_rgb_led("rgb_led");

void blinker_callback(const String & data);

void rgb_device_callback(const String & data);
void rgb_button_callback(const String & state);
void rgb_led_callback(uint8_t r_value, uint8_t g_value, uint8_t b_value, uint8_t bright_value);




/*******************************/
void setup() {
    Serial.begin(115200);
    

    // lcd_init();
    // creat_task();
    led_init();

    BLINKER_DEBUG.stream(Serial);
    Blinker.begin("b914310a8ac1");
    Blinker.attachData(blinker_callback);

    g_rgb_device.attach(rgb_device_callback);
    g_rgb_button.attach(rgb_button_callback);
    g_rgb_led.attach(rgb_led_callback);
}

void loop()
{
    Blinker.run();
    rgb_set(RR, GG, BB, Bright);
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


#define BLINKER_WIFI
#define BLINKER_ESP_SMARTCONFIG

#include <Blinker.h>
#include "lcd.h"
#include "task.h"


char auth[] = "4844c3d968a6";
// char ssid[] = "RT-AC56U";
// char pswd[] = "kangkang";

#define RGB_R_PIN 23

// 新建组件对象
BlinkerButton Button1("btn-ky");
BlinkerBridge BridgeDevice1("94bc03e0c17c");


/************* 回调 *************/
void bridge1_callback(const String & data)
{
    BLINKER_LOG("BridgeDevice1 readString: ", data);    
}

void button1_callback(const String & state)
{
    BLINKER_LOG("btn-ky按键回调", state);
    digitalWrite(RGB_R_PIN, !digitalRead(RGB_R_PIN));
}

void dataRead(const String & data)
{
    // 不触发其他回调时也会来这
    BLINKER_LOG("设备回调", data);

    // must print Json data
    BridgeDevice1.print("{\"hello\":\"bridge\"}");
}

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;
void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}


/*******************************/
void setup() {
    Serial.begin(115200);
    BLINKER_DEBUG.stream(Serial);

    // lcd_init();
creat_task();

    // pinMode(RGB_R_PIN, OUTPUT);
    // digitalWrite(RGB_R_PIN, HIGH);

    /* 固定密码 */
    // Blinker.begin(auth, ssid, pswd);
    // Blinker.attachData(dataRead);
    /* 动态配网 */
    Blinker.begin(auth);
    Blinker.attachData(dataRead);

    BridgeDevice1.attach(bridge1_callback);
    Button1.attach(button1_callback);


    // lcd_fill(0, 0, 100, 100, BLUE);
    // lcd_draw_line(100, 100, 200, 200, RED);
    // lcd_show_char(200, 200, WHITE, BLACK, 'K', 24, 1);
    // lcd_show_num(100, 100, WHITE, BLACK, 12345, 6, 24, '0', 1);
    // lcd_show_str(0, 0, WHITE, BLACK, "你好世界abcdABCD", 16, 1);

    /* 获取时间，完全不准 */
    // delay(5000);
    // configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    // printLocalTime();
}

void loop()
{
    Blinker.run();
}

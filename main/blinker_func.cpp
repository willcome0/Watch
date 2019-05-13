#include "blinker_func.h"
#include <Blinker.h>

char auth[] = "4844c3d968a6";

BlinkerButton Button1("btn-ky");
BlinkerBridge BridgeDevice1("94bc03e0c17c");

/************* 回调 *************/
static void bridge1_callback(const String & data)
{
    BLINKER_LOG("BridgeDevice1 readString: ", data);    
}

static void button1_callback(const String & state)
{
    BLINKER_LOG("btn-ky按键回调", state);
    digitalWrite(RGB_R_PIN, !digitalRead(RGB_R_PIN));
}

static void blinker_callback(const String & data)
{
    // 不触发其他回调时也会来这
    BLINKER_LOG("设备回调", data);

    // must print Json data
    BridgeDevice1.print("{\"hello\":\"bridge\"}");
}


void blinker_init(void)
{
    BLINKER_DEBUG.stream(Serial);

    Blinker.begin(auth);
    Blinker.attachData(blinker_callback);

    BridgeDevice1.attach(bridge1_callback);
    Button1.attach(button1_callback);
}

void blinker_run(void)
{
    Blinker.run();
}
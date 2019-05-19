#define BLINKER_WIFI
// #define BLINKER_ESP_SMARTCONFIG

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
#include "Wire.h"

#include <Ticker.h>
Ticker ticker_datatime;
Ticker ticker_iic;

void datatime_count(void)
{
    g_date_time.change_flag = 1;
    g_date_time.all_sec++;
    if (++g_date_time.sec == 60) // 秒
    {
        g_date_time.sec = 0;
        if (++g_date_time.minute == 60) // 分
        {
            g_date_time.minute = 0;
            if (++g_date_time.hour == 24) // 时
            {
                g_date_time.hour = 0;
                if (++g_date_time.day == 31) // 日
                {
                    g_date_time.day = 1;
                    if (++g_date_time.month == 12) // 月
                    {
                        g_date_time.month = 1;
                        g_date_time.year++;
                    }
                }
            }
        }
    }
    // Serial.printf("20%02d年%02d月%02d日 %02d时%02d分%02d秒\r\n",
    //                 g_date_time.year, g_date_time.month, g_date_time.day,
    //                 g_date_time.hour, g_date_time.minute, g_date_time.sec);
}

volatile uint8_t iic_get_flag = 0;
void iic_get(void)
{
    if (iic_get_flag==0)
        iic_get_flag = 1;
}


#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

char auth[] = "b914310a8ac1";
// char ssid[] = "will的iPhone";
char ssid[] = "kangkang";
char pswd[] = "kangkang";

uint8_t RR, GG, BB, Bright;
uint8_t RR_Old, GG_Old, BB_Old;

// 新建组件对象
BlinkerBridge g_rgb_device("968276a8d249");
BlinkerButton g_rgb_button("rgb_button");
BlinkerRGB g_rgb_led("rgb_led");

void blinker_callback(const String &data);
void heartbeat(void);

void rgb_device_callback(const String &data);
void rgb_button_callback(const String &state);
void rgb_led_callback(uint8_t r_value, uint8_t g_value, uint8_t b_value, uint8_t bright_value);

/*******************************/
void setup()
{
    Serial.begin(115200);
    ticker_datatime.attach_ms(1000, datatime_count);
    ticker_iic.attach_ms(30, iic_get);

    Wire.begin(21, 22, 200000);
    write_touch_register(0, 0);       // device mode = Normal
    write_touch_register(0xA4, 0x00); // Interrupt polling mode

    Wire.beginTransmission(0x68);
    Wire.write(0x6B);
    Wire.write(0);
    Wire.endTransmission(true);

    led_init();
    lcd_init();
    lcd_clear(BLACK);

    key_init();

    // creat_task();

    BLINKER_DEBUG.stream(Serial);
    Blinker.begin("b914310a8ac1", ssid, pswd);
    Blinker.attachData(blinker_callback);
    Blinker.attachHeartbeat(heartbeat);
    Blinker.setTimezone(8.0);
    

    // g_rgb_device.attach(rgb_device_callback);
    // g_rgb_button.attach(rgb_button_callback);
    // g_rgb_led.attach(rgb_led_callback);
}

static int16_t AccX = 0, AccY = 0, AccZ = 0, Temp = 0, GyroX = 0, GyroY = 0, GyroZ = 0;
static int16_t o_AccX = 0, o_AccY = 0, o_AccZ = 0, o_Temp = 0, o_GyroX = 0, o_GyroY = 0, o_GyroZ = 0;
static int16_t leda_flag = 1;
static uint32_t first_static_time = 0;

static uint16_t key_pressed = 0;
void loop()
{
    Blinker.run();
    static uint8_t net_time_flag = 0;
    if (Blinker.status() == WL_CONNECTED && net_time_flag == 0)
    {
        net_time_flag = 1;
        heartbeat();
    }
    struct TouchLocation tp[20];
    if (leda_flag && iic_get_flag)
    {
        iic_get_flag = 0;
        first_static_time = 0;

        for (uint8_t i=1; i<20; i++)
        {
            tp[20-i] = tp[19-i];
        }
        read_touch_location(tp, 1);
        // tp[5] = tp[4];
        // tp[4] = tp[3];
        // tp[3] = tp[2];
        // tp[2] = tp[1];
        // tp[1] = tp[0];
        // read_touch_location(tp, 1);

         
        // 向上划，下一个菜单
        if (60 < tp[0].x && tp[0].x < 180 && tp[0].y < 60 &&
            tp[0].x != tp[1].x && tp[0].y != tp[1].y)
        {
            for (uint8_t i=0; i<10; i++)
            {
                if (60 < tp[1+i].x && tp[1+i].x < 180 && 60 < tp[1+i].y && tp[1+i].y < 180 &&
                    tp[2+i].x == 0 && tp[2+i].y == 0)
                {
                    set_motor(1);
                    if (g_main_ui_case < 10)// 主菜单才能上下
                        g_main_ui_case++;
                    memset(tp, 0, sizeof(tp));
                    break;
                }
            }
            // if ((60 < tp[2].x && tp[2].x < 180 && 60 < tp[2].y && tp[2].y < 180 &&
            //     tp[3].x == 0 && tp[3].y == 0) ||
            //     (60 < tp[3].x && tp[3].x < 180 && 60 < tp[3].y && tp[3].y < 180 &&
            //     tp[4].x == 0 && tp[4].y == 0) ||
            //     (60 < tp[4].x && tp[4].x < 180 && 60 < tp[4].y && tp[4].y < 180 &&
            //     tp[5].x == 0 && tp[5].y == 0))
            // {
            //     set_motor(1);
            //     if (g_main_ui_case < 10)// 主菜单才能上下
            //         g_main_ui_case++;
            //     memset(tp, 0, sizeof(tp));
            // }
        }

        // 向下划，上一个菜单
        if (60 < tp[0].x && tp[0].x < 180 && tp[0].y > 180 &&
            tp[0].x != tp[1].x && tp[0].y != tp[1].y)
        {
            for (uint8_t i=0; i<10; i++)
            {
                if (60 < tp[1+i].x && tp[1+i].x < 180 && 60 < tp[1+i].y && tp[1+i].y < 180 &&
                    tp[2+i].x == 0 && tp[2+i].y == 0)
                {
                    set_motor(1);
                    if (g_main_ui_case < 10)// 主菜单才能上下
                        g_main_ui_case--;
                    memset(tp, 0, sizeof(tp));
                    break;
                }
            }
            // if ((60 < tp[2].x && tp[2].x < 180 && 60 < tp[2].y && tp[2].y < 180 &&
            //     tp[3].x == 0 && tp[3].y == 0) ||
            //     (60 < tp[3].x && tp[3].x < 180 && 60 < tp[3].y && tp[3].y < 180 &&
            //     tp[4].x == 0 && tp[4].y == 0) ||
            //     (60 < tp[4].x && tp[4].x < 180 && 60 < tp[4].y && tp[4].y < 180 &&
            //     tp[5].x == 0 && tp[5].y == 0))
            // {
            //     set_motor(1);
            //     if (g_main_ui_case < 10)// 主菜单才能上下
            //         g_main_ui_case--;
            //     memset(tp, 0, sizeof(tp));
            // }
        }

        // // 返回
        // if (60 < tp[0].x && tp[0].x < 180 && 60 < tp[0].y && tp[0].y < 220 &&
        //     tp[0].x != tp[1].x && tp[0].y != tp[1].y)
        // {
        //     for (uint8_t i=0; i<10; i++)
        //     {
        //         if (60 < tp[1+i].x && tp[1+i].x < 180 && 60 < tp[1+i].y && tp[1+i].y < 180 &&
        //             tp[2+i].x == 0 && tp[2+i].y == 0)
        //         {
        //             set_motor(1);
        //             g_main_ui_case--;
        //             memset(tp, 0, sizeof(tp));
        //             break;
        //         }
        //     }

        //     // if ((60 < tp[2].y && tp[2].y < 220 && 50 > tp[2].x &&
        //     //     tp[3].x == 0 && tp[3].y == 0) ||
        //     //     (60 < tp[3].y && tp[3].y < 220 && 50 > tp[3].x &&
        //     //     tp[4].x == 0 && tp[4].y == 0) ||
        //     //     (60 < tp[4].y && tp[4].y < 220 && 50 > tp[4].x &&
        //     //     tp[5].x == 0 && tp[5].y == 0))
        //     // {
        //     //     set_motor(1);
        //     //     g_main_ui_case--;
        //     //     memset(tp, 0, sizeof(tp));
        //     // }
        // }

        // 向左划，进子菜单
        if (60 < tp[0].y && tp[0].y < 180 && tp[0].x < 60 &&
            tp[0].x != tp[1].x && tp[0].y != tp[1].y)
        {
            for (uint8_t i=0; i<10; i++)
            {
                if (60 < tp[1+i].x && tp[1+i].x < 180 && 60 < tp[1+i].y && tp[1+i].y < 180 &&
                    tp[2+i].x == 0 && tp[2+i].y == 0)
                {
                    set_motor(1);
                    if (g_main_ui_case < 20 && g_main_ui_case != 0)
                        g_main_ui_case += 10;
                    memset(tp, 0, sizeof(tp));
                    break;
                }
            }

            // if ((60 < tp[2].x && tp[2].x < 220 && 60 < tp[2].y && tp[2].y < 220 &&
            //     tp[3].x == 0 && tp[3].y == 0) ||
            //     (60 < tp[3].x && tp[3].x < 220 && 60 < tp[3].y && tp[3].y < 220 &&
            //     tp[4].x == 0 && tp[4].y == 0) ||
            //     (60 < tp[4].x && tp[4].x < 220 && 60 < tp[4].y && tp[4].y < 220 &&
            //     tp[5].x == 0 && tp[5].y == 0))
            // {
            //     set_motor(1);
            //     if (g_main_ui_case < 20 && g_main_ui_case != 0)
            //         g_main_ui_case += 10;
            //     memset(tp, 0, sizeof(tp));
            // }
        }


        // 向右划，返回上一层子菜单
        if (60 < tp[0].y && tp[0].y < 180 && tp[0].x > 180 &&
            tp[0].x != tp[1].x && tp[0].y != tp[1].y)
        {
            for (uint8_t i=0; i<10; i++)
            {
                if (60 < tp[1+i].x && tp[1+i].x < 180 && 60 < tp[1+i].y && tp[1+i].y < 180 &&
                    tp[2+i].x == 0 && tp[2+i].y == 0)
                {
                    set_motor(1);
                    if (g_main_ui_case>10)
                        g_main_ui_case -= 10;
                    memset(tp, 0, sizeof(tp));
                    break;
                }
            }
            // if ((60 < tp[2].x && tp[2].x < 220 && 60 < tp[2].y && tp[2].y < 220 &&
            //     tp[3].x == 0 && tp[3].y == 0) ||
            //     (60 < tp[3].x && tp[3].x < 220 && 60 < tp[3].y && tp[3].y < 220 &&
            //     tp[4].x == 0 && tp[4].y == 0) ||
            //     (60 < tp[4].x && tp[4].x < 220 && 60 < tp[4].y && tp[4].y < 220 &&
            //     tp[5].x == 0 && tp[5].y == 0))
            // {
            //     set_motor(1);
            //     if (g_main_ui_case>10)
            //         g_main_ui_case -= 10;
            //     memset(tp, 0, sizeof(tp));
            // }
        }

        // 主界面点击操作
        static uint16_t press_func_count = 0;
        if (g_main_ui_case < 10 && g_main_ui_case != 0)
        {
            if (tp[0].x != 0 && tp[0].y != 0 &&
                tp[0].x == tp[1].x && tp[0].y == tp[1].y &&
                // tp[0].x == tp[2].x && tp[0].y == tp[2].y && 
                60 < tp[0].x && tp[0].x < 180 && 
                60 < tp[0].y && tp[0].y < 180)
            {
                delay(30);
                press_func_count++;

                if (press_func_count == 2)
                {
                    set_motor(1);
                    g_main_ui_case += 10;
                    memset(tp, 0, sizeof(tp));
                }
            }
            else
                press_func_count = 0;
        }

        // 图标按键按下。返回表盘；已在表盘息屏
        static uint16_t press_main_count = 0;
        if (tp[0].x == 120 && tp[0].y == 320)
        {
            delay(30);
            press_main_count++;
            if (press_main_count == 2)
            {
                set_motor(1);
                if (g_main_ui_case == 0)
                {
                    ledcWrite(4, 0);
                    leda_flag = 0;
                }
                else
                    g_main_ui_case = 0;

                memset(tp, 0, sizeof(tp));
            }
        }
        else
            press_main_count = 0;

#ifdef DEBUG
        Serial.printf("\r\nx: %d  y: %d", tp[0].x, tp[0].y);
#endif
    } // 触摸采集、判断结束



#ifdef DEBUG
    Serial.printf("\r\n%d", g_main_ui_case);
#endif

    { // 采集姿态
        Wire.beginTransmission(0x68);
        Wire.write(0x3B);
        Wire.endTransmission(true);
        Wire.requestFrom(0x68, 14, true);

        o_AccX = AccX;
        o_AccY = AccY;
        o_AccZ = AccZ;
        o_GyroX = GyroX;
        o_GyroY = GyroY;
        o_GyroZ = GyroZ;

        AccX = Wire.read() << 8 | Wire.read();
        AccY = Wire.read() << 8 | Wire.read();
        AccZ = Wire.read() << 8 | Wire.read();
        Temp = Wire.read() << 8 | Wire.read();
        GyroX = Wire.read() << 8 | Wire.read();
        GyroY = Wire.read() << 8 | Wire.read();
        GyroZ = Wire.read() << 8 | Wire.read();
    }


    // Serial.printf("\r\temp: %f", Temp / 340.00 + 36.53);
    // Serial.printf("\r\nax:%d ay:%d az:%d gx:%d gy:%d gz:%d", AccX, AccY, AccZ, GyroX, GyroY, GyroZ);
    // Serial.printf("\r\ngx:%d gy:%d gz:%d\r\n", GyroX, GyroY, GyroZ);
    // Serial.println(AccX);   Serial.println(',');
    // Serial.println(AccY);   Serial.println(',');
    // Serial.println(AccZ);   Serial.println(',');
    // Serial.println(GyroX);  Serial.println(',');
    // Serial.println(GyroY);  Serial.println(',');
    // Serial.println(GyroZ);  Serial.println(',');



    if (tp[0].x == 0 && tp[0].y == 0)   // 触摸状态，重新计时
        first_static_time;
    if (leda_flag == 1) // 只有屏幕亮时才会判断静止
    {
        if (abs(o_AccX - AccX) < 300 && abs(o_AccY - AccY) < 300 && abs(o_AccZ - AccZ) < 300 &&
            abs(o_GyroX - GyroX) < 80 && (o_GyroY - GyroY) < 80 && (o_GyroZ - GyroZ) < 80)
        {
            if (first_static_time == 0)
            {
                first_static_time = g_date_time.all_sec;
            }
            else
            {
                if (g_date_time.all_sec - first_static_time > 3)
                {
                    ledcWrite(4, 0);
                    leda_flag = 0;
                }
            }
        }
    }


    // else
    // {
    //     first_static_time = 0;
    //     ledcWrite(4, 255);
    //     leda_flag = 1;
    // }

    static uint8_t old_main_ui_case = 99;
    static uint8_t old_bat_charge_flag = 99;
    static uint8_t reflash_flag = 0;

    // if (leda_flag == 0)
    {
        if (g_main_ui_case<10)
        {
            g_main_ui_case = g_main_ui_case % 5;
            g_main_ui_case = g_main_ui_case == -1 ? 4 : g_main_ui_case;
        }

        if (old_main_ui_case != g_main_ui_case)
        {
            old_main_ui_case = g_main_ui_case;
            reflash_flag = 1;
            lcd_clear(BLACK);
        }
        if (old_bat_charge_flag != g_bat_charge_flag || reflash_flag)
        {
            reflash_flag = 0;
            old_bat_charge_flag = g_bat_charge_flag;
            lcd_show_img_charge(g_bat_charge_flag);
        }

        // if (reflash_flag || (g_main_ui_case == 0 && g_date_time.change_flag==1))
        {
            g_date_time.change_flag = 0;
            lcd_show_img_bat(1);
            lcd_show_img_wifi(g_bat_charge_flag);

            char str[30] = {0};
            switch (g_main_ui_case)
            {
            case 0: // 表盘
                ui_show_dial();
                break;

            case 1: // 设备控制
                ui_show_device();
                break;

            case 2: // 游戏
                ui_show_game();
                break;

            case 3: // 设置
                ui_show_set();
                break;

            case 4: // 帮助
                ui_show_help();
                break;

            case 11: // 设备界面一
                lcd_show_str(90, 0, WHITE, BLACK, "设 备", 24, 1);
                lcd_show_img_device1(40, 80);
                lcd_show_str( 12, 140, WHITE, BLACK, "氛 围 灯", 24, 1);
                lcd_show_img_device2(148, 80);
                lcd_show_str(132, 140, WHITE, BLACK, "智能插排", 24, 1);
                break;

            case 21: // 设备界面二（敬请期待）
                lcd_show_str(90, 0, WHITE, BLACK, "设 备", 24, 1);
                lcd_show_img_wait();
                lcd_show_str(78, 200, WHITE, BLACK, "设 备 二", 24, 1);
                break;

            case 12: // 游戏界面一
                lcd_show_str(90, 0, WHITE, BLACK, "游 戏", 24, 1);
                lcd_show_img_wait();
                lcd_show_str(78, 200, WHITE, BLACK, "游 戏 一", 24, 1);
                break;

            case 22: // 游戏界面二（敬请期待）
                lcd_show_str(90, 0, WHITE, BLACK, "游 戏", 24, 1);
                lcd_show_img_wait();
                lcd_show_str(78, 200, WHITE, BLACK, "游 戏 二", 24, 1);
                break;

            case 13: // 设置界面一
                lcd_show_str(90, 0, WHITE, BLACK, "设 置", 24, 1);
                lcd_show_img_wait();
                lcd_show_str(78, 200, WHITE, BLACK, "设 置 一", 24, 1);
                break;

            case 23: // 设置界面二
                lcd_show_str(90, 0, WHITE, BLACK, "设 置", 24, 1);
                lcd_show_img_wait();
                lcd_show_str(78, 200, WHITE, BLACK, "设 置 二", 24, 1);
                break;

            case 14: // 关于界面一
                lcd_show_str(90, 0, WHITE, BLACK, "关 于", 24, 1);
                lcd_show_img_wait();
                lcd_show_str(78, 200, WHITE, BLACK, "关 于 一", 24, 1);
                break;

            case 24: // 关于界面二（敬请期待）
                lcd_show_str(90, 0, WHITE, BLACK, "关 于", 24, 1);
                lcd_show_img_wait();
                lcd_show_str(78, 200, WHITE, BLACK, "关 于 二", 24, 1);
                break;

            default: break;
            }
        }
    }

    // 按键及电机停止部分
    set_motor(0);
    if (digitalRead(34) == LOW)
    {
        if (key_pressed == 0)
            set_motor(1);
        vTaskDelay(50);
        if (digitalRead(34) == LOW)
        {
            if (key_pressed == 0)
            {
                if (leda_flag)
                {
                    ledcWrite(4, 0);
                    leda_flag = 0;
                }
                else
                {
                    ledcWrite(4, 255);
                    leda_flag = 1;
                    first_static_time = 0;
                }
                Serial.printf("按键按下");
            }

            if (key_pressed > 50) // 长按关机
            {
                ledcWrite(4, 255);
                leda_flag = 1;
                lcd_clear(BLACK);
                lcd_show_img_poweroff();

                while (1)
                {
                    read_touch_location(tp, 1);
                    if (tp[0].x == 120 && tp[0].y == 320)
                    {
                        set_motor(1);
                        delay(50);
                        set_motor(0);
                        // 关机操作
                    }
                    else if (tp[0].x != 0 && tp[0].y != 0)
                    { // 取消关机操作
                        set_motor(1);
                        delay(50);
                        set_motor(0);
                        key_pressed = 0;
                        reflash_flag = 1;
                    }
                }
            }
            key_pressed++;
        }
        else
            key_pressed = 0;
    }
    else
        key_pressed = 0;


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

void heartbeat(void)
{

    // 设置网络时间
    
    int8_t sec = Blinker.second();
    int8_t minute = Blinker.minute();
    int8_t hour = Blinker.hour();
    int8_t wday = Blinker.wday();
    int8_t mday = Blinker.mday();
    int8_t month = Blinker.month();
    int16_t year = Blinker.year();
    if (sec != -1 && minute != -1 && hour != -1 && wday != -1 && mday != -1 && month != -1 && year != -1)
    {
        g_date_time.year = year-2000;
        g_date_time.month = month;
        g_date_time.day = mday;
        g_date_time.hour = hour;
        g_date_time.minute = minute;
        g_date_time.sec = sec;
        g_date_time.week = wday;
    }

}

// void rgb_device_callback(const String & data)
// {
//     BLINKER_LOG("BridgeDevice1 readString: ", data);
// }

// void rgb_button_callback(const String & state)
// {
//     const String bnt_tap = "tap";
//     static uint8_t rgb_on = 0;

//     BLINKER_LOG("btn-ky按键回调", state);
//     if (bnt_tap == state)
//     {
//         rgb_on = !rgb_on;
//     }
//     if (rgb_on)
//     {
//         RR = RR_Old;
//         GG = GG_Old;
//         BB = BB_Old;
//         g_rgb_button.text("开", "开");
//         g_rgb_button.print();
//     }
//     else
//     {
//         RR_Old = RR;
//         GG_Old = GG;
//         BB_Old = BB;
//         RR = 0;
//         GG = 0;
//         BB = 0;
//         g_rgb_button.text("关", "关");
//         g_rgb_button.print();
//     }
// }

// void rgb_led_callback(uint8_t r_value, uint8_t g_value, uint8_t b_value, uint8_t bright_value)
// {
//     RR = r_value;
//     GG = g_value;
//     BB = b_value;
//     Bright = bright_value;
//     BLINKER_LOG("R value: ", r_value);
//     BLINKER_LOG("G value: ", g_value);
//     BLINKER_LOG("B value: ", b_value);
//     BLINKER_LOG("Rrightness value: ", bright_value);
// }

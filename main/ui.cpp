#include "ui.h"
#include "lcd.h"

uint8_t g_main_ui_case = 0;
uint8_t g_bat_charge_flag = 0;

void task_ui(void *pvParameters)
{
    (void)pvParameters;
    lcd_init();
    lcd_clear(BLACK);
    static uint8_t old_main_ui_case = 99;
    static uint8_t old_bat_charge_flag = 99;

    for (;;)
    {
        g_main_ui_case = g_main_ui_case%5;
        if (old_main_ui_case != g_main_ui_case)
        {
            old_main_ui_case = g_main_ui_case;
            lcd_clear(BLACK);
        }
        if (old_bat_charge_flag != g_bat_charge_flag)
        {
            old_bat_charge_flag = g_bat_charge_flag;
            lcd_show_img_charge(g_bat_charge_flag);
        }
        lcd_show_img_bat(g_bat_charge_flag);
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
        }
    }
}


    // lcd_fill(0, 0, 100, 100, BLUE);
    // lcd_draw_line(100, 100, 200, 200, RED);
    // lcd_show_char(200, 200, WHITE, BLACK, 'K', 24, 1);
    // lcd_show_num(100, 100, WHITE, BLACK, 12345, 6, 24, '0', 1);
    // lcd_show_str(0, 0, WHITE, BLACK, "你好世界abcdABCD", 16, 1);
#include "lcd.h"
#include "SPI.h"
#include "font.h"

SPIClass *g_hspi = NULL;

#define PIN_NUM_MISO 35 // 原25
#define PIN_NUM_MOSI 23 // 一致
#define PIN_NUM_CLK 18
// #define PIN_NUM_CS   22  // CS????
#define PIN_NUM_RST 5
#define PIN_NUM_DC 17

// #define PIN_NUM_MISO 21
// #define PIN_NUM_MOSI 22
// #define PIN_NUM_CLK  23

// // #define PIN_NUM_CS   22
// #define PIN_NUM_RST  19
// #define PIN_NUM_DC   18

#define LCD_TOTAL_BUF_SIZE 115200
#define LCD_BUF_SIZE 1152
static uint8_t lcd_buf[LCD_BUF_SIZE];

static const int spiClk = 40000000;

static void lcd_write_cmd(uint8_t cmd)
{
    digitalWrite(PIN_NUM_DC, LOW);
    g_hspi->transfer(cmd);
}

static void lcd_write_data(uint8_t data)
{
    digitalWrite(PIN_NUM_DC, HIGH);
    g_hspi->transfer(data);
}

static void lcd_write_buf(uint8_t *buf, uint32_t size)
{
    digitalWrite(PIN_NUM_DC, HIGH);
    g_hspi->transferBytes(buf, NULL, size); // 别用transfer 重载的另一个函数
}

static void lcd_write_color(uint16_t color)
{
    digitalWrite(PIN_NUM_DC, HIGH);
    g_hspi->transfer16(color);
}

static void lcd_address_set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    lcd_write_cmd(0x2a);
    lcd_write_data(x1 >> 8);
    lcd_write_data(x1);
    lcd_write_data(x2 >> 8);
    lcd_write_data(x2);

    lcd_write_cmd(0x2b);
    lcd_write_data(y1 >> 8);
    lcd_write_data(y1);
    lcd_write_data(y2 >> 8);
    lcd_write_data(y2);

    lcd_write_cmd(0x2C);
}

void lcd_init(void)
{
    pinMode(PIN_NUM_RST, OUTPUT);
    pinMode(PIN_NUM_DC, OUTPUT);

    digitalWrite(PIN_NUM_RST, HIGH);
    digitalWrite(PIN_NUM_DC, HIGH);

    digitalWrite(PIN_NUM_RST, LOW);
    vTaskDelay(200 / portTICK_RATE_MS);
    digitalWrite(PIN_NUM_RST, HIGH);
    vTaskDelay(200 / portTICK_RATE_MS);

    g_hspi = new SPIClass(HSPI);
    g_hspi->begin(PIN_NUM_CLK, PIN_NUM_MISO, PIN_NUM_MOSI, -1);
    g_hspi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE3));



    {
        /* Sleep Out */
        lcd_write_cmd(0x11);
        /* wait for power stability */
        vTaskDelay(120 / portTICK_RATE_MS);

        /* Memory Data Access Control */
        lcd_write_cmd(0x36);
        lcd_write_data(0x00);

        /* RGB 5-6-5-bit  */
        lcd_write_cmd(0x3A);
        lcd_write_data(0x65);

        /* Porch Setting */
        lcd_write_cmd(0xB2);
        lcd_write_data(0x0C);
        lcd_write_data(0x0C);
        lcd_write_data(0x00);
        lcd_write_data(0x33);
        lcd_write_data(0x33);

        /*  Gate Control */
        lcd_write_cmd(0xB7);
        lcd_write_data(0x72);

        /* VCOM Setting */
        lcd_write_cmd(0xBB);
        lcd_write_data(0x3D); //Vcom=1.625V

        /* LCM Control */
        lcd_write_cmd(0xC0);
        lcd_write_data(0x2C);

        /* VDV and VRH Command Enable */
        lcd_write_cmd(0xC2);
        lcd_write_data(0x01);

        /* VRH Set */
        lcd_write_cmd(0xC3);
        lcd_write_data(0x19);

        /* VDV Set */
        lcd_write_cmd(0xC4);
        lcd_write_data(0x20);

        /* Frame Rate Control in Normal Mode */
        lcd_write_cmd(0xC6);
        lcd_write_data(0x0F); //60MHZ

        /* Power Control 1 */
        lcd_write_cmd(0xD0);
        lcd_write_data(0xA4);
        lcd_write_data(0xA1);

        /* Positive Voltage Gamma Control */
        lcd_write_cmd(0xE0);
        lcd_write_data(0xD0);
        lcd_write_data(0x04);
        lcd_write_data(0x0D);
        lcd_write_data(0x11);
        lcd_write_data(0x13);
        lcd_write_data(0x2B);
        lcd_write_data(0x3F);
        lcd_write_data(0x54);
        lcd_write_data(0x4C);
        lcd_write_data(0x18);
        lcd_write_data(0x0D);
        lcd_write_data(0x0B);
        lcd_write_data(0x1F);
        lcd_write_data(0x23);

        /* Negative Voltage Gamma Control */
        lcd_write_cmd(0xE1);
        lcd_write_data(0xD0);
        lcd_write_data(0x04);
        lcd_write_data(0x0C);
        lcd_write_data(0x11);
        lcd_write_data(0x13);
        lcd_write_data(0x2C);
        lcd_write_data(0x3F);
        lcd_write_data(0x44);
        lcd_write_data(0x51);
        lcd_write_data(0x2F);
        lcd_write_data(0x1F);
        lcd_write_data(0x1F);
        lcd_write_data(0x20);
        lcd_write_data(0x23);

        /* Display Inversion On */
        lcd_write_cmd(0x21);

        lcd_write_cmd(0x29);
    }

    lcd_address_set(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);

    
}

void lcd_clear(uint16_t color)
{
    lcd_address_set(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);

    for (uint16_t j = 0; j < LCD_BUF_SIZE / 2; j++)
    {
        lcd_buf[j * 2] = color >> 8;
        lcd_buf[j * 2 + 1] = color;
    }

    for (uint16_t i = 0; i < (LCD_TOTAL_BUF_SIZE / LCD_BUF_SIZE); i++)
    {
        lcd_write_buf(lcd_buf, LCD_BUF_SIZE);
    }
}

void lcd_fill(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t color)
{
    uint32_t size_remain = 0;
    uint32_t size = (x_end - x_start + 1) * (y_end - y_start + 1) * 2;

    if (size > LCD_BUF_SIZE)
    {
        size_remain = size - LCD_BUF_SIZE;
        size = LCD_BUF_SIZE;
    }

    lcd_address_set(x_start, y_start, x_end, y_end);

    while (1)
    {
        for (uint16_t i = 0; i < size / 2; i++)
        {
            lcd_buf[2 * i] = color >> 8;
            lcd_buf[2 * i + 1] = color;
        }

        lcd_write_buf(lcd_buf, size);

        if (size_remain == 0)
            break;

        if (size_remain > LCD_BUF_SIZE)
        {
            size_remain = size_remain - LCD_BUF_SIZE;
        }

        else
        {
            size = size_remain;
            size_remain = 0;
        }
    }
}

void lcd_draw_point(uint16_t x, uint16_t y, uint16_t color)
{
    lcd_address_set(x, y, x, y);
    lcd_write_color(color);
}

void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    int16_t distance;
    int16_t incx, incy;

    if (y1 == y2)
    {
        /*??????*/
        lcd_address_set(x1, y1, x2, y2);

        for (uint16_t i = 0; i < x2 - x1; i++)
        {
            lcd_buf[2 * i] = color >> 8;
            lcd_buf[2 * i + 1] = color;
        }
        lcd_write_buf(lcd_buf, (x2 - x1) * 2);
        return;
    }

    int16_t delta_x = x2 - x1;
    int16_t delta_y = y2 - y1;
    int16_t row = x1;
    int16_t col = y1;
    int16_t xerr;
    int16_t yerr;

    if (delta_x > 0)
        incx = 1;
    else if (delta_x == 0)
        incx = 0;
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if (delta_y > 0)
        incy = 1;
    else if (delta_y == 0)
        incy = 0;
    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if (delta_x > delta_y)
        distance = delta_x;

    else
        distance = delta_y;

    for (uint16_t i = 0; i <= distance + 1; i++)
    {
        lcd_draw_point(row, col, color);
        xerr += delta_x;
        yerr += delta_y;

        if (xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }

        if (yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}

void lcd_draw_rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    lcd_draw_line(x1, y1, x2, y1, color);
    lcd_draw_line(x1, y1, x1, y2, color);
    lcd_draw_line(x1, y2, x2, y2, color);
    lcd_draw_line(x2, y1, x2, y2, color);
}


void lcd_show_char(u16 x, u16 y, u16 fc, u16 bc, u8 chr, u8 size, u8 cover)
{
    uint8_t data;
    uint8_t RowNum = size;
	uint8_t ColNum = size/2/8 + ((size/2)%8!=0?1:0);
    if (size>=80)
    {
        chr = chr - '0';
    }
    else
    {
        chr = chr - ' ';
    }
    
    lcd_address_set(x, y, x+size/2-1, y+size-1);
    for (uint8_t j=0; j<RowNum; j++)
	{
		for (uint8_t i=0; i<ColNum; i++)
		{
            switch (size)
            {
                // case 12:
				// 	data = asc2_1206[chr][j*ColNum + i];
                //     break;
				
                case 16:
					data = asc2_1608[chr][j*ColNum + i];
                    break;
				
                case 24:
					data = asc2_2412[chr][j*ColNum + i];
					break;
				
                case 80:
					data = asc2_8040[chr][j*ColNum + i];
					break;

                default:
                    return;
            }

			for (uint8_t k=0; k<((i==ColNum-1)?((size/2)%8!=0?(size/2)%8:8):8); k++)
            {
				if (cover)
				{
					if (data&0x01)
						lcd_write_color(fc);
					else
						lcd_write_color(bc);
					data >>= 1; 
				}
				else
				{
					if (data&0x01)
						lcd_draw_point(x+i*8+k, y+j, fc);
					data >>= 1; 
				}
            }
        }
    }
    lcd_address_set(0, 0, LCD_WIDTH-1, LCD_HEIGHT-1);
}

static uint32_t lcd_pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;

    while(n--)result *= m;

    return result;
}

void lcd_show_num(u16 x, u16 y, u16 fc, u16 bc, u32 num, u8 len, u8 size, u8 add, u8 cover)
{
    uint8_t enshow = 0;

    for(uint8_t t = 0; t < len; t++)
    {
        uint8_t temp = (num / lcd_pow(10, len - t - 1)) % 10;

        if(enshow == 0 && t < (len - 1))
        {
            if(temp == 0)
            {
                lcd_show_char(x + (size / 2)*t, y, fc, bc, add, size, cover);
                continue;
            }

            else enshow = 1;
        }

		lcd_show_char(x + (size / 2)*t, y, fc, bc, temp + '0', size, cover);
    }
}

void lcd_show_zh(u16 x, u16 y, u16 fc, u16 bc, char *str, u8 size, u8 cover)
{
    uint16_t zh_num;
    uint8_t data;
    uint8_t index[3];
    uint8_t RowNum = size;
    uint8_t ColNum = size / 8 + (size % 8 != 0 ? 1 : 0);

    switch (size)
    {
    case 16:
        zh_num = ZH_FONT16_NUM;
        break;
    case 24:
        zh_num = ZH_FONT24_NUM;
        break;
    // case 36:
    //     zh_num = ZH_FONT36_NUM;
        break;
    default:
        return;
    }
    for (uint16_t k = 0; k < zh_num; k++)
    {
        switch (size)
        {
        case 16:
            index[0] = ZH_FONT16[k].index[0];
            index[1] = ZH_FONT16[k].index[1];
            index[2] = ZH_FONT16[k].index[2];
            break;
        case 24:
            index[0] = ZH_FONT24[k].index[0];
            index[1] = ZH_FONT24[k].index[1];
            index[2] = ZH_FONT24[k].index[2];
            break;
        // case 36:
        //     index[0] = ZH_FONT36[k].index[0];
        //     index[1] = ZH_FONT36[k].index[1];
        //     index[2] = ZH_FONT36[k].index[2];
        //     break;
        default:
            return;
        }
        if ((index[0] == *(str)) && (index[1] == *(str + 1) && (index[2] == *(str + 2))))
        {
            lcd_address_set(x, y, x + size - 1, y + size - 1);

            for (uint8_t j = 0; j < RowNum; j++)
            {
                for (uint8_t i = 0; i < ColNum; i++)
                {
                    switch (size)
                    {
                    case 16:
                        data = ZH_FONT16[k].msk[j * ColNum + i];
                        break;
                    case 24:
                        data = ZH_FONT24[k].msk[j * ColNum + i];
                        break;
                    // case 36:
                    //     data = ZH_FONT36[k].msk[j * ColNum + i];
                    //     break;
                    default:
                        return;
                    }

                    for (uint8_t a = 0; a < ((i == ColNum - 1) ? ((size) % 8 != 0 ? (size) % 8 : 8) : 8); a++)
                    {
                        if (cover)
                        {
                            if (data & 0x01)
                                lcd_write_color(fc);
                            else
                                lcd_write_color(bc);
                            data >>= 1;
                        }
                        else
                        {
                            if (data & 0x01)
                                lcd_draw_point(x + i * 8 + a, y + j, fc);
                            data >>= 1;
                        }
                    }
                }
            }
        }
        continue;
    }
}

void lcd_show_str(u16 x, u16 y, u16 fc, u16 bc, char *str, u8 size, u8 cover)
{
    uint8_t bHz = 0;

    while (*str != 0)
    {
        if (!bHz)
        {
            if (*str > 0x80)
                bHz = 1;
            else
            {

                lcd_show_char(x, y, fc, bc, *str, size, cover);
                x += (size / 2);

                str++;
            }
        }
        else
        {
            bHz = 0;

            lcd_show_zh(x, y, fc, bc, str, size, cover);
            str += 3;
            x += size;
        }
    }
}

void lcd_show_img(u16 x, u16 y, u16 width, u16 height, const u8 *p)
{
    if(x + width > LCD_WIDTH || y + height > LCD_HEIGHT)
    {
        return;
    }
    lcd_address_set(x, y, x + width - 1, y + height - 1);
    lcd_write_buf((uint8_t *)p, width * height * 2);
}


void lcd_show_img_bin(u16 x, u16 y, u16 width, u16 height, u16 fc, u16 bc, const u8 *img, u8 cover)
{
    uint8_t RowNum = height; // 行
    uint8_t ColNum = width/8 + (width%8!=0?1:0);
    uint8_t data;
    
    lcd_address_set(x, y, x+width-1, y+height-1);
    
    for (uint8_t j=0; j<RowNum; j++)
    {
        for (uint8_t i=0; i<ColNum; i++)
        {
            data = img[j*ColNum + i];
            for (uint8_t a=0; a<((i==ColNum-1)?((width)%8!=0?(width)%8:8):8); a++)
            {
                if (cover)
                {
                    if (data&0x01)
                        lcd_write_color(fc);
                    else
                        lcd_write_color(bc);
                    data >>= 1; 
                }
                else
                {
                    if (data&0x01)
                        lcd_draw_point(x+i*8+a, y+j, fc);
                    data >>= 1; 
                }
            }
        }
    }
}




#include "task.h"

void lcd_show_img_bat(uint8_t show_flag)
{
    if (show_flag)
        lcd_show_img_bin(206, 1, 32, 18, WHITE, BLACK, img_bin_bat, 1);
    else
        lcd_show_img_bin(206, 1, 32, 18, BLACK, BLACK, img_bin_bat, 1);
}

void lcd_show_img_charge(uint8_t show_flag)
{
    if (show_flag)
        lcd_show_img(186, 0, 15, 20, img_charge);
    else
        lcd_fill(186, 0, 186+15, 0+20, BLACK);
}

void lcd_show_img_wifi(uint8_t show_flag)
{
    if (show_flag)
        lcd_show_img(0, 0, 23, 20, img_wifi_on);
    else
    {
        lcd_draw_line(0, 0, 0, 20, BLACK);
        lcd_show_img(1, 0, 22, 20, img_wifi_off);
    }    
}

void lcd_show_img_device(void)
{
    lcd_show_img_bin(56, 46, 128, 128, WHITE, BLACK, img_bin_device, 1);
}

void lcd_show_img_game(void)
{
    lcd_show_img_bin(56, 46, 128, 128, WHITE, BLACK, img_bin_game, 1);
}

void lcd_show_img_set(void)
{
    lcd_show_img_bin(56, 46, 128, 128, WHITE, BLACK, img_bin_set, 1);
}

void lcd_show_img_help(void)
{
    lcd_show_img_bin(56, 46, 128, 128, WHITE, BLACK, img_bin_help, 1);
}

void lcd_show_img_wait(void)
{
    lcd_show_img_bin(56, 46, 128, 128, WHITE, BLACK, img_bin_wait, 1);
}

void lcd_show_img_poweroff(void)
{
    lcd_show_img_bin(56, 36, 128, 128, WHITE, BLACK, img_bin_poweroff, 1);
}

void lcd_show_img_device1(uint16_t x, uint16_t y)
{
    lcd_show_img_bin(x, y, 40, 48, WHITE, BLACK, img_bin_device1, 1);
}

void lcd_show_img_device2(uint16_t x, uint16_t y)
{
    lcd_show_img_bin(x, y, 40, 48, WHITE, BLACK, img_bin_device2, 1);
}



void ui_show_dial(void)
{
    char str[30] = {0};

    sprintf(str, "%02d:%02d", g_date_time.hour, g_date_time.minute);
    lcd_show_str(20, 55, WHITE, BLACK, str, 80, 1);

    lcd_show_num(210, 125, WHITE, BLACK, g_date_time.sec, 2, 24, '0', 1);

    sprintf(str, "20%02d/%02d/%02d  周%s", 
                g_date_time.year, g_date_time.month, g_date_time.day, ZH_FONT24[g_date_time.week].index);
    lcd_show_str(30, 152, WHITE, BLACK, str, 24, 1);
}

void ui_show_device(void)
{
    char str[30] = {0};

    lcd_show_img_device();
    lcd_show_str(90, 200, WHITE, BLACK, "设 备", 24, 1);
}

void ui_show_game(void)
{
    char str[30] = {0};

    lcd_show_img_game();
    lcd_show_str(90, 200, WHITE, BLACK, "游 戏", 24, 1);
}

void ui_show_set(void)
{
    char str[30] = {0};

    lcd_show_img_set();
    lcd_show_str(90, 200, WHITE, BLACK, "设 置", 24, 1);
}

void ui_show_help(void)
{
    char str[30] = {0};

    lcd_show_img_help();
    lcd_show_str(90, 200, WHITE, BLACK, "帮 助", 24, 1);
}

void ui_show_device_con(void)
{
    char str[30] = {0};
    lcd_show_img_device1(40, 80);
    lcd_show_str( 12, 140, WHITE, BLACK, "氛 围 灯", 24, 1);
    lcd_show_img_device2(148, 80);
    lcd_show_str(132, 140, WHITE, BLACK, "智能插排", 24, 1);
}
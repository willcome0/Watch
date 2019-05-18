#include "task.h"
#include "Arduino.h"

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



void task_data_time(void *pvParameters);
void task_mpu6050(void *pvParameters);
void task_iic(void *pvParameters);



void creat_task(void)
{
    iic_init();
    ft6236_init();



    xTaskCreatePinnedToCore(
        task_data_time,
        "task_data_time",
        1024,
        NULL, configMAX_PRIORITIES - 1,
        NULL, ARDUINO_RUNNING_CORE);

    xTaskCreatePinnedToCore(
        task_led,
        "task_led",
        1024,
        NULL, 2,
        NULL, ARDUINO_RUNNING_CORE);

    xTaskCreatePinnedToCore(
        task_bat_adc,
        "task_bat_adc",
        2048,
        NULL, 2, // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        NULL, ARDUINO_RUNNING_CORE);

    xTaskCreatePinnedToCore(
        task_key,
        "task_key",
        2048,
        NULL, configMAX_PRIORITIES - 2, // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        NULL, ARDUINO_RUNNING_CORE);

    xTaskCreatePinnedToCore(
        task_ui,
        "task_ui",
        2048,
        NULL, 2,
        NULL, ARDUINO_RUNNING_CORE);

    xTaskCreatePinnedToCore(
        task_touch,
        "task_touch",
        2048,
        NULL, configMAX_PRIORITIES - 3,
        NULL, ARDUINO_RUNNING_CORE);

    // xTaskCreatePinnedToCore(
    //     task_mpu6050,
    //     "task_mpu6050",
    //     2048,
    //     NULL, configMAX_PRIORITIES - 1,
    //     NULL, ARDUINO_RUNNING_CORE);
}

struct Date_Time g_date_time = {19, 5, 16, 14, 01, 02, 4};

void task_data_time(void *pvParameters)
{
    (void)pvParameters;

    for (;;)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
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
        Serial.printf("20%02d年%02d月%02d日 %02d时%02d分%02d秒\r\n",
                      g_date_time.year, g_date_time.month, g_date_time.day,
                      g_date_time.hour, g_date_time.minute, g_date_time.sec);
    }
}


// void task_mpu6050(void *pvParameters)
// {
//     (void)pvParameters;

//     for (;;)
//     {
//         // vTaskDelay(100);

//         Wire.beginTransmission(MPU6050_addr);
//         Wire.write(0x3B);
//         Wire.endTransmission(false);
//         Wire.requestFrom(MPU6050_addr, 14, true);
//         AccX = Wire.read() << 8 | Wire.read();
//         AccY = Wire.read() << 8 | Wire.read();
//         AccZ = Wire.read() << 8 | Wire.read();
//         Temp = Wire.read() << 8 | Wire.read();
//         GyroX = Wire.read() << 8 | Wire.read();
//         GyroY = Wire.read() << 8 | Wire.read();
//         GyroZ = Wire.read() << 8 | Wire.read();
//         // Serial.print("AccX = ");
//         // Serial.print(AccX);
//         // Serial.print(" || AccY = ");
//         // Serial.print(AccY);
//         // Serial.print(" || AccZ = ");
//         // Serial.print(AccZ);
//         Serial.print(" || Temp = ");
//         Serial.print(Temp / 340.00 + 36.53);
//         // Serial.print(" || GyroX = ");
//         // Serial.print(GyroX);
//         // Serial.print(" || GyroY = ");
//         // Serial.print(GyroY);
//         // Serial.print(" || GyroZ = ");
//         // Serial.println(GyroZ);
//         // Serial.print("a/g:\t");
//         // Serial.print(ax); Serial.print("\t");
//         // Serial.print(ay); Serial.print("\t");
//         // Serial.print(az); Serial.print("\t");
//         // Serial.print(gx); Serial.print("\t");
//         // Serial.print(gy); Serial.print("\t");
//         // Serial.println(gz);
//     }
// }
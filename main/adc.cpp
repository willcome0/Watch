#include "adc.h"


#define PIN_NUM_BAT_ADC 13

void bat_adc_init(void)
{
    pinMode(PIN_NUM_BAT_ADC, INPUT);
    analogReadResolution(12);                   // Sets the sample bits and read resolution, default is 12-bit (0 - 4095), range is 9 - 12 bits
    analogSetWidth(12);                         // Sets the sample bits and read resolution, default is 12-bit (0 - 4095), range is 9 - 12 bits
    //  9-bit gives an ADC range of 0-511
    // 10-bit gives an ADC range of 0-1023
    // 11-bit gives an ADC range of 0-2047
    // 12-bit gives an ADC range of 0-4095
    analogSetCycles(8);                         // Set number of cycles per sample, default is 8 and provides an optimal result, range is 1 - 255
    analogSetSamples(1);                        // Set number of samples in the range, default is 1, it has an effect on sensitivity has been multiplied
    analogSetClockDiv(1);                       // Set the divider for the ADC clock, default is 1, range is 1 - 255
    analogSetAttenuation(ADC_0db);              // Sets the input attenuation for ALL ADC inputs, default is ADC_11db, range is ADC_0db, ADC_2_5db, ADC_6db, ADC_11db
    // analogSetPinAttenuation(VP,ADC_11db);    // Sets the input attenuation, default is ADC_11db, range is ADC_0db, ADC_2_5db, ADC_6db, ADC_11db
    // ADC_0db provides no attenuation so IN/OUT        = 1 / 1 an input of 3 volts remains at 3 volts before ADC measurement
    // ADC_2_5db provides an attenuation so that IN/OUT = 1 / 1.34 an input of 3 volts is reduced to 2.238 volts before ADC measurement
    // ADC_6db provides an attenuation so that IN/OUT   = 1 / 2 an input of 3 volts is reduced to 1.500 volts before ADC measurement
    // ADC_11db provides an attenuation so that IN/OUT  = 1 / 3.6 an input of 3 volts is reduced to 0.833 volts before ADC measurement

    adcAttachPin(PIN_NUM_BAT_ADC);
    analogSetClockDiv(255);                     // 1338mS
}

void task_bat_adc(void *pvParameters)
{
    (void)pvParameters;
    bat_adc_init();
    for (;;)
    {
        static uint16_t adc_value = 0;
        adcStart(PIN_NUM_BAT_ADC);
        while ( adcBusy(PIN_NUM_BAT_ADC));
        adc_value = analogRead(PIN_NUM_BAT_ADC);

        Serial.printf("\r\n电池电压：%d", adc_value);

        vTaskDelay(5000);
    }
 


    // adc_value = adcEnd(PIN_NUM_BAT_ADC);
    // RA_ChA2.addValue(adc_value);
    // adc_value = RA_ChA2.getAverage();

    // double accurateTemp = readVoltage(adc_value)*100;
    // Serial.print(accurateTemp / 3, 2);         // COrrect reading only when divided by 3 ..
    // Serial.println(" Deg C."); 
    // delay(500);

    // if (ledState ) ledState = LOW;
    // else ledState = HIGH; 

    // digitalWrite( thirteenLED, ledState);      // I am alive !!
}


#include "led.h"

#define PIN_NUM_LED_R 27
#define PIN_NUM_LED_G 25
#define PIN_NUM_LED_B 26

#define LCD_LEDA 14
#define MOTOR1  32
#define MOTOR2 33

#define TP_RST 16
#define LCD_DC 17
#define LCD_RST 5
#define KEY 19

#define LED_FREQ 12000
#define LED_BIT_NUM 8

uint8_t color = 0;          // a value from 0 to 255 representing the hue
uint32_t R, G, B;           // the Red Green and Blue color components
uint8_t brightness = 255; 

void led_init(void)
{
    // digitalWrite(PIN_NUM_LED_R, HIGH);
    // digitalWrite(PIN_NUM_LED_G, HIGH);
    // digitalWrite(PIN_NUM_LED_B, HIGH);

    // pinMode(PIN_NUM_LED_R, OUTPUT);
    // pinMode(PIN_NUM_LED_G, OUTPUT);
    // pinMode(PIN_NUM_LED_B, OUTPUT);

    ledcAttachPin(PIN_NUM_LED_R, 1);
    ledcAttachPin(PIN_NUM_LED_G, 2);
    ledcAttachPin(PIN_NUM_LED_B, 3);
    ledcAttachPin(LCD_LEDA, 4);
    ledcAttachPin(MOTOR1, 5);
    ledcAttachPin(MOTOR2, 6);

    ledcSetup(1, LED_FREQ, LED_BIT_NUM); // 12 kHz PWM, 8-bit resolution
    ledcSetup(2, LED_FREQ, LED_BIT_NUM);
    ledcSetup(3, LED_FREQ, LED_BIT_NUM);
    ledcSetup(4, LED_FREQ, LED_BIT_NUM);
    ledcSetup(5, LED_FREQ, LED_BIT_NUM);
    ledcSetup(6, LED_FREQ, LED_BIT_NUM);

    ledcWrite(1, 255);
    ledcWrite(2, 255);
    ledcWrite(3, 255);
    ledcWrite(4, 255);
}

void rgb_set(uint8_t r_value, uint8_t g_value, uint8_t b_value, uint8_t bright_value)
{
    // ledcWrite(1, (255 - r_value)*(255-bright_value)/255);
    // ledcWrite(2, (255 - g_value)*(255-bright_value)/255);
    // ledcWrite(3, (255 - b_value)*(255-bright_value)/255);
    ledcWrite(1, (255 - r_value));
    ledcWrite(2, (255 - g_value));
    ledcWrite(3, (255 - b_value));
}

void hueToRGB(uint8_t hue, uint8_t brightness)
{
    uint16_t scaledHue = (hue * 6);
    uint8_t segment = scaledHue / 256; // segment 0 to 5 around the
                                            // color wheel
    uint16_t segmentOffset =
      scaledHue - (segment * 256); // position within the segment

    uint8_t complement = 0;
    uint16_t prev = (brightness * ( 255 -  segmentOffset)) / 256;
    uint16_t next = (brightness *  segmentOffset) / 256;

    if(true) // 共阳极为真
    {
      brightness = 255 - brightness;
      complement = 255;
      prev = 255 - prev;
      next = 255 - next;
    }

    switch(segment ) {
    case 0:      // red
        R = brightness;
        G = next;
        B = complement;
    break;
    case 1:     // yellow
        R = prev;
        G = brightness;
        B = complement;
    break;
    case 2:     // green
        R = complement;
        G = brightness;
        B = next;
    break;
    case 3:    // cyan
        R = complement;
        G = prev;
        B = brightness;
    break;
    case 4:    // blue
        R = next;
        G = complement;
        B = brightness;
    break;
   case 5:      // magenta
    default:
        R = brightness;
        G = complement;
        B = prev;
    break;
    }
}

void task_led(void *pvParameters)
{
    (void)pvParameters;
    led_init();
    // ledcWrite(1, 255);
    //     ledcWrite(2, 255);
    //     ledcWrite(3, 255);
    ledcWrite(4, 255);
    for (;;)
    {
        // digitalWrite(PIN_NUM_LED_R, HIGH);
        // digitalWrite(PIN_NUM_LED_G, HIGH);
        // digitalWrite(PIN_NUM_LED_B, HIGH);
        ledcWrite(1, 200);
        ledcWrite(2, 200);
        ledcWrite(3, 200);
            vTaskDelay(2000);
            
        // digitalWrite(PIN_NUM_LED_R, LOW);
        // digitalWrite(PIN_NUM_LED_G, LOW);
        // digitalWrite(PIN_NUM_LED_B, LOW);
        ledcWrite(1, 0);
        ledcWrite(2, 0);
        ledcWrite(3, 0);
            vTaskDelay(2000);

        for (color = 0; color < 255; color++) 
        { // Slew through the color spectrum
            hueToRGB(color, brightness);  // call function to convert hue to RGB

            // write the RGB values to the pins
            ledcWrite(1, R); // write red component to channel 1, etc.
            ledcWrite(2, G);   
            ledcWrite(3, B);

            vTaskDelay(10); // full cycle of rgb over 256 colors takes 26 seconds
        }
    }
}

void set_motor(uint8_t on_off)
{
    if (on_off)
    {
        ledcWrite(5, 0);
        ledcWrite(6, 200);
    }
    else
    {
        // for (uint8_t i=0; i<11; i++)
        // {
        //     ledcWrite(5, 200-20*i);
        //     ledcWrite(6, 200-20*i);
        //     vTaskDelay(20);
        // }
        ledcWrite(5, 0);
        ledcWrite(6, 0);
    }
}
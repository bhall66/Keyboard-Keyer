 /**************************************************************************
      Author:   Bruce E. Hall, w8bh.net
        Date:   26 Aug 2021
    Hardware:   Seeed XIAO, 128x64 I2C OLED display module
    Software:   Arduino IDE 1.8.13
       Legal:   Copyright (c) 2021  Bruce E. Hall.
                Open Source under the terms of the MIT License. 
    
 Description:   Keyboard Keyer, Step 3b
                Adds OLED display to simple "CQ" Morse message
  
 **************************************************************************/

#include <Wire.h>                         // built-in I2C library
#include <Adafruit_GFX.h>                 // Adafruit graphics library
#include <Adafruit_SSD1306.h>             // Adafruit OLED library

#define SCREEN_WIDTH     128              // OLED display width, in pixels
#define SCREEN_HEIGHT    64               // OLED display height, in pixels
#define SCREEN_ADDRESS   0x3C             // OLED display I2C address, in hex

Adafruit_SSD1306 led(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
 
void dit() {
  digitalWrite(LED_BUILTIN,0);            // turn on LED
  tone(9,1200);                           // 1200 Hz tone on pin 9
  delay(120);                             // wait 120 milliseconds
  digitalWrite(LED_BUILTIN,1);            // turn off LED
  noTone(9);                              // turn off tone
  delay(120);                             // space between code elements
}

void dah() {
  digitalWrite(LED_BUILTIN,0);            // turn on LED
  tone(9,1200);                           // 1200 Hz tone on pin 9
  delay(360);                             // wait 360 milliseconds
  digitalWrite(LED_BUILTIN,1);            // turn off LED
  noTone(9);                              // turn off tone
  delay(120);                             // space between code elements
}

void setup() {
  pinMode(LED_BUILTIN,OUTPUT);
  led.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  led.setTextColor(WHITE,BLACK);         // use black on white text
  led.setTextSize(2);                    // medium-sized font
  led.clearDisplay();
}

void loop() {
  led.print('C'); led.display();         // show "C" on display
  dah(); dit(); dah(); dit();            // and send it in Morse
  delay(360);                            // space between characters
  
  led.print('Q'); led.display();         // show "Q" on display
  dah(); dah(); dit(); dah();            // and send it in Morse

  led.print(' '); led.display();         // space between words
  delay(3000);                           // repeat every 3 seconds
}

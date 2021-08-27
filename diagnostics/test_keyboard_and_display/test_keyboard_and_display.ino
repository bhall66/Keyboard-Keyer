 /**************************************************************************
      Author:   Bruce E. Hall, w8bh.net
        Date:   25 Aug 2021
    Hardware:   Seeeduino XIAO, 128x64 I2C OLED display, PS/2 keyboard
    Software:   Arduino IDE 1.8.13
       Legal:   Copyright (c) 2021  Bruce E. Hall.
                Open Source under the terms of the MIT License. 
    
 Description:   Tests Seeeduino XIAO with keyboard & OLED devices

                For the display, make the following connections:
                Connect XIAO pin 4 to display SDA
                Connect XIAO pin 5 to display SCL
                Connect XIAO 3v3 to display VCC
                Connect XIAO Gnd to display GND 

                For the keyboard, first convert to 3.3v logic levels. Then,
                Connect keyboard data (3.3V) to XIAO pin 1
                Connect keyboard clock (3.3V) to XIAO pin 0
                >> Connecting the keyboard directly will ruin the XIAO!! <<

                Check the I2C address of your device.
                SCREEN_ADDRESS 0x3C works for my board
  
 **************************************************************************/


#include <Wire.h>                     // Arduino I2C library
#include <Adafruit_GFX.h>             // Adafruit graphics library
#include <Adafruit_SSD1306.h>         // Adafruit OLED library
#include <EasyKey.h>                  // https://github.com/bhall66/EasyKey

#define CLOCK_PIN            0        // pin for keyboard clock line
#define DATA_PIN             1        // pin for keyboard data line
#define SCREEN_WIDTH       128        // OLED display width, in pixels
#define SCREEN_HEIGHT       64        // OLED display height, in pixels
#define SCREEN_ADDRESS    0x3C        // OLED display I2C address

EasyKey kbd;
Adafruit_SSD1306 led(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

void setup()   {     
  kbd.begin(DATA_PIN, CLOCK_PIN);             
  led.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  Serial.begin(115200);
  led.clearDisplay();
  led.setTextSize(2);
  led.setTextColor(WHITE,BLACK);
  led.print("Tap a key");
  led.display();                      // show Intro message on screen
  led.setTextSize(3);                 // use larger font for characters
}

void loop() { 
  if (kbd.available()) {              // As soon as a key is pressed:
    char c = kbd.read();              // get the keyboard character
    
    Serial.print(c);                  // and echo it to serial monitor
    Serial.print(', 0x');
    Serial.println(c,HEX);            // with hexadecimal equivalent

    led.clearDisplay();               // erase previous character
    led.setCursor(10,10);             // position cursor near top/left
    led.print(c);                     // show character on the screen
    led.print(' ');
    led.print(c,DEC);                 // with its ASCII codes
    led.display();                    // update display
  }
}

 /**************************************************************************
      Author:   Bruce E. Hall, w8bh.net
        Date:   26 Aug 2021
    Hardware:   Seeeduino XIAO, 128x64 I2C OLED display module
    Software:   Arduino IDE 1.8.13
       Legal:   Copyright (c) 2021  Bruce E. Hall.
                Open Source under the terms of the MIT License. 
    
 Description:   Hello, World test for I2C OLED display

                Make 4 wire connections between the two devices:
                Connect XIAO pin D4 to display SDA
                Connect XIAO pin D5 to display SCL
                Connect XIAO 3v3 to display VCC
                Connect XIAO Gnd to display GND 

                Check the I2C address of your device.
                For mine, the board documentation was wrong!
                SCREEN_ADDRESS 0x3C works for my board
  
 **************************************************************************/

 
#include <Wire.h>                         // built-in I2C library
#include <Adafruit_GFX.h>                 // Adafruit graphics library
#include <Adafruit_SSD1306.h>             // Adafruit OLED library

#define SCREEN_WIDTH     128              // OLED display width, in pixels
#define SCREEN_HEIGHT    64               // OLED display height, in pixels
#define SCREEN_ADDRESS   0x3C             // OLED display I2C address, in hex

Adafruit_SSD1306 led(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

void setup()   {            
  led.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  led.display();
}

void loop() {
}

 /**************************************************************************
      Author:   Bruce E. Hall, w8bh.net
        Date:   26 Aug 2021
    Hardware:   Seeeduino XIAO, 128x64 I2C OLED display module
    Software:   Arduino IDE 1.8.13
       Legal:   Copyright (c) 2021  Bruce E. Hall.
                Open Source under the terms of the MIT License. 
    
 Description:   Interrupt test.

                First, connect OLED display as follows:
                Connect XIAO pin D4 to display SDA
                Connect XIAO pin D5 to display SCL
                Connect XIAO 3v3 to display VCC
                Connect XIAO Gnd to display GND 

                Next, briefly touch pin0 to ground.
                The display should count 1->0 transitions.
  
 **************************************************************************/

 
#include <Wire.h>                         // built-in I2C library
#include <Adafruit_GFX.h>                 // Adafruit graphics library
#include <Adafruit_SSD1306.h>             // Adafruit OLED library

#define SCREEN_WIDTH     128              // OLED display width, in pixels
#define SCREEN_HEIGHT    64               // OLED display height, in pixels
#define SCREEN_ADDRESS   0x3C             // OLED display I2C address, in hex

#define INTERRUPT_PIN    0                // Testing interrupt on pin 0


Adafruit_SSD1306 led(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
volatile int count=0;

void isr() {                             // Interrupt Service Routine:
  count++;                               // Increment the count!
}

void setup()   {            
  led.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  pinMode(INTERRUPT_PIN,INPUT_PULLUP);   // Pull-up interrupt pin to logic 1
  attachInterrupt(
    digitalPinToInterrupt
    (INTERRUPT_PIN),isr,FALLING);        // Interrupt on 1->0 transitions
  led.setTextColor(WHITE,BLACK);         // White on Black text
  led.setTextSize(3);                    // Using a large font
}

void loop() {                            // Repeat at 10x per second:
  led.clearDisplay();                    // Clear previous result
  led.setCursor(20,20);                  // Position near top/left
  led.print(count);                      // Show the current count
  led.display();                         // Update the display
  delay(100);                            // Wait 0.1 sec before repeating
}

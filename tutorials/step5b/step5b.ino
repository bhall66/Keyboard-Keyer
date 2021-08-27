 /**************************************************************************
      Author:   Bruce E. Hall, w8bh.net
        Date:   26 Aug 2021
    Hardware:   Seeeduino XIAO, 128x64 I2C OLED display, PS/2 Keyboard
    Software:   Arduino IDE 1.8.13, EasyKey library
       Legal:   Copyright (c) 2021  Bruce E. Hall.
                Open Source under the terms of the MIT License. 
    
 Description:   Keyboard Keyer, Step 5b
                Send Morse letters A through I via keyboard input
  
 **************************************************************************/

#include <Wire.h>                         // built-in I2C library
#include <Adafruit_GFX.h>                 // Adafruit graphics library
#include <Adafruit_SSD1306.h>             // Adafruit OLED library
#include <EasyKey.h>                      // https://github.com/bhall66/EasyKey

#define SCREEN_WIDTH     128              // OLED display width, in pixels
#define SCREEN_HEIGHT    64               // OLED display height, in pixels
#define SCREEN_ADDRESS   0x3C             // OLED display I2C address, in hex
#define CLOCK_PIN        0                // pin for keyboard clock line
#define DATA_PIN         1                // pin for keyboard data line


Adafruit_SSD1306 led(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
EasyKey kbd;
 
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

void sendMorse(char c) {                  
  switch(c) {
    case 'A': dit(); dah();               break;
    case 'B': dah(); dit(); dit(); dit(); break;
    case 'C': dah(); dit(); dah(); dit(); break;
    case 'D': dah(); dit(); dit();        break;
    case 'E': dit();                      break;
    case 'F': dit(); dit(); dah(); dit(); break;
    case 'G': dah(); dah(); dit();        break;
    case 'H': dit(); dit(); dit(); dit(); break;
    case 'I': dit(); dit();               break;

    // You get the idea.
    // Feel free to extend this table for all
    // alphanumeric characters
    
  }
}

void setup() {
  pinMode(LED_BUILTIN,OUTPUT);
  led.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  led.setTextColor(WHITE,BLACK);          // use black on white text
  led.setTextSize(2);                     // medium-sized font
  led.clearDisplay();
  kbd.begin(DATA_PIN, CLOCK_PIN);         // initialize keyboard library   
}

void loop() {
  if (kbd.available()) {                  // wait for keypress
    char c = toupper(kbd.read());         // get the character, as uppercase
    led.print(c); led.display();          // show character on display
    sendMorse(c);                         // and send it as Morse.
  }
}

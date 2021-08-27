 /**************************************************************************
      Author:   Bruce E. Hall, w8bh.net
        Date:   26 Aug 2021
    Hardware:   Seeed XIAO, PS/2 Keyboard
    Software:   Arduino IDE 1.8.13
       Legal:   Copyright (c) 2021  Bruce E. Hall.
                Open Source under the terms of the MIT License. 
    
 Description:   SUPER-SIMPLE PS/2 KEYBOARD TEST
                Using the "EasyKey" library

                After converting your keyboard logic levels to 3.3V,
                connect the keyboard clock line to D0
                and the keyboard data line to D1.
             
                Press a key, and the character
                is echoed to the serial monitor
  
 **************************************************************************/

#include <EasyKey.h>                  // https://github.com/bhall66/EasyKey

#define CLOCK_PIN            0        // specify pin for keyboard clock line
#define DATA_PIN             1        // specify pin for keyboard data line
#define BAUDRATE        115200        // serial monitor baud rate

EasyKey kbd;                          // 'kbd' is the keyboard object

void setup()   {     
  kbd.begin(DATA_PIN, CLOCK_PIN);     // initialize keyboard library          
  Serial.begin(BAUDRATE);             // initialize serial monitor
}

void loop() { 
  if (kbd.available()) {              // As soon as a key is pressed:
    char c = kbd.read();              // get the keyboard character
    Serial.print(c);                  // echo it to serial monitor
  }
}

 /**************************************************************************
      Author:   Bruce E. Hall, w8bh.net
        Date:   26 Aug 2021
    Hardware:   Seeeduino XIAO
    Software:   Arduino IDE 1.8.13
       Legal:   Copyright (c) 2021  Bruce E. Hall.
                Open Source under the terms of the MIT License. 
    
 Description:   Keyboard Keyer, Step 1
                The famous BLINK sketch: flash onboard LED at 1Hz
  
 **************************************************************************/


void setup() {
  pinMode(LED_BUILTIN,OUTPUT);
}

void loop() {                             // REPEAT FOREVER:
  digitalWrite(LED_BUILTIN,0);            // turn on LED
  delay(500);                             // wait 500 milliseconds
  digitalWrite(LED_BUILTIN,1);            // turn off LED
  delay(500);                             // wait 500 milliseconds
}

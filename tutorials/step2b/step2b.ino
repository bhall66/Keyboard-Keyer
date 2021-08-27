 /**************************************************************************
      Author:   Bruce E. Hall, w8bh.net
        Date:   26 Aug 2021
    Hardware:   Seeeduino XIAO
    Software:   Arduino IDE 1.8.13
       Legal:   Copyright (c) 2021  Bruce E. Hall.
                Open Source under the terms of the MIT License. 
    
 Description:   Keyboard Keyer, Step 2b
                Send a simple "CQ" Morse message
                For audio, attach a piezo element between pin D9 and ground.
  
 **************************************************************************/


void dit() {                              // SEND A DIT TO PIEZO & LED
  digitalWrite(LED_BUILTIN,0);            // turn on LED
  tone(9,1200);                           // 1200 Hz tone on pin 9
  delay(120);                             // wait 120 milliseconds
  digitalWrite(LED_BUILTIN,1);            // turn off LED
  noTone(9);                              // turn off tone
  delay(120);                             // space between code elements
}

void dah() {                              // SEND A DAH TO PIEZO & LED
  digitalWrite(LED_BUILTIN,0);            // turn on LED
  tone(9,1200);                           // 1200 Hz tone on pin 9
  delay(360);                             // wait 360 milliseconds
  digitalWrite(LED_BUILTIN,1);            // turn off LED
  noTone(9);                              // turn off tone
  delay(120);                             // space between code elements
}

void setup() {
  pinMode(LED_BUILTIN,OUTPUT);
}

void loop() {                             // HERES WHRE IT ALL HAPPENS:
  dah(); dit(); dah(); dit();             // send 'C' in Morse
  delay(360);                             // space between characters
  dah(); dah(); dit(); dah();             // send 'Q' in Morse
  delay(3000);                            // repeat every 3 seconds
}

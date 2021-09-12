 /**************************************************************************
      Author:   Bruce E. Hall, w8bh.net
        Date:   10 Sep 2021
    Hardware:   Seeeduino XIAO
    Software:   Arduino IDE 1.8.13
       Legal:   Copyright (c) 2021  Bruce E. Hall.
                Open Source under the terms of the MIT License. 
    
 Description:   Keyboard Keyer, Step 2a
                Send the character "S" (3 dits) in Morse Code.
  
 **************************************************************************/


void dit() {                              // SEND A DIT TO PIEZO & LED
  digitalWrite(LED_BUILTIN,0);            // turn on LED
  delay(120);                             // wait 120 milliseconds
  digitalWrite(LED_BUILTIN,1);            // turn off LED
  delay(120);                             // space between code elements
}

void setup() {
  pinMode(LED_BUILTIN,OUTPUT);
}

void loop() {                             // HERES WHERE IT ALL HAPPENS:
  dit(); dit(); dit();                    // send 'S' in Morse
  delay(2000);                            // repeat every 2 seconds
}

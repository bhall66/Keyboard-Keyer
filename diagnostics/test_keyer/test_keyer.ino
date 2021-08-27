
 /**************************************************************************
      Author:   Bruce E. Hall, w8bh.net
        Date:   26 Aug 2021
    Hardware:   Seeeduino XIAO module, Piezo
    Software:   Arduino IDE 1.8.13
       Legal:   Copyright (c) 2021  Bruce E. Hall.
                Open Source under the terms of the MIT License. 
    
 Description:   Tests keyer output with a string of dits at 2 Hz.
                Keyer should key in time with the XIAO orange LED.
   
 **************************************************************************/

#define KEYER            2                   // pin for Morse key output

void setup() {     
  pinMode(KEYER,OUTPUT);                     // allow control of Morse Output
  digitalWrite(KEYER,0);                     // turn off Morse output 
}

void loop() {             
  digitalWrite(KEYER,1);                     // turn on keyer
  digitalWrite(LED_BUILTIN,0);               // turn on LED
  delay(100);                                // dit!
  digitalWrite(KEYER,0);                     // turn off keyer
  digitalWrite(LED_BUILTIN,1);               // turn off LED
  delay(400);                                // set dit rate at 2 Hz
}

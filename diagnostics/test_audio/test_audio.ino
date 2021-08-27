
 /**************************************************************************
      Author:   Bruce E. Hall, w8bh.net
        Date:   26 Aug 2021
    Hardware:   Seeeduino XIAO module, Piezo
    Software:   Arduino IDE 1.8.13
       Legal:   Copyright (c) 2021  Bruce E. Hall.
                Open Source under the terms of the MIT License. 
    
 Description:   Tests Seeeduino XIAO with beeping tone
                Attach a piezo element between pin 9 and ground.
   
 **************************************************************************/

#define AUDIO     9       // pin for audio ouput
#define PITCH     1200    // audio frequency, in Hz
#define DURATION  1000    // tone duration, in mS

void setup() {            // no need to set AUDIO pin as output
}

void loop() {             // Beep piezo element as follows:
  tone(AUDIO, PITCH);     // Start tone
  delay(DURATION);        // Keep tone on for a while
  noTone(AUDIO);          // Stop tone
  delay(DURATION);        // Keep tone off for a while
}

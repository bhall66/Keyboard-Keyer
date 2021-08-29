
 /**************************************************************************
      Author:   Bruce E. Hall, w8bh.net
        Date:   29 Aug 2021
    Hardware:   Seeeduino XIAO, 128x64 I2C OLED display, PS/2 Keyboard
    Software:   Arduino IDE 1.8.13, EasyKey library
       Legal:   Copyright (c) 2021  Bruce E. Hall.
                Open Source under the terms of the MIT License. 
    
 Description:   Generate Morse Code with a PS/2 keyboard 
                Characters are displayed on OLED display as they are sent.
                Suitable for keying modern ham radio transceivers!
                See w8bh.net for full tutorial and construction details.
   
 **************************************************************************/ 
 
#include <Wire.h>                            // I2C communications library
#include <Adafruit_GFX.h>                    // graphics library
#include <Adafruit_SSD1306.h>                // OLED display library
#include <FlashStorage.h>                    // loaded with Seeeduino core
#include <EasyKey.h>                         // https://github.com/bhall66/EasyKey 

//===================================  XIAO Data Pin Connections ======================
#define CLOCK_PIN        0                   // PS/2 keyboard clock pin
#define DATA_PIN         1                   // PS/2 keyboard data pin
#define KEYER            2                   // pin for Morse key output
#define SDA              4                   // to OLED display "SDA"
#define SCL              5                   // to OLED display "SCL"
#define AUDIO            9                   // pin attached to piezo element
#define LED              10                  // onboard LED pin

//===================================  Screen Constants =============================
#define SCREEN_WIDTH     128                 // OLED display width, in pixels
#define SCREEN_HEIGHT    64                  // OLED display height, in pixels
#define SCREEN_ADDRESS   0x3C                // OLED display address, in hex

//===================================  Morse Code Constants =============================
#define VERSION          "v1.0"              // software version of this sketch
#define CODESPEED        20                  // default speed in Words per Minute
#define MINSPEED         5                   // lowest code speed
#define MAXSPEED         50                  // highest code speed
#define PITCH            1200                // audio pitch, in Hz
#define MINPITCH         300                 // lowest audio pitch
#define MAXPITCH         2500                // highest audio pitch
#define AUDIO_ON         true                // if true, speaker is active on startup
#define LED_ON           true                // if true, user LED active on startup
#define PAUSE_TOKEN      '*'                 // character used for pausing a macro
#define CALL_TOKEN       '`'                 // call substitution character
#define NAME_TOKEN       '~'                 // name substitution character
#define COUNT_TOKEN      '#'                 // qso counter substitution character
#define MACRO_NUM        12                  // number of text macros
#define MACRO_LENGTH     100                 // number of characters per macro + 1
#define SHORT_SERIAL     false               // if true, send serial<100 as 2 digits
#define LEADING_T        false               // if true, send leading zero as T

//===================================  Default Text Macros =============================
// Specify the default text macros below.
// Remember, they can all be changed at runtime.

//string length "         |10       |20       |30       |40       |50       |60       |70       |80       |90       "
#define MSG_F1  "CQ CQ CQ de W8BH W8BH K" 
#define MSG_F2  "TNX fer call= UR RST *= Name Bruce Bruce es QTH Dayton OH ? Dayton OH= HW CPY? ` de W8BH ("
#define MSG_F3  "5NN #"
#define MSG_F4  "W8BH"
#define MSG_F5  ""
#define MSG_F6  "Rig is Elecraft K3 es Ant is Vertical ="
#define MSG_F7  ""
#define MSG_F8  ""
#define MSG_F9  ""
#define MSG_F10 ""
#define MSG_F11 "TESTING 123"
#define MSG_F12 "` de W8BH ("
  
//===================================  Global variables =============================
EasyKey kbd;
Adafruit_SSD1306 led(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

typedef struct {                             // Data to be stored in flash goes here:
  bool valid;                                // if true, flash contains valid data
  int  pitch;                                // frequency of audio output
  int  codeSpeed;                            // current morse speed, in words/minute
  bool ledOn;                                // if true, morse output goes to user LED
  bool audioOn;                              // if true, morse output goes to speaker
  int  fontSize;                             // currently limited to size 2 or 3
  int  qCount;                               // qso counter (contest serial number)
  char msg[MACRO_NUM][MACRO_LENGTH];         // 12 CW memories of 99 characters each
} FlashObject;

FlashStorage(flash,FlashObject);             // dedicate a portion of flash memory
FlashObject f;                               // instantiate a flash object

int  ditPeriod;                              // length of a dit, in microseconds
int  x=-1,y=0;                               // character position on screen
char scn[10][3];                             // character buffer for screen
bool keyerOn = true;                         // if true, morse output goes to keyer
char call[12] = "";                          // other ham's callsign
char name[20] = "";                          // other ham's name

const byte morse[] = {                       // Each character is encoded into an 8-bit byte:
  0b01001010,        // ! exclamation        // Some punctuation not commonly used
  0b01101101,        // " quotation          // and taken for wikipedia: International Morse Code
  0b0,               // # pound
  0b10110111,        // $ dollar
  0b00000000,        // % percent
  0b00111101,        // & ampersand
  0b01100001,        // ' apostrophe
  0b00110010,        // ( open paren or ~KN
  0b01010010,        // ) close paren
  0b01010111,        // * asterisk or ~SK                       
  0b00110101,        // + plus or ~AR
  0b01001100,        // , comma
  0b01011110,        // - hypen
  0b01010101,        // . period
  0b00110110,        // / slant   
  0b00100000,        // 0                    // Read the bits from RIGHT to left,   
  0b00100001,        // 1                    // with a "1"=dit and "0"=dah
  0b00100011,        // 2                    // example: 2 = 11000 or dit-dit-dah-dah-dah
  0b00100111,        // 3                    // the final bit is always 1 = stop bit.
  0b00101111,        // 4                    // see "sendElements" routine for more info.
  0b00111111,        // 5
  0b00111110,        // 6
  0b00111100,        // 7
  0b00111000,        // 8
  0b00110000,        // 9
  0b01111000,        // : colon
  0b01101010,        // ; semicolon
  0b0,               // <
  0b00101110,        // = equals or ~BT
  0b0,               // >
  0b01110011,        // ? question
  0b01101001,        // @ at
  0b00000101,        // A 
  0b00011110,        // B
  0b00011010,        // C
  0b00001110,        // D
  0b00000011,        // E
  0b00011011,        // F
  0b00001100,        // G
  0b00011111,        // H
  0b00000111,        // I
  0b00010001,        // J
  0b00001010,        // K
  0b00011101,        // L
  0b00000100,        // M
  0b00000110,        // N
  0b00001000,        // O
  0b00011001,        // P
  0b00010100,        // Q
  0b00001101,        // R
  0b00001111,        // S
  0b00000010,        // T
  0b00001011,        // U
  0b00010111,        // V
  0b00001001,        // W
  0b00010110,        // X
  0b00010010,        // Y 
  0b00011100         // Z
};

void ditSpaces(int spaces=1) {               // user specifies #dits to wait
  for (int i=0; i<spaces; i++)               // count the dits...
    delay(ditPeriod);                        // no action, just mark time
}

void dit() {
  digitalWrite(LED_BUILTIN,0);               // turn on built-in LED
  if (keyerOn) digitalWrite(KEYER,1);        // turn on morse key output
  if (f.ledOn) digitalWrite(LED,1);          // turn on user LED
  if (f.audioOn) tone(AUDIO,f.pitch);        // and turn on sound
  ditSpaces();
  digitalWrite(LED_BUILTIN,1);               // turn off built-in LED
  digitalWrite(KEYER,0);                     // turn off morse key output
  digitalWrite(LED,0);                       // turn off user LED
  noTone(AUDIO);                             // and turn off sound
  ditSpaces();                               // space between code elements
}

void dah() {
  digitalWrite(LED_BUILTIN,0);               // turn on built-in LED
  if (keyerOn) digitalWrite(KEYER,1);        // turn on morse key output
  if (f.ledOn) digitalWrite(LED,1);          // turn on user LED
  if (f.audioOn) tone(AUDIO,f.pitch);        // and turn on sound
  ditSpaces(3);                              // length of dah = 3 dits
  digitalWrite(LED_BUILTIN,1);               // turn off built-in LED
  digitalWrite(KEYER,0);                     // turn off morse key output
  digitalWrite(LED,0);                       // turn off user LED
  noTone(AUDIO);                             // and turn off sound
  ditSpaces();                               // space between code elements
}

// the following routine accepts a numberic input, where each bit represents a code element:
// 1=dit and 0=dah.   The elements are read right-to-left.  The left-most bit is a stop bit.
// For example:  5 = binary 0b0101.  The right-most bit is 1 so the first element is a dit.
// The next element to the left is a 0, so a dah follows.  Only a 1 remains after that, so
// the character is complete: dit-dah = morse character 'A'.

void sendElements(int x) {                   // send string of bits as Morse      
  while (x>1) {                              // stop when value is 1 (or less)
    if (x & 1) dit();                        // right-most bit is 1, so dit
    else dah();                              // right-most bit is 0, so dah
    x >>= 1;                                 // rotate bits right
  }
  ditSpaces(2);                              // add inter-character spacing
}

void sendChar(char c) {                      // send a single ASCII char in Morse
  if (c<32) return;                          // ignore control characters
  if (c>96) c -= 32;                         // convert lower case to upper case
  if (c>90) return;                          // not a character
  if (c==32) ditSpaces(5);                   // space between words 
  else sendElements(morse[c-33]);            // send the character
}

void addBigChar(char c) {                    // font size 3: 7x2 char display
  led.setTextSize(3);                        // use font size 3
  if ((x==6)&&(y==1)) {                      // scroll up one line:
    x = 0;                                   // return cursor to start of line
    for (int j=0; j<2; j++)                  // for each row
      for (int i=0; i<7; i++) {              // and for each posn in that row,
        if (j<1)
          scn[i][j]=scn[i][j+1];             // get char from row below.
        else
          scn[i][j] = ' ';                   // fill bottom row with spaces
        led.setCursor(i*18,16+j*24);
        led.print(scn[i][j]);
      }  
  } else {
    x++; if (x>6) {
      x=0;
      y++;
    }
  }
  scn[x][y] = c;
  led.setCursor(x*18,16+y*24);
  led.print(c);
  led.display();
}

void addSmallChar(char c) {                  // font size 2: 10x3 char display
  led.setTextSize(2);                        // use font size 2
  if ((x==9)&&(y==2)) {                      // scroll up one line:
    x = 0;                                   // return cursor to start of line
    for (int j=0; j<3; j++)                  // for each row
      for (int i=0; i<10; i++) {             // and for each posn in that row,
        if (j<2)
          scn[i][j]=scn[i][j+1];             // get char from row below.
        else
          scn[i][j] = ' ';                   // fill bottom row with spaces
        led.setCursor(i*12,(j+1)*16);
        led.print(scn[i][j]);
      }     
  } else {
    x++; if (x>9) {
      x=0;
      y++;
    }
  }
  scn[x][y] = c;
  led.setCursor(x*12,(y+1)*16);
  led.print(c);
  led.display();
}

void addCharToScreen(char c) {
  if (f.fontSize==3) addBigChar(c);
  else addSmallChar(c);
}

void setCodeSpeed(int speed) {
   f.codeSpeed = speed;                      // update codespeed variable
   ditPeriod = 1200/speed;                   // and dit length, in milliseconds
   showStatus();                             // show new speed on status bar
}

void showStatus() {                          // redraw status bar at top of screen
   led.setTextSize(1);                       // status bar uses tiny text
   led.setCursor(2,2);                       // position of Audio icon
   if (f.audioOn) led.print('A');            // On = show an 'A'
   else led.print(' ');                      // Off = no 'A'
   led.setCursor(20,2);                      // positon of LED icon
   led.fillRect(20,2,8,8,BLACK);             // erase previous icon
   if (f.ledOn) 
     led.fillRect(20,2,8,8,WHITE);           // On = filled square
   else led.drawRect(20,2,8,8,WHITE);        // Off = open square
   led.setCursor(40,2);                      // position qso count
   led.print('#');                           // show "#", followed by number
   led.print(f.qCount);                      // followed by number
   led.print("  ");                          // clear previous entry
   led.setCursor(85,2);                      // position speed at top/right
   led.print(f.codeSpeed);                   // show current morse speed
   led.print(" wpm ");                       // and words/minute unit
   led.display();                            // now show all changes
}

void homeScreen() {
   x=-1,y=0;                                 // reset cursor to top, left
   led.fillRect(0,16,127,64-16,BLACK);       // erase existing chars
   led.display();                            // update screen      
   led.setTextSize(f.fontSize);              // set font size
   led.setCursor(0,20);                      // start at top,right                                         
}


bool isChar(char c) {
  return ((c>31)&&(c<127));                  // true if key a..z, 0..9, or symbol
}

bool isDigit(char c) {
  return ((c>='0')&&(c<='9'));               // return true if key is 0..9
}

bool isFKey(char c) {
  return ((c>=PS2_F1)&&(c<=PS2_F12));        // return true if key is F1..F12
}

void roger() {                               // send roger 'R' to LED and Piezo
  keyerOn = false;                           // but NOT to keyer!
  dit(); dah(); dit();                       // send it &
  keyerOn = true;                            // restore keyer output
}

void sendCharacter(char c) {                 // send a single character
  c = toupper(c);                            // convert to upper case
  addCharToScreen(c);                        // show it on screen
  sendChar(c);                               // and send its Morse
}

void sendString (char *ptr) {             
  while (*ptr) {                             // send the entire string, as follows:
     if (*ptr==PAUSE_TOKEN) {                // character is a pause for keyboard entry
       char c=0;                             // get user input
       while (c!=PAUSE_TOKEN) {              // until user types PAUSE character
         if (isChar(c))                      // for regular characters,
            sendCharacter(c);                // display and send them.
         c = kbd.read();                     // get next keyboard character
       }  //while                            // user input done, back to macro!
      
     } else if (*ptr==CALL_TOKEN) {          // this character is callsign marker
         sendString(call);                   // so send callsign
     } else if (*ptr==NAME_TOKEN) {          // this char is name marker 
         sendString(name);                   // so send name
     } else if (*ptr==COUNT_TOKEN) {         // this char is qso counter marker
         sendSerial();                       // so send current count,
         f.qCount++;                         // auto increment count,
         showStatus();                       // and show new count on status bar
     } else {
         sendCharacter(*ptr);                // show&send next character in macro
         if (kbd.available()) {              // check keyboard while sending
           char c=kbd.read();                // oops, user typed something...
           if (c==PS2_ESC) {                 // is it the dreaded Escape key??
             homeScreen();                   // clear the screen
             break;                          // and STOP SENDING
           }
         }
     }
     ptr++;                                  // go to next character in macro
  }
}

void sendMacro(int i) {                      // send text msg to display & Morse
  sendString(f.msg[i]);                      // easy-peasy: get the msg and send
} 

void message(char* s, int msWait=1000) {     // put a message on screen
  click();                                   // audio feedback
  homeScreen();                              // start with empty screen
  led.print(s);                              // display the text 
  led.display();                             // make it so.
  delay(msWait);                             // wait for user to see it
  homeScreen();                              // and clear the message
}

void showReady() {
  message("READY\nREADY");
  roger();
}

void click() {
  tone(AUDIO,2000,3);                        // audio feedback for keypress
}

int getInteger(char* s) {                    // from user via keyboard input
  char c = 0;                                // character received from kbd
  int digit = 0;                             // current digit user entered
  int value = 0;                             // start with value of zero
  homeScreen();                              // erase current screen
  led.println(s);
  led.display();
  while (c != PS2_ENTER) {                   // stop input on <enter>
    c = kbd.read();                          // get keyboard character
    if (c>0) {                               // character was received    
      if (isDigit(c)) {                      // is it a digit 0..9?
        click();                             // audible response
        led.print(c);                        // show digit on the screen   
        led.display();
        digit = c-'0';                       // convert it to number
        value = value*10 + digit;            // and add it to value
      }
    }
  }
  homeScreen();                              // clear screen when done
  return value;
}

void sendDigit(int digit) {
  if ((digit>=0)&&(digit<=9))                // must be 0..9
    sendCharacter('0'+digit);                // convert to char & send it.
}

void sendSerial() {                          // SEND QSO COUNT (3 digits)
  int temp = f.qCount;                       // create temporary variable
  int hundreds = temp/100;                   // get hundreds digit
  temp -= hundreds*100;                      // remove hundreds digit
  int tens = temp/10;                        // get tens digit
  temp -= tens*10;                           // get ones digit
  if ((f.qCount>99)||(!SHORT_SERIAL)) {      // need to send hundreds digit?
    if ((hundreds==0)&&(LEADING_T))          // check for leading T         
      sendCharacter('T');                    // send hundreds leading T
    else sendDigit(hundreds);                // otherwise send hundreds digit
  }
  if ((f.qCount<10)&&(LEADING_T))            // leading T for tens spot?
    sendCharacter('T');                      // send leading T    
  else sendDigit(tens);                      // otherwise send tens digit
  sendDigit(temp);                           // send ones digit
}

void setPitch() {
  char c = 0;                                // character received from kbd
  int value = f.pitch;                       // start with current pitch
  homeScreen();                              // erase current screen
  led.print(value);
  led.display();
  while (c != PS2_ENTER) {                   // stop input on <enter>
    if (kbd.available()) {
      c = kbd.read();                        // get keyboard character
      if ((c==PS2_UPARROW)||                 // sllow only up,down,pageUp,pageDn
      (c==PS2_DOWNARROW)||
      (c==PS2_PAGEUP)||
      (c==PS2_PAGEDOWN)) {
        if (c==PS2_UPARROW)                  // Up: increase by 10 Hz  
          value += 10;
        else if (c==PS2_DOWNARROW)           // Dn: decrease by 10 Hz
          value -= 10;
        else if (c==PS2_PAGEUP)              // PgUp: increase by 100 Hz
          value += 100;
        else value -= 100;                   // PgDn: decrease by 100 Hz
        value = constrain(value,                
          MINPITCH,MAXPITCH);                // keep pitch within bounds
        homeScreen();
        led.print(value);                    // show the updated pitch
        led.display();
        tone(AUDIO,value,90);                // and let user hear it.
        delay(100);
      }
    }   
  }
  f.pitch = value;                           // use updated pitch
  homeScreen();                              // clear screen when done
  roger();
}

void getString(char *s, int maxLen) {
  int len =0;  
  char c=0;
  while ((len<=maxLen)&&(c!=PS2_ENTER)) {    // stop input on enter key
    c = toupper(kbd.read());                 // get keyboard character
    if (isChar(c)) {                         // consider only regular characters    
        click();                             // audible response
        addCharToScreen(c);                  // show latest character
        s[len++] = c;                        // and add it to the string
    }
  } // while
  s[len]=0;                                  // add terminating /0
  homeScreen();
  roger();
}

void setMacro() {
  char s[MACRO_LENGTH] = "";                 // start with empty string.
  int len =0;                                // current length of macro
  message("Set Macro");                      // erase current screen
  message("Enter text");
  message("Then F-key");                     // give user a few instructions
  char c=0;
  while (!isFKey(c)) {                       // stop input on function key
    c = toupper(kbd.read());                 // get keyboard character
    if (isChar(c)) {                         // consider only regular characters    
        click();                             // audible response
        addCharToScreen(c);                  // show latest character
        s[len++] = c;                        // and add it to the string
    }
  } // while
  s[len]=0;                                  // add terminating /0
  strcpy(f.msg[c-PS2_F1],s);                 // save macro in volatile memory
  homeScreen();
  roger();
}

void setDefaults() {                         // Reset all user settings to defaults
  f.pitch = PITCH;                           // frequency of audio output
  setCodeSpeed(CODESPEED);                   // current morse speed, in words/minute
  f.ledOn = LED_ON;                          // if true, morse output goes to user LED
  f.audioOn = AUDIO_ON;                      // if true, morse output goes to speaker
  f.fontSize = 2;                            // currently limited to 2 or 3
  f.qCount = 1;                              // start qso counter at 1
  strcpy(f.msg[0],MSG_F1);                   // initialize the CW messages F1..F12
  strcpy(f.msg[1],MSG_F2);
  strcpy(f.msg[2],MSG_F3);
  strcpy(f.msg[3],MSG_F4);
  strcpy(f.msg[4],MSG_F5);
  strcpy(f.msg[5],MSG_F6);
  strcpy(f.msg[6],MSG_F7);
  strcpy(f.msg[7],MSG_F8);
  strcpy(f.msg[8],MSG_F9);
  strcpy(f.msg[9],MSG_F10);
  strcpy(f.msg[10],MSG_F11);
  strcpy(f.msg[11],MSG_F12);
}

void loadParams() {                          // LOAD USER SETTINGS FROM FLASH
  f = flash.read();                          // get all macros & settings from flash
  if (!f.valid) setDefaults();               // not saved yet, so use defaults instead
  setCodeSpeed(f.codeSpeed);                 // change speed to saved value
}

void saveParams() {                          // SAVE USER SETTINGS TO FLASH
  message("SAVING"); roger();                // inform user going to save settings now
  f.valid = true;                            // flag the flash contents as valid data
  flash.write(f);                            // and save the settings.
}

void toggleLED() {
  f.ledOn = !f.ledOn;                        // toggle the flag
  showStatus();                              // show updated flag on status bar
  if (f.ledOn) message("LED ON");            // and inform user
  else message("LED OFF"); 
}

void toggleAudio() {
  f.audioOn = !f.audioOn;                    // toggle the flag
  showStatus();                              // show updated flag on status bar
  if (f.audioOn) message("AUDIO ON");        // and inform user
  else message("AUDIO OFF");   
}

void toggleText() {                          // TOGGLER SCREEN TEXT SIZE
  f.fontSize = (f.fontSize==3)?2:3;          // toggle between 2 & 3                           
  showReady();                               // and show what it looks like
}

void speedUp() {                             // INCREASE MORSE SPEED:
  int i = f.codeSpeed;                       // get current speed
  if (i<MAXSPEED) {                          // if less than maximum,                              
    setCodeSpeed(++i);                       // bump it up
    click();                                 // audible feedback
  }  
}

void slowDown() {                            // DECRESE MORSE SPEED:
  int i = f.codeSpeed;                       // get current speed                  
  if (i>MINSPEED) {                          // if greater than minumum, 
    setCodeSpeed(--i);                       // decrease it 
    click();                                 // audible feedback
  }  
}

void showSpeed() {                           // SHOW WPM CODESPEED
  char s[4];                                 // if you cant read the statusbar!
  itoa(f.codeSpeed,s,10);                    // convert int to string
  message(s);                                // and show it to the OM
}

void decSerial() {                           // DECREASE QSO COUNTER
  if (f.qCount>1) {                          // dont go below 1
    f.qCount--;                              // do it
    showStatus();                            // show it
    click();                                 // give audio feedback
  }
}

void incSerial() {                           // INCREASE QSO COUNTER
  f.qCount++;                                // do it
  showStatus();                              // show it
  click();                                   // give audio feedback
  name[0]=0; call[0]=0;                      // erase name and call.
}

void setSerial() {                           // SET QSO COUNTER
  int i = getInteger("QSO #?");              // ask for number
  if (i==0) i=1;                             // if no input, start at 1
  if (i>999) i=1;                            // keep # less than 1000
  f.qCount = i;                              // update qso counter
  showStatus();                              // show new value on status bar
  roger();                                   // acknowledge update
}

void setTone() {                             // SET AUDIO TONE
  f.pitch = getInteger("TONE?");             // ask for value in Hz
  roger();                                   // and sound it out with 'r'
}

void getCall() {                             // get a callsign from user        
  message("Call?");
  getString(call,12);                        
}

void getName() {                             // get a name from user
  message("Name?");
  getString(name,20);                        
}

void reset() {                               // USER WANTS TO RESET
  message("Reset");                         
  setDefaults();                             // use all default params
  showStatus();                              // update status bar
  roger();                                   // and demo new pitch & speed
}


void setup()   {   
  kbd.begin(DATA_PIN, CLOCK_PIN);            // initialize PS/2 keyboard         
  led.begin(SSD1306_SWITCHCAPVCC,            // initialize OLED display
    SCREEN_ADDRESS);
  led.clearDisplay();                        // start with blank screen
  led.drawLine(0,14,127,14,WHITE);           // create status line @ screen top
  led.setTextColor(WHITE,BLACK);             // use black on white text
  pinMode(LED_BUILTIN,OUTPUT);               // allow control of built-in LED
  pinMode(LED,OUTPUT);                       // allow control of user LED
  digitalWrite(LED,0);                       // turn LED off
  pinMode(AUDIO,OUTPUT);                     // allow control of audio
  digitalWrite(AUDIO,0);                     // turn off speaker output
  pinMode(KEYER,OUTPUT);                     // allow control of Morse Output
  digitalWrite(KEYER,0);                     // turn off Morse output                 
  loadParams();                              // get stored variables
  showReady();                               // quick startup message
}

void loop() {                                // THIS IS THE SKETCH:
  if (kbd.available()) {                     // when the user types something,
    char c = kbd.read();                     // get the keyboard character
    switch(c) {                              // and deal with it.
                                             // look for commands first.
      case PS2_ENTER:       homeScreen();  break;
      case PS2_HOME:        homeScreen();  break;
      case PS2_ESC:         homeScreen();  break;
      case PS2_END:         incSerial();   break;
      case PS2_PAGEUP:      toggleAudio(); break;
      case PS2_PAGEDOWN:    toggleLED();   break;
      case PS2_LEFTARROW:   decSerial();   break;
      case PS2_RIGHTARROW:  incSerial();   break;
      case PS2_UPARROW:     speedUp();     break;
      case PS2_DOWNARROW:   slowDown();    break; 
      case PS2_CTRL_A:      toggleAudio(); break;
      case PS2_CTRL_C:      getCall();     break;
      case PS2_CTRL_F:      setMacro();    break;
      case PS2_CTRL_L:      toggleLED();   break;
      case PS2_CTRL_N:      getName();     break;
      case PS2_CTRL_O:      toggleText();  break;
      case PS2_CTRL_P:      setPitch();    break;
      case PS2_CTRL_Q:      setSerial();   break;
      case PS2_CTRL_R:      reset();       break;
      case PS2_CTRL_S:      saveParams();  break;
      case PS2_CTRL_T:      setTone();     break;
      case PS2_CTRL_W:      showSpeed();   break;
      case PS2_CTRL_V:      message(VERSION); break;
      default:
        if (isFKey(c)) sendMacro(c-PS2_F1);  // then check if key is a fn key.
        else if (c==CALL_TOKEN)              // shortcut key for name
          sendString(call);
        else if (c==NAME_TOKEN)              // shortcut key for callsign
          sendString(name);
        else sendCharacter(c);               // nope, just a regular character. 
    }
  }
}

# include <Arduino.h>
# include "Mouse.h"	  		// For Arduino Mouse
# include "Joystick.h"		// For Joystick control (mheironimus/Joystick)
/* 
* -------------------------------------------------------------------------
* Interface Atari ST Keyboard to USB HID Keyboard
* -------------------------------------------------------------------------
* Initial idea and some original code provided by user 'joska' of 
* http://www.atari-forum.com - license unknown
* -------------------------------------------------------------------------
* Copyright Kevin Peat 2017
* kevin@kevinpeat.com
* My changes and additions are licensed public domain
* -------------------------------------------------------------------------
* Developed for use with an Arduino Leonardo as it is able to act directly 
* as a USB keyboard controller so doesn't require the Arduino firmware to 
* be modified as some of the other Arduinos (eg. Uno) would do
* -------------------------------------------------------------------------
*/

#define DEBUG

#define ArduinoKeyboard_Source 

#ifdef ArduinoKeyboard_Source
# include <Keyboard.h>
#else
# include <Keyboard_Real.h>
#endif

/*
For 7 pin Atari keyboards, you can use the floppy LED as a caps lock LED
Unsupported on RJ11 external Keyboards: Mega ST, Mega STE, TT
*/
#define UseFloppyCapsLock  // Define, to use the floppy LED as capslock LED

// ST keyboard Floppy LED pin
const int ST_FLOPPY_LED = 9; // D9 for Arduino (Pro) Micro

// ST keyboard reset pin
const int ST_KB_RESET = 8; // D8 for Arduino (Pro) Micro

// Atari modifier key codes
const uint8_t ST_LEFT_CTRL = 0x1D;
const uint8_t ST_LEFT_SHIFT = 0x2A;
const uint8_t ST_LEFT_ALT = 0x38;
const uint8_t ST_RIGHT_SHIFT = 0x36;
const uint8_t ST_CAPS_LOCK = 0x3A;

/*
The Arduino Keyboard.h subtracts 136 from all keyboard codes above 136.
To get the real code, add 136 to it.
It also subtracts 128 from all keycodes from 128-135.
If using a proper keyboard library, set to 0
*/
#ifdef ArduinoKeyboard_Source 
const int KeyboardStupid = 0x88;
#else
const int KeyboardStupid = 0x00;
#endif

const uint8_t Key_BackSlashPipe = 0x31 + KeyboardStupid;
const uint8_t Key_TildeHash_NonUS = 0x32 + KeyboardStupid;
const uint8_t Key_GraveAccentTilde = 0x35 + KeyboardStupid;
const uint8_t NUM_Slash = 0x54 + KeyboardStupid;
const uint8_t NUM_Star = 0x55 + KeyboardStupid;
const uint8_t NUM_Minus = 0x56 + KeyboardStupid;
const uint8_t NUM_Plus = 0x57 + KeyboardStupid;
const uint8_t NUM_Enter = 0x58 + KeyboardStupid;
const uint8_t NUM_1 = 0x59 + KeyboardStupid;
const uint8_t NUM_2 = 0x5A + KeyboardStupid;
const uint8_t NUM_3 = 0x5B + KeyboardStupid;
const uint8_t NUM_4 = 0x5C + KeyboardStupid;
const uint8_t NUM_5 = 0x5D + KeyboardStupid;
const uint8_t NUM_6 = 0x5E + KeyboardStupid;
const uint8_t NUM_7 = 0x5F + KeyboardStupid;
const uint8_t NUM_8 = 0x60 + KeyboardStupid;
const uint8_t NUM_9 = 0x61 + KeyboardStupid;
const uint8_t NUM_0 = 0x62 + KeyboardStupid;
const uint8_t NUM_Period = 0x63 + KeyboardStupid;
const uint8_t Key_BackSlashPipe_NonUS = 0x64 + KeyboardStupid;


// Keyboard auto-repeat
static uint8_t last_make;    // Last make char
static unsigned long last_make_time;  // Last make time (milliseconds)
int auto_repeat_delay = 500; // Keyboard auto-repeat delay (milliseconds)
int auto_repeat_rate = 25;   // Keyboard auto-repeat rate (milliseconds)

// Key scancodes
// Use ST scancode as index to find the corresponding USB scancode.
// Make-codes are single byte, with some exceptions.
// These are escaped with a 0xe0 byte, when this appear in this table a
// simple switch is used to look up the correct scancode.
// All break codes are the same as the scancodes, but are prefixed with 0xf0.
// The escaped keys are first escaped, then 0xf0, then scancode.

/* 
  Arduino treats values below 128 as printable and uses an ascii lookup table. 
  Add 136 to the value to overcome this.
  Because of this, keycodes above 120 cannot be used.
https://forum.arduino.cc/index.php?topic=179548.0
https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf
*/

uint8_t scanCodes[] =
{
  0x00, // (Nothing)
  KEY_ESC, // Esc
  0x31, // 1
  0x32, // 2
  0x33, // 3
  0x34, // 4
  0x35, // 5
  0x36, // 6
  0x37, // 7
  0x38, // 8
  0x39, // 9
  0x30, // 0
  0x2D, // -
  0x3D, // == (Mapped to =)
  KEY_BACKSPACE, // Backspace
  KEY_TAB, // Tab
  0x71, // q
  0x77, // w
  0x65, // e
  0x72, // r
  0x74, // t
  0x79, // y
  0x75, // u
  0x69, // i
  0x6F, // o
  0x70, // p
  0x5B, // [
  0x5D, // ]
  KEY_RETURN, // Enter
  KEY_LEFT_CTRL, // Control
  0x61, // a
  0x73, // s
  0x64, // d
  0x66, // f
  0x67, // g
  0x68, // h
  0x6A, // j
  0x6B, // k
  0x6C, // l
  0x3B, // ;
  0x27, // ' (Mapped to '")
  Key_GraveAccentTilde, // # 
  KEY_LEFT_SHIFT, // Lshift
  Key_TildeHash_NonUS, // #~
  0x7A, // z
  0x78, // x
  0x63, // c
  0x76, // v
  0x62, // b
  0x6E, // n
  0x6D, // m
  0x2C, // ,
  0x2E, // .
  0x2F, // /
  KEY_RIGHT_SHIFT, // Rshift
  0x37, // (Not used)
  KEY_LEFT_ALT, // Alternate
  0x20, // Space
  KEY_CAPS_LOCK, // CapsLock
  KEY_F1, // F1
  KEY_F2, // F2
  KEY_F3, // F3
  KEY_F4, // F4
  KEY_F5, // F5
  KEY_F6, // F6
  KEY_F7, // F7
  KEY_F8, // F8
  KEY_F9, // F9
  KEY_F10, // F10
  0x45, // (Not used)
  0x46, // (Not used)
  KEY_HOME, // Clr/Home
  KEY_UP_ARROW, // Up Arrow
  0x49, // (Not used)
  NUM_Minus, // Numeric Pad -
  KEY_LEFT_ARROW, // Left Arrow
  0x4c, // (Not used)
  KEY_RIGHT_ARROW, // Right Arrow
  NUM_Plus, // Numeric Pad +
  0x4f, // (Not used)
  KEY_DOWN_ARROW, // Down Arrow
  0x51, // (Not used)
  KEY_INSERT, // Insert
  KEY_DELETE, // Delete
  0x54, // (Not used)
  0x55, // (Not used)
  0x56, // (Not used)
  0x57, // (Not used)
  0x58, // (Not used)
  0x59, // (Not used)
  0x5a, // (Not used)
  0x5b, // (Not used)
  0x5c, // (Not used)
  0x5d, // (Not used)
  0x5e, // (Not used)
  0x5f, // (Not used)
  Key_BackSlashPipe_NonUS, // Keyboard Non-US \ and |
  KEY_F12, // Undo (Mapped to F12)
  KEY_F11, // Help (Mapped to F11)
  0x28, // Numeric Pad ( [Mapped to ( since the Arduino keyboard can't map keycode 182]
  0x29, // Numeric Pad ) [Mapped to ) since the Arduino keyboard can't map keycode 183]
  NUM_Slash,  // Numeric Pad /
  NUM_Star,   // Numeric Pad *
  NUM_7,      // Numeric Pad 7
  NUM_8,      // Numeric Pad 8
  NUM_9,      // Numeric Pad 9
  NUM_4,      // Numeric Pad 4
  NUM_5,      // Numeric Pad 5
  NUM_6,      // Numeric Pad 6
  NUM_1,      // Numeric Pad 1
  NUM_2,      // Numeric Pad 2
  NUM_3,      // Numeric Pad 3
  NUM_0,      // Numeric Pad 0
  NUM_Period, // Numeric Pad .
  NUM_Enter   // Numeric Pad Enter
};

/*
Switch on/off the floppy drive led via the ST_FLOPPY_LED Pin.
*/
void TurnOnOfFloppyLED(int HighLow)
{
  pinMode(ST_FLOPPY_LED, OUTPUT);
  digitalWrite(ST_FLOPPY_LED,HighLow); // For output, HIGH is 5v, LOW is 0v
}

// Reset ST Keyboard
void reset_st_keyboard(void)
{
  Serial1.print(0x80);
  Serial1.print(1);
  pinMode(ST_KB_RESET, OUTPUT);
  digitalWrite(ST_KB_RESET, HIGH);
  delay(20);
  digitalWrite(ST_KB_RESET, LOW);
  delay(20);
  digitalWrite(ST_KB_RESET, HIGH);
}

// Process modifier keypresses
boolean process_modifier(uint8_t key)
{
  // Modifier key press  
  switch (key)
    {
      case ST_LEFT_CTRL:
        Keyboard.press(KEY_LEFT_CTRL);
        return true;
      case ST_LEFT_SHIFT:
        Keyboard.press(KEY_LEFT_SHIFT);
        return true;
      case ST_LEFT_ALT:
        Keyboard.press(KEY_LEFT_ALT);
        return true;
      case ST_RIGHT_SHIFT:
        Keyboard.press(KEY_RIGHT_SHIFT);
        return true;        
      case ST_CAPS_LOCK:
        Keyboard.press(KEY_CAPS_LOCK);
        #ifdef UseFloppyCapsLock
          // Switch off Floppy LED
          TurnOnOfFloppyLED(HIGH); // For output, HIGH is 5v, LOW is 0v
        #endif
        return true;
    }

  // Modifier key release
  switch (key & 0x7f)
    {
      case ST_LEFT_CTRL:
        Keyboard.release(KEY_LEFT_CTRL);
        return true;
      case ST_LEFT_SHIFT:
        Keyboard.release(KEY_LEFT_SHIFT);
        return true;
      case ST_LEFT_ALT:
        Keyboard.release(KEY_LEFT_ALT);
        return true;
      case ST_RIGHT_SHIFT:
        Keyboard.release(KEY_RIGHT_SHIFT);
        return true;        
      case ST_CAPS_LOCK:
        Keyboard.release(KEY_CAPS_LOCK);
        #ifdef UseFloppyCapsLock
          // Switch off Floppy LED
          TurnOnOfFloppyLED(LOW); // For output, HIGH is 5v, LOW is 0v
        #endif
        return true;
    }
  
  return false;  
}

// Send code for escaped Atari keypresses
void send_escaped_key(uint8_t key)
{
  Keyboard.press(key);
  delay(20);
  Keyboard.release(key); 
}


// Convert from ST scancode to PC scancode
void convert_scancode(uint8_t key)
{
  uint8_t pc_code = scanCodes[key & 0x7f];
  uint8_t escaped = (pc_code == 0xe0 ? true:false);
  
#ifdef DEBUG
  uint8_t break_code = key & 0x80;

    Serial.print("Atari scancode: ");
    Serial.println(key, DEC);
    Serial.print("PC scancode: ");
    Serial.println(pc_code, DEC);
    Serial.print("Break code: ");
    Serial.println(break_code, DEC);
    Serial.print("Escaped: ");
    Serial.println(escaped, DEC);
#endif
  
  // Handle modifier key presses
  if (process_modifier(key)) return;

  // Special handling required for escaped keypresses
  if (escaped)
  {
    switch (key & 0x7f)
    {
      case 0x48: // Up arrow
        send_escaped_key(KEY_UP_ARROW);
        break;
      case 0x4b: // Left arrow
        send_escaped_key(KEY_LEFT_ARROW);
        break;
      case 0x4d: // Right arrow
        send_escaped_key(KEY_RIGHT_ARROW);
        break;
      case 0x50: // Down arrow
        send_escaped_key(KEY_DOWN_ARROW);
        break;
      case 0x52: // Insert
        send_escaped_key(KEY_INSERT);
        break;
      case 0x53: // Delete
        send_escaped_key(KEY_DELETE);
        break;
      case 0x47: // Clr/Home
        send_escaped_key(KEY_HOME);
        break;
      case 0x65: // Num /
        send_escaped_key(NUM_Slash);
        break;
      case 0x72: // Num Enter
        send_escaped_key(NUM_Enter);
        break;
      case 0x2b: // Tilde
        send_escaped_key(0x23);
        break;
      case 0x62: // Help
        send_escaped_key(KEY_F1);
        break;
    }
  }
  else
  {
    Keyboard.write(pc_code);
  }
}


// Process each keypress
void process_keypress(uint8_t key)
{
  // Keypress
  if (((key & 0x7f) > 0) && ((key & 0x7f) < 0x73))
  {
    // Break codes (other than modifiers) do not need to be sent 
    // to the PC as the Arduino keyboard interface handles that
    if (key & 0x80) // Break
    {
      last_make = 0;
      last_make_time = 0;
      process_modifier(key);
    }
    else // Make
    {
      last_make = key;
      last_make_time = millis();
      convert_scancode(key);
    }
  }
}


// Keyboard auto repeat
void auto_repeat(void)
{
  static unsigned long last_repeat;
  static byte key_repeating;  // True if key being repeated
  
  // Don't want to repeat modifiers  
  switch (last_make)
  {
    case ST_LEFT_CTRL:
    case ST_LEFT_SHIFT:
    case ST_RIGHT_SHIFT:
    case ST_LEFT_ALT:
    case ST_CAPS_LOCK:
    case 0x00: // No key held down
      key_repeating = false;
      return;
  }

  // Delay to first repeat  
  if (!key_repeating && (millis() - last_make_time > auto_repeat_delay))
  {
    key_repeating = true;
    last_repeat = millis();
    return;
  }

  // Start repeating
  if (key_repeating && (millis() - last_repeat > auto_repeat_rate))
  {
    last_repeat = millis();
    convert_scancode(last_make);
  }  
}


void setup(void)
{
  // Initialize keyboard:
  Keyboard.begin();
  
  // Open serial port from Atari keyboard
  Serial1.begin(7812);  // Serial speed is locked to 7812.5 bits/s

#ifdef DEBUG
  // Open serial port to PC
  Serial.begin(9600);
#endif

  // Reset ST keyboard
  delay(200);
  reset_st_keyboard();
  delay(200);

  // Empty serial buffer before starting
  while(Serial1.available() > 0) {
    Serial1.read();
  }
}


void loop()
{
  // Process incoming Atari keypresses
  if (Serial1.available() > 0) {
  process_keypress(Serial1.read());
  }

   // Handle keyboard auto-repeat
   auto_repeat();
}
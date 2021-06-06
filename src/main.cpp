#include <Arduino.h>
/* 
* -------------------------------------------------------------------------
* Interface Atari ST Keyboard to USB HID Keyboard
* -------------------------------------------------------------------------
* Initial idea and some original code provided by user 'joska' of 
* http://www.atari-forum.com - license unknown
* Based on Arduino code from Kevin Peat 2017  -  kevin@kevinpeat.com
* -------------------------------------------------------------------------
* Copyright Michael Farrell 2021
* mikerochip(at)hotmail.com
*
* -------------------------------------------------------------------------
* Arduino USB HID interface for ATmega32U4 based controllers:
* Leonardo, Micro, Pro Micro, etc.
* Supports 6 pin RJ11, 7 pin and 18 pin Atari Keyboards from:
* Atari ST, STF, STM, STFM, STE, Mega ST, Mega STE, TT, Falcon
* Includes UK and US keyboard layout.
* Optionally supports:
* Joystick Player 1. (Should be plugged in before connecting keyboard)
* Digital Pin 8 can be used to reset the 7 / 18 pin keyboards.
* Digital Pin 9 can be connected to use the floppy drive led as a capslock led.
* TODO: Figure out mouse / Joystick Player 2
* -------------------------------------------------------------------------
*/

// Define to include console output.
// #define DEBUG

// Define this, to use the normal Arduino Keyboard library.
#define ArduinoKeyboard_Source

#ifdef ArduinoKeyboard_Source
#include <Keyboard.h>
#else
#include <Keyboard_Real.h>
#endif

// Define this, to enable Joysticks and Mouse support, with the ST Keyboard
// #define JoystickMouse_Support

#ifdef JoystickMouse_Support
#include "Joystick.h" // For Joystick control (mheironimus/Joystick)
#include "Mouse.h"    // For Arduino Mouse
#endif

// define, to use the UK keyboard layout.
#define UK_ATARI_KEYBOARD

// define, to use the US Keyboard layout.
// #define US_ATARI_KEYBOARD

// define, to use the German keyboard layout.
// #define DE_ATARI_KEYBOARD

/*
For 7 pin Atari keyboards, you can use the floppy LED as a caps lock LED
Unsupported on RJ11 external Keyboards: Mega ST, Mega STE, TT
*/
// #define UseFloppyCapsLock // Define, to use the floppy LED as capslock LED

#ifdef UseFloppyCapsLock
// ST keyboard Floppy LED pin
const int ST_FLOPPY_LED = 9; // D9 for Arduino (Pro) Micro
#endif

#ifdef JoystickMouse_Support
#define MAXBUTTONS 1     // The number of buttons you are using up to 9, for 2 player mode.
#define JOYSTICK_COUNT 2 // The number of joysticks connected to the Arduino

//Create Joystick objects
#if JOYSTICK_COUNT == 1
Joystick_ Joystick[JOYSTICK_COUNT] =
    {
        Joystick_(0x03, JOYSTICK_TYPE_GAMEPAD, MAXBUTTONS, 0, true, true, false, false, false, false, false, false, false, false, false)};
#endif

#if JOYSTICK_COUNT == 2
Joystick_ Joystick[JOYSTICK_COUNT] =
    {
        Joystick_(0x03, JOYSTICK_TYPE_GAMEPAD, MAXBUTTONS, 0, true, true, false, false, false, false, false, false, false, false, false),
        Joystick_(0x04, JOYSTICK_TYPE_GAMEPAD, MAXBUTTONS, 0, true, true, false, false, false, false, false, false, false, false, false),
};
#endif
/* Use ID 03 for first joystick, 04 for second joystick
   Joystick type: gamepad / digital joystick
   MAXBUTTONS: Button Count (Both sticks will be the same)
   0:  Hat Switch Count
   X Axis. We need at least two axes. 
   Y Axis. Second axis used.
   No Z Axis  
   No Rx
   No Ry
   No Rz
   No rudder
   No throttle      
   No accelerator
   No brake
   No steering
*/

const int16_t Joy0 = 0xFF;             // Initial Serial char for Joystick 0 Commands
const int16_t Joy1 = 0xF2;             // Initial Serial char for Joystick 1 Commands
const uint8_t JoyBtnReleaseAll = 0xF8; // Initial Serial char for Release Both Joystick/Mouse Fire buttons
const uint8_t JoyBtnPress0 = 0xF9;     // Initial Serial char for Press Joystick 0 Fire
const uint8_t JoyBtnPress1 = 0xFA;     // Initial Serial char for Press Joystick 1 Fire
const uint8_t JoyBtnPressBoth = 0xFB;  // Initial Serial char for Press Joystick 0+1 Fire together
#endif

// ST keyboard reset pin
const int ST_KB_RESET = 8; // D8 for Arduino (Pro) Micro

const uint8_t FirstKeypress = 0x01;
const uint8_t LastKeypress = 0x72;

uint8_t No_Op_Count = 0x00; //Loop, to not process the next serial bytes, since they're part of the previous instruction
uint8_t Prev_Serial_Byte = 0x00;
int16_t ActionByte = 0x00;

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

/* 
  Arduino treats values below 128 as printable and uses an ascii lookup table. 
  Add 136 to the value to overcome this.
  Because of this, keycodes above 120 cannot be used.
  https://forum.arduino.cc/index.php?topic=179548.0
  https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf
*/

#ifdef US_ATARI_KEYBOARD
// US ST Layout, for US USB HID Keyboard
uint8_t scanCodes[] =
    {
        0x00,                 // (Nothing)
        KEY_ESC,              // Esc
        0x31,                 // 1
        0x32,                 // 2
        0x33,                 // 3
        0x34,                 // 4
        0x35,                 // 5
        0x36,                 // 6
        0x37,                 // 7
        0x38,                 // 8
        0x39,                 // 9
        0x30,                 // 0
        0x2D,                 // -
        0x3D,                 // == (Mapped to =)
        KEY_BACKSPACE,        // Backspace
        KEY_TAB,              // Tab
        0x71,                 // q
        0x77,                 // w
        0x65,                 // e
        0x72,                 // r
        0x74,                 // t
        0x79,                 // y
        0x75,                 // u
        0x69,                 // i
        0x6F,                 // o
        0x70,                 // p
        0x5B,                 // [
        0x5D,                 // ]
        KEY_RETURN,           // Enter
        KEY_LEFT_CTRL,        // Control
        0x61,                 // a
        0x73,                 // s
        0x64,                 // d
        0x66,                 // f
        0x67,                 // g
        0x68,                 // h
        0x6A,                 // j
        0x6B,                 // k
        0x6C,                 // l
        0x3B,                 // ;
        0x27,                 // ' (Mapped to '")
        Key_GraveAccentTilde, // ` and ~
        KEY_LEFT_SHIFT,       // Lshift
        Key_BackSlashPipe,    // \ and |
        0x7A,                 // z
        0x78,                 // x
        0x63,                 // c
        0x76,                 // v
        0x62,                 // b
        0x6E,                 // n
        0x6D,                 // m
        0x2C,                 // ,
        0x2E,                 // .
        0x2F,                 // /
        KEY_RIGHT_SHIFT,      // Rshift
        0x37,                 // (Not used)
        KEY_LEFT_ALT,         // Alternate
        0x20,                 // Space
        KEY_CAPS_LOCK,        // CapsLock
        KEY_F1,               // F1
        KEY_F2,               // F2
        KEY_F3,               // F3
        KEY_F4,               // F4
        KEY_F5,               // F5
        KEY_F6,               // F6
        KEY_F7,               // F7
        KEY_F8,               // F8
        KEY_F9,               // F9
        KEY_F10,              // F10
        0x45,                 // (Not used)
        0x46,                 // (Not used)
        KEY_HOME,             // Clr/Home
        KEY_UP_ARROW,         // Up Arrow
        0x49,                 // (Not used)
        NUM_Minus,            // Numeric Pad -
        KEY_LEFT_ARROW,       // Left Arrow
        0x4c,                 // (Not used)
        KEY_RIGHT_ARROW,      // Right Arrow
        NUM_Plus,             // Numeric Pad +
        0x4f,                 // (Not used)
        KEY_DOWN_ARROW,       // Down Arrow
        0x51,                 // (Not used)
        KEY_INSERT,           // Insert
        KEY_DELETE,           // Delete
        0x54,                 // (Not used)
        0x55,                 // (Not used)
        0x56,                 // (Not used)
        0x57,                 // (Not used)
        0x58,                 // (Not used)
        0x59,                 // (Not used)
        0x5a,                 // (Not used)
        0x5b,                 // (Not used)
        0x5c,                 // (Not used)
        0x5d,                 // (Not used)
        0x5e,                 // (Not used)
        0x5f,                 // (Not used)
        0x5C,                 // ISO Key
        KEY_F12,              // Undo (Mapped to F12)
        KEY_F11,              // Help (Mapped to F11)
        0x28,                 // Numeric Pad ( [Mapped to ( since the Arduino keyboard can't map keycode 182]
        0x29,                 // Numeric Pad ) [Mapped to ) since the Arduino keyboard can't map keycode 183]
        NUM_Slash,            // Numeric Pad /
        NUM_Star,             // Numeric Pad *
        NUM_7,                // Numeric Pad 7
        NUM_8,                // Numeric Pad 8
        NUM_9,                // Numeric Pad 9
        NUM_4,                // Numeric Pad 4
        NUM_5,                // Numeric Pad 5
        NUM_6,                // Numeric Pad 6
        NUM_1,                // Numeric Pad 1
        NUM_2,                // Numeric Pad 2
        NUM_3,                // Numeric Pad 3
        NUM_0,                // Numeric Pad 0
        NUM_Period,           // Numeric Pad .
        NUM_Enter             // Numeric Pad Enter
};
#endif

#ifdef UK_ATARI_KEYBOARD
// UK ST Layout, for UK British USB HID Keyboard
uint8_t scanCodes[] =
    {
        0x00,                    // (Nothing)
        KEY_ESC,                 // Esc
        0x31,                    // 1
        0x32,                    // 2
        0x33,                    // 3
        0x34,                    // 4
        0x35,                    // 5
        0x36,                    // 6
        0x37,                    // 7
        0x38,                    // 8
        0x39,                    // 9
        0x30,                    // 0
        0x2D,                    // -
        0x3D,                    // == (Mapped to =)
        KEY_BACKSPACE,           // Backspace
        KEY_TAB,                 // Tab
        0x71,                    // q
        0x77,                    // w
        0x65,                    // e
        0x72,                    // r
        0x74,                    // t
        0x79,                    // y
        0x75,                    // u
        0x69,                    // i
        0x6F,                    // o
        0x70,                    // p
        0x5B,                    // [
        0x5D,                    // ]
        KEY_RETURN,              // Enter
        KEY_LEFT_CTRL,           // Control
        0x61,                    // a
        0x73,                    // s
        0x64,                    // d
        0x66,                    // f
        0x67,                    // g
        0x68,                    // h
        0x6A,                    // j
        0x6B,                    // k
        0x6C,                    // l
        0x3B,                    // ;
        0x27,                    // ' (Mapped to '")
        Key_GraveAccentTilde,    // #
        KEY_LEFT_SHIFT,          // Lshift
        Key_TildeHash_NonUS,     // #~
        0x7A,                    // z
        0x78,                    // x
        0x63,                    // c
        0x76,                    // v
        0x62,                    // b
        0x6E,                    // n
        0x6D,                    // m
        0x2C,                    // ,
        0x2E,                    // .
        0x2F,                    // /
        KEY_RIGHT_SHIFT,         // Rshift
        0x37,                    // (Not used)
        KEY_LEFT_ALT,            // Alternate
        0x20,                    // Space
        KEY_CAPS_LOCK,           // CapsLock
        KEY_F1,                  // F1
        KEY_F2,                  // F2
        KEY_F3,                  // F3
        KEY_F4,                  // F4
        KEY_F5,                  // F5
        KEY_F6,                  // F6
        KEY_F7,                  // F7
        KEY_F8,                  // F8
        KEY_F9,                  // F9
        KEY_F10,                 // F10
        0x45,                    // (Not used)
        0x46,                    // (Not used)
        KEY_HOME,                // Clr/Home
        KEY_UP_ARROW,            // Up Arrow
        0x49,                    // (Not used)
        NUM_Minus,               // Numeric Pad -
        KEY_LEFT_ARROW,          // Left Arrow
        0x4c,                    // (Not used)
        KEY_RIGHT_ARROW,         // Right Arrow
        NUM_Plus,                // Numeric Pad +
        0x4f,                    // (Not used)
        KEY_DOWN_ARROW,          // Down Arrow
        0x51,                    // (Not used)
        KEY_INSERT,              // Insert
        KEY_DELETE,              // Delete
        0x54,                    // (Not used)
        0x55,                    // (Not used)
        0x56,                    // (Not used)
        0x57,                    // (Not used)
        0x58,                    // (Not used)
        0x59,                    // (Not used)
        0x5a,                    // (Not used)
        0x5b,                    // (Not used)
        0x5c,                    // (Not used)
        0x5d,                    // (Not used)
        0x5e,                    // (Not used)
        0x5f,                    // (Not used)
        Key_BackSlashPipe_NonUS, // Keyboard Non-US \ and |
        KEY_F12,                 // Undo (Mapped to F12)
        KEY_F11,                 // Help (Mapped to F11)
        0x28,                    // Numeric Pad ( [Mapped to ( since the Arduino keyboard can't map keycode 182]
        0x29,                    // Numeric Pad ) [Mapped to ) since the Arduino keyboard can't map keycode 183]
        NUM_Slash,               // Numeric Pad /
        NUM_Star,                // Numeric Pad *
        NUM_7,                   // Numeric Pad 7
        NUM_8,                   // Numeric Pad 8
        NUM_9,                   // Numeric Pad 9
        NUM_4,                   // Numeric Pad 4
        NUM_5,                   // Numeric Pad 5
        NUM_6,                   // Numeric Pad 6
        NUM_1,                   // Numeric Pad 1
        NUM_2,                   // Numeric Pad 2
        NUM_3,                   // Numeric Pad 3
        NUM_0,                   // Numeric Pad 0
        NUM_Period,              // Numeric Pad .
        NUM_Enter                // Numeric Pad Enter
};
#endif

#ifdef DE_ATARI_KEYBOARD
// German ST Layout, for DE USB HID Keyboard
uint8_t scanCodes[] =
    {
        0x00,                    // (Nothing)
        KEY_ESC,                 // Esc
        0x31,                    // 1
        0x32,                    // 2
        0x33,                    // 3
        0x34,                    // 4
        0x35,                    // 5
        0x36,                    // 6
        0x37,                    // 7
        0x38,                    // 8
        0x39,                    // 9
        0x30,                    // 0=
        0x2D,                    // ß?
        0x3D,                    // ´´
        KEY_BACKSPACE,           // Backspace
        KEY_TAB,                 // Tab
        0x71,                    // q
        0x77,                    // w
        0x65,                    // e
        0x72,                    // r
        0x74,                    // t
        0x79,                    // y or z
        0x75,                    // u
        0x69,                    // i
        0x6F,                    // o
        0x70,                    // p
        0x3B,                    // Ö
        0x27,                    // Ä
        KEY_RETURN,              // Enter
        KEY_LEFT_CTRL,           // Control
        0x61,                    // a
        0x73,                    // s
        0x64,                    // d
        0x66,                    // f
        0x67,                    // g
        0x68,                    // h
        0x6A,                    // j
        0x6B,                    // k
        0x6C,                    // l
        0x2C,                    // ,;
        0x5B,                    // ü
        Key_GraveAccentTilde,    // ^^
        KEY_LEFT_SHIFT,          // Lshift
        Key_BackSlashPipe,       // <-- Incorrect TODO: Fix!
        0x7A,                    // z
        0x78,                    // x
        0x63,                    // c
        0x76,                    // v
        0x62,                    // b
        0x6E,                    // n
        0x6D,                    // m
        Key_BackSlashPipe_NonUS, // <>
        0x2E,                    // .:
        0x2F,                    // -_
        KEY_RIGHT_SHIFT,         // Rshift
        0x37,                    // (Not used)
        KEY_LEFT_ALT,            // Alternate
        0x20,                    // Space
        KEY_CAPS_LOCK,           // CapsLock
        KEY_F1,                  // F1
        KEY_F2,                  // F2
        KEY_F3,                  // F3
        KEY_F4,                  // F4
        KEY_F5,                  // F5
        KEY_F6,                  // F6
        KEY_F7,                  // F7
        KEY_F8,                  // F8
        KEY_F9,                  // F9
        KEY_F10,                 // F10
        0x45,                    // (Not used)
        0x46,                    // (Not used)
        KEY_HOME,                // Clr/Home
        KEY_UP_ARROW,            // Up Arrow
        0x49,                    // (Not used)
        NUM_Minus,               // Numeric Pad -
        KEY_LEFT_ARROW,          // Left Arrow
        0x4c,                    // (Not used)
        KEY_RIGHT_ARROW,         // Right Arrow
        NUM_Plus,                // Numeric Pad +
        0x4f,                    // (Not used)
        KEY_DOWN_ARROW,          // Down Arrow
        0x51,                    // (Not used)
        KEY_INSERT,              // Insert
        KEY_DELETE,              // Delete
        0x54,                    // (Not used)
        0x55,                    // (Not used)
        0x56,                    // (Not used)
        0x57,                    // (Not used)
        0x58,                    // (Not used)
        0x59,                    // (Not used)
        0x5a,                    // (Not used)
        0x5b,                    // (Not used)
        0x5c,                    // (Not used)
        0x5d,                    // (Not used)
        0x5e,                    // (Not used)
        0x5f,                    // (Not used)
                                 //        Key_BackSlashPipe_NonUS, // Keyboard Non-US \ and |
        0x5D,                    //
        KEY_F12,                 // Undo (Mapped to F12)
        KEY_F11,                 // Help (Mapped to F11)
        0x28,                    // Numeric Pad ( [Mapped to ( since the Arduino keyboard can't map keycode 182]
        0x29,                    // Numeric Pad ) [Mapped to ) since the Arduino keyboard can't map keycode 183]
        NUM_Slash,               // Numeric Pad /
        NUM_Star,                // Numeric Pad *
        NUM_7,                   // Numeric Pad 7
        NUM_8,                   // Numeric Pad 8
        NUM_9,                   // Numeric Pad 9
        NUM_4,                   // Numeric Pad 4
        NUM_5,                   // Numeric Pad 5
        NUM_6,                   // Numeric Pad 6
        NUM_1,                   // Numeric Pad 1
        NUM_2,                   // Numeric Pad 2
        NUM_3,                   // Numeric Pad 3
        NUM_0,                   // Numeric Pad 0
        NUM_Period,              // Numeric Pad .
        NUM_Enter                // Numeric Pad Enter
};
#endif

#ifdef UseFloppyCapsLock
boolean CapsState = true;
/*
Switch on/off the floppy drive led via the ST_FLOPPY_LED Pin.
*/
void TurnOnOfFloppyLED(int HighLow)
{
  pinMode(ST_FLOPPY_LED, OUTPUT);
  digitalWrite(ST_FLOPPY_LED, HighLow); // For output, HIGH is 5v, LOW is 0v
}
#endif

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

void setup(void)
{
  // Initialize keyboard:
  Keyboard.begin();
  Keyboard.releaseAll(); // Not required, but just in case.

  // Open serial port from Atari keyboard
  Serial1.begin(7812); // Serial speed is locked to 7812.5 bits/s

#ifdef DEBUG
  // Open serial port to PC
  Serial.begin(9600);
#endif

  // Reset ST keyboard
  delay(200);
  reset_st_keyboard();
  delay(200);

  // Empty serial buffer before starting
  while (Serial1.available() > 0)
  {
    Serial1.read();
  }

#ifdef JoystickMouse_Support
  //Start the joysticks
  for (int index = 0; index < JOYSTICK_COUNT; index++)
  {
    Joystick[index].begin();

    // Set Range for digital joystick / joypad ie on/off.
    Joystick[index].setXAxisRange(-1, 1);
    Joystick[index].setYAxisRange(-1, 1);

    //Center the X and Y axes on the joystick
    Joystick[index].setXAxis(0);
    Joystick[index].setYAxis(0);
  }
  Mouse.begin; // Starts the mouse.
#endif
}

#ifdef JoystickMouse_Support
// Hndles
void HandleJoy(uint8_t joy, uint8_t char01)
{
  switch (char01)
  {
  case 0x00:                   // Joy0 Centered
    Joystick[joy].setXAxis(0); // Cancel direction left/right
    Joystick[joy].setYAxis(0); // Cancel direction up/down
    break;

  case 0x01:                    // Joy0 Up
    Joystick[joy].setXAxis(0);  // Cancel left/right
    Joystick[joy].setYAxis(-1); // Set direction up
    break;

  case 0x02:                   // Joy0 Down
    Joystick[joy].setXAxis(0); // Cancel left/right
    Joystick[joy].setYAxis(1); // Set direction Down
    break;

  case 0x04:                    // Joy0 Left
    Joystick[joy].setXAxis(-1); // Set direction Left
    Joystick[joy].setYAxis(0);  // Cancel up/down
    break;

  case 0x05:                    // Joy0 Up,Left
    Joystick[joy].setXAxis(-1); // Set direction Left
    Joystick[joy].setYAxis(-1); // Set direction up
    break;

  case 0x06:                    // Joy0 Down,Left
    Joystick[joy].setXAxis(-1); // Set direction Left
    Joystick[joy].setYAxis(1);  // Set direction down
    break;

  case 0x08:                   // Joy0 Right
    Joystick[joy].setXAxis(1); // Set direction Right
    Joystick[joy].setYAxis(0); // Cancel up/down
    break;

  case 0x09:                    // Joy0 Up,Right
    Joystick[joy].setXAxis(1);  // Set direction Right
    Joystick[joy].setYAxis(-1); // Set direction up
    break;

  case 0x0A:                   // Joy0 Down,Right
    Joystick[joy].setXAxis(1); // Set direction Right
    Joystick[joy].setYAxis(1); // Set direction Down
    break;

  default: // If something unexpected happens:
    break;
  }
}

void HandleReleaseBtnAll()
{
  Joystick[0].releaseButton(0);
  Joystick[1].releaseButton(0);
}

void HandlePressBtn0()
{
  Joystick[0].pressButton(0);
  Joystick[1].releaseButton(0);
}

void HandlePressBtn1()
{
  Joystick[0].releaseButton(0);
  Joystick[1].pressButton(0);
}

void HandlePressBtnBoth()
{
  Joystick[0].pressButton(0);
  Joystick[1].pressButton(0);
}
#endif

void Handle_Serial(int16_t SerialChar)
{

#ifdef DEBUG
  if (No_Op_Count > 0)
  {
    Serial.print("No_Op_Loop Count: ");
    Serial.println(No_Op_Count, DEC);
    Serial.print("Stored char: ");
    Serial.println(Prev_Serial_Byte);
    Serial.print("Serial char: ");
    Serial.println(SerialChar);
  }
  else
  {
    Serial.print("Atari serial char: ");
    Serial.println(SerialChar);
  }
#endif

#ifdef JoystickMouse_Support
  if (SerialChar >= JoyBtnReleaseAll)
  {
#ifdef DEBUG
    Serial.print("Storing Prepherial Action: ");
    Serial.println(SerialChar);
#endif
    ActionByte = SerialChar; // Save the current Serial Char

    switch (SerialChar) // Number of Loops required:
    {
    case Joy0:
      No_Op_Count = 1; // Joystick Direction, etc, 2 more serial chars:
      break;

    default:
      No_Op_Count = 2; // Directions, fire etc, 1 more serial char:
      break;
    }
  }
  else
  {
    if (No_Op_Count > 0)
    {
      if (No_Op_Count == 1)
      {
        No_Op_Count--;      // Decrease the loop counter
        switch (ActionByte) // Handle Loop
        {
        case Joy0:
#ifdef DEBUG
          Serial.print("Handle Joy 0: ");
          Serial.println(SerialChar);
#endif
          HandleJoy(0x00, SerialChar); // Press/release Joystick 0 direction(s)
          break;

        case Joy1:
#ifdef DEBUG
          Serial.print("Handle Joy 1: ");
          Serial.println(SerialChar);
#endif
          HandleJoy(0x01, SerialChar); // Press/release Joystick 0 direction(s)
          break;

        case JoyBtnReleaseAll: // Release Both joystick/mouse fire buttons
#ifdef DEBUG
          Serial.println("Handle Release Both Buttons");
#endif
          HandleReleaseBtnAll();
          break;

        case JoyBtnPress0:
#ifdef DEBUG
          Serial.println("Handle Press Joy0 Button / Release Joy1 Button");
#endif
          HandlePressBtn0(); // Press 0 + release 1 Joystick/mouse fire buttons
          break;

        case JoyBtnPress1:
#ifdef DEBUG
          Serial.println("Handle Release Joy0 Button / Press Joy1 Button");
#endif
          HandlePressBtn1(); // Press 1 + release 0 Joystick/mouse fire buttons
          break;

        case JoyBtnPressBoth:
#ifdef DEBUG
          Serial.println("Handle Press Joy0 Button / Press Joy1 Button");
#endif
          HandlePressBtnBoth(); // Press 0 + Press 1 Joystick/mouse fire buttons
          break;
        }
      }
      else
      {                                // Loop = 2
        Prev_Serial_Byte = SerialChar; // Stores current Serial Char
        No_Op_Count--;                 // decrease the loop count
      }
    }
    else
    {
#endif
      //If Previous SerialChar set, handle Joystick stuff. Else: regular scancodes.
      switch (SerialChar)
      {
      case 0: // Do nothing
        break;

      default: // Handle Normal keyboard keys
        if ((SerialChar >= 0x81) & (SerialChar <= 0xF2))
        {

          Keyboard.release(scanCodes[SerialChar - 0x80]);
#ifdef DEBUG
          Serial.print("PC scancode (release): ");
          Serial.println(scanCodes[SerialChar - 0x80]);
#endif
        }
        else
        {
          if (SerialChar <= 0x80)
          {
            Keyboard.press(scanCodes[SerialChar]);

#ifdef UseFloppyCapsLock
            if (SerialChar == ST_CAPS_LOCK)
            {
              TurnOnOfFloppyLED(!CapsState);
              CapsState = !CapsState;
            }
#endif

#ifdef DEBUG
            Serial.print("PC scancode (press): ");
            Serial.println(scanCodes[SerialChar]);
#endif
          }
        }
        break;
      }
#ifdef JoystickMouse_Support
    }
  }
#endif
}

void loop()
{
  if (Serial1.available() > 0)
  {
    Handle_Serial(Serial1.read());
  }
}
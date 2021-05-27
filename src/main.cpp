# include <Arduino.h>
# include "Joystick.h"		// For Joystick control (mheironimus/Joystick)
// # include "Mouse.h"	  		// For Arduino Mouse

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

// #define DEBUG

#define ArduinoKeyboard_Source 

#ifdef ArduinoKeyboard_Source
# include <Keyboard.h>
#else
# include <Keyboard_Real.h>
#endif

// #define US_Layout

/*
For 7 pin Atari keyboards, you can use the floppy LED as a caps lock LED
Unsupported on RJ11 external Keyboards: Mega ST, Mega STE, TT
*/
#define UseFloppyCapsLock // Define, to use the floppy LED as capslock LED
#define maxBut 1		 	    // The number of buttons you are using up to 9, for 2 player mode.
#define JOYSTICK_COUNT 2 	// The number of joysticks connected to the Arduino

//Create Joystick objects
#if JOYSTICK_COUNT == 1
Joystick_ Joystick[JOYSTICK_COUNT] =
{
  Joystick_(0x03, JOYSTICK_TYPE_GAMEPAD, maxBut, 0, true, true, false, false, false, false, false, false, false, false, false)
};
#endif

#if JOYSTICK_COUNT == 2
Joystick_ Joystick[JOYSTICK_COUNT] =
{
  Joystick_(0x03, JOYSTICK_TYPE_GAMEPAD, maxBut, 0, true, true, false, false, false, false, false, false, false, false, false),
  Joystick_(0x04, JOYSTICK_TYPE_GAMEPAD, maxBut, 0, true, true, false, false, false, false, false, false, false, false, false),
};
#endif
/* Use ID 03 for first joystick, 04 for second joystick
   Joystick type: gamepad / digital joystick
   maxBut: Button Count (Both sticks will be the same)
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

// ST keyboard Floppy LED pin
const int ST_FLOPPY_LED = 9; // D9 for Arduino (Pro) Micro

// ST keyboard reset pin
const int ST_KB_RESET = 8; // D8 for Arduino (Pro) Micro

const uint8_t Joy0 = 0xFF;                // Initial Serial char for Joystick 0 Commands
const uint8_t JoyBtnReleaseAll = 0xF8;    // Initial Serial char for Release Both Joystick/Mouse Fire buttons
const uint8_t JoyBtnPress0 = 0xF9;        // Initial Serial char for Press Joystick 0 Fire
const uint8_t JoyBtnPress1 = 0xFA;        // Initial Serial char for Press Joystick 1 Fire
const uint8_t JoyBtnPressBoth = 0xFB;     // Initial Serial char for Press Joystick 0+1 Fire together

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

#ifdef US_Layout
// US ST Layout, for US USB HID Keyboard 
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
  Key_GraveAccentTilde, // ` and ~
  KEY_LEFT_SHIFT, // Lshift
  Key_BackSlashPipe, // \ and |
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
  0x5C, // ISO Key
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
#else
// UK ST Layout, for UK British USB HID Keyboard
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
#endif

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

void setup(void)
{
  // Initialize keyboard:
  Keyboard.begin();
 	Keyboard.releaseAll();  // Not required, but just in case.
 
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
}

void HandleJoy0(uint8_t joy)
{
switch (Serial1.read()){
  case 0x00:  // Joy0 Centered
    Joystick[joy].setXAxis(0);  // Cancel direction left/right
    Joystick[joy].setYAxis(0);  // Cancel direction up/down
    break;

  case 0x01:  // Joy0 Up
    Joystick[joy].setXAxis(0);  // Cancel left/right
    Joystick[joy].setYAxis(1);  // Set direction up
    break;

  case 0x02:  // Joy0 Down
    Joystick[joy].setXAxis(0);  // Cancel left/right
    Joystick[joy].setYAxis(-1); // Set direction Down
    break;

  case 0x04:  // Joy0 Left
    Joystick[joy].setXAxis(-1); // Set direction Left
    Joystick[joy].setYAxis(0);  // Cancel up/down
    break;

  case 0x05:  // Joy0 Up,Left
    Joystick[joy].setXAxis(-1); // Set direction Left
    Joystick[joy].setYAxis(1);  // Set direction up
    break;

  case 0x06:  // Joy0 Down,Left
    Joystick[joy].setXAxis(-1); // Set direction Left
    Joystick[joy].setYAxis(-1); // Set direction down
    break;

  case 0x08:  // Joy0 Right
    Joystick[joy].setXAxis(1);  // Set direction Right
    Joystick[joy].setYAxis(0);  // Cancel up/down
    break;

  case 0x09:  // Joy0 Up,Right
    Joystick[joy].setXAxis(1);  // Set direction Right
    Joystick[joy].setYAxis(1);  // Set direction up
    break;

  case 0x0A:  // Joy0 Down,Right
    Joystick[joy].setXAxis(1);  // Set direction Right
    Joystick[joy].setYAxis(-1); // Set direction Down
    break;  

  default:  // If something unexpected happens:
    break;   
  }
}

void HandleReleaseBtnAll()
{
uint8_t Char01 = 0x00;
uint8_t Char02 = 0x00;

Char01 = Serial1.read();
Char02 = Serial1.read();
Joystick[0].releaseButton(0);
Joystick[1].releaseButton(0);
}

void HandlePressBtn0()
{
uint8_t Char01 = 0x00;
uint8_t Char02 = 0x00;

Char01 = Serial1.read();
Char02 = Serial1.read();
Joystick[0].pressButton(0);
Joystick[1].releaseButton(0);
}

void HandlePressBtn1()
{
uint8_t Char01 = 0x00;
uint8_t Char02 = 0x00;

Char01 = Serial1.read();
Char02 = Serial1.read();
Joystick[0].releaseButton(0);
Joystick[1].pressButton(0);
}

void HandlePressBtnBoth()
{
uint8_t Char01 = 0x00;
uint8_t Char02 = 0x00;

Char01 = Serial1.read();
Char02 = Serial1.read();
Joystick[0].pressButton(0);
Joystick[1].pressButton(0);
}


void Handle_Serial(uint8_t SerialChar)
{

#ifdef DEBUG 
    Serial.print("Atari scancode: ");
    Serial.println(SerialChar);
#endif

switch (SerialChar){
  case Joy0:
    HandleJoy0(0x00); // Press/release Joystick 0 direction(s)
    break;

  case JoyBtnReleaseAll:  // Release Both joystick/mouse fire buttons
    HandleReleaseBtnAll();
    break;

  case JoyBtnPress0:
    HandlePressBtn0(); // Press 0 + release 1 Joystick/mouse fire buttons
    break;

  case JoyBtnPress1:
    HandlePressBtn1(); // Press 1 + release 0 Joystick/mouse fire buttons
    break;

  case JoyBtnPressBoth:
    HandlePressBtnBoth(); // Press 0 + Press 1 Joystick/mouse fire buttons
    break; 

  case 0:   // Do nothing
    break;

  default:  // Handle Normal keyboard keys
    if (SerialChar >= 0x81){
      Keyboard.release(scanCodes[SerialChar - 0x80]);
      #ifdef DEBUG
        Serial.print("PC scancode (release): ");
        Serial.println(scanCodes[SerialChar - 0x80]);
      #endif
    }
    else {
      Keyboard.press(scanCodes[SerialChar]);
      #ifdef DEBUG
        Serial.print("PC scancode (press): ");
        Serial.println(scanCodes[SerialChar]);
      #endif  
    }
    break;
  }
}

void loop()
{
  // Process serial data
  if (Serial1.available() > 0) {
    Handle_Serial(Serial1.read());
  }
}
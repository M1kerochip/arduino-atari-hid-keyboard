/*
  Keyboard.h

  Copyright (c) 2015, Arduino LLC
  Original code (pre-library): Copyright (c) 2011, Peter Barrett

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef KEYBOARD_h
#define KEYBOARD_h

#include "HID.h"

#if !defined(_USING_HID)

#warning "Using legacy HID core (non pluggable)"

#else

//================================================================================
//================================================================================
//  Keyboard

#define KEY_NOEVENT             0x00  // 00
#define KEY_ERROR_ROLLOVER      0x01  // 01
#define KEY_POSTERROR           0x02  // 02
#define KEY_ERROR_UNDEFINED     0x03  // 03



#define KEY_RETURN              0x28  // 40 
#define KEY_ESC                 0x29  // 41
#define KEY_BACKSPACE           0x2A  // 42 (Delete in the USB Keylist)
#define KEY_TAB                 0x2B  // 43


#define Key_BackSlashPipe       0x31  // 49
#define Key_TildeHash_NonUS     0x32  // 50
#define Key_SemiColonColon      0x33  // 51
#define Key_QuoteDoubleQuote    0x34  // 52
#define Key_GraveAccentTilde    0x35  // 53



#define KEY_CAPS_LOCK           0x39  // 57
#define KEY_F1                  0x3A  // 58
#define KEY_F2                  0x3B  // 59  
#define KEY_F3                  0x3C  // 60
#define KEY_F4                  0x3D  // 61
#define KEY_F5                  0x3E  // 62
#define KEY_F6                  0x3F  // 63
#define KEY_F7                  0x40  // 64
#define KEY_F8                  0x41  // 65
#define KEY_F9                  0x42  // 66
#define KEY_F10                 0x43  // 67
#define KEY_F11                 0x44  // 68
#define KEY_F12                 0x45  // 69
#define Key_PrintScreen         0x46  // 70
#define Key_ScrollLock          0x47  // 71
#define Key_PauseBreak          0x48  // 72
#define KEY_INSERT              0x49  // 73
#define KEY_HOME                0x4A  // 74
#define KEY_PAGE_UP             0x4B  // 75
#define KEY_DELETE              0x4C  // 76 (Delete Forward in the USB Keylist)
#define KEY_END                 0x4D  // 77
#define KEY_PAGE_DOWN           0x4E  // 78
#define KEY_RIGHT_ARROW         0x4F  // 79
#define KEY_LEFT_ARROW          0x50  // 80
#define KEY_DOWN_ARROW          0x51  // 81
#define KEY_UP_ARROW            0x52  // 82
#define NUM_NumlockClear        0x53  // 83
#define NUM_Slash               0x54  // 84
#define NUM_Star                0x55  // 85
#define NUM_Minus               0x56  // 86
#define NUM_Plus                0x57  // 87
#define NUM_Enter               0x58  // 88
#define NUM_1                   0x59  // 89
#define NUM_2                   0x5A  // 90
#define NUM_3                   0x5B  // 91
#define NUM_4                   0x5C  // 92
#define NUM_5                   0x5D  // 93
#define NUM_6                   0x5E  // 94
#define NUM_7                   0x5F  // 95
#define NUM_8                   0x60  // 96
#define NUM_9                   0x61  // 97
#define NUM_0                   0x62  // 98
#define NUM_Period              0x63  // 99
#define Key_BackSlashPipe_NonUS 0x64  //100
#define KEY_Application         0x65  //101 (Note_10)
#define KEY_Power               0x66  //102
#define NUM_Equals              0x67  //103
#define KEY_F13                 0x68  //104
#define KEY_F14                 0x69  //105
#define KEY_F15                 0x6A  //106
#define KEY_F16                 0x6B  //107
#define KEY_F17                 0x6C  //108
#define KEY_F18                 0x6D  //109
#define KEY_F19                 0x6E  //110
#define KEY_F20                 0x6F  //111
#define KEY_F21                 0x70  //112
#define KEY_F22                 0x71  //113
#define KEY_F23                 0x72  //114
#define KEY_F24                 0x73  //115
#define KEY_Execute             0x74  //116
#define KEY_Help                0x75  //117

#define NUM_OpenBracket         0xB6  //182
#define NUM_CloseBracket        0xB7  //183

#define KEY_LEFT_CTRL           0xE0  //224
#define KEY_LEFT_SHIFT          0xE1  //225
#define KEY_LEFT_ALT            0xE2  //226
#define KEY_LEFT_GUI            0xE3  //227
#define KEY_RIGHT_CTRL          0xE4  //228
#define KEY_RIGHT_SHIFT         0xE5  //229
#define KEY_RIGHT_ALT           0xE6  //230
#define KEY_RIGHT_GUI           0xE7  //231

// E8-FFFF undefined.

//  Low level key report: up to 6 keys and shift, ctrl etc at once
typedef struct
{
  uint8_t modifiers;
  uint8_t reserved;
  uint8_t keys[6];
} KeyReport;

class Keyboard_Real_ : public Print
{
private:
  KeyReport _keyReport;
  void sendReport(KeyReport* keys);
public:
  Keyboard_Real_(void);
  void begin(void);
  void end(void);
  size_t write(uint8_t k);
  size_t write(const uint8_t *buffer, size_t size);
  size_t press(uint8_t k);
  size_t release(uint8_t k);
  void releaseAll(void);
};
extern Keyboard_Real_ Keyboard;

#endif
#endif


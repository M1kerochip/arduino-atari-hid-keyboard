Interface Atari ST Keyboard to USB HID Keyboard
===============================================

This code runs on an ATmega32U4 based Arduino to provide an interface between an Atari ST keyboard (8 Pin, 18 Pin and RJ11) and a USB HID keyboard device.
Atari 520 STFM (UK), Atari 1040 STE (UK) and Atari TT keyboards tested.

The purpose of this is to allow an ST keyboard to be used with any machine that accecpts USB HID Keyboards.

Developed for use with an ATmega32U4 based Arduino (eg Micro, Leonardo, Pro Micro etc) as it is able to act directly as a USB keyboard controller.

The Atari keyboard connector is wired to the Arduino as follows:

| Atari 8-pin Keyboard Connector  | Arduino |
| ------------------------------- |:-------:|
| left foo                        | right foo     |
| 1 0V                            | GND |
| Pin 2: (Unused) | |

| Atari 8-pin Keyboard Connector  | Arduino |
| ------------------------------- |:-------:|
| Pin 1: 0V | GND |
| Pin 2: Blocked (Unused) | - |
| Pin 3: (Unused) | - |
| Pin 4: 5V | 5V |
| Pin 5: TX | RX (PD2) |
| Pin 6: RX | TX (PD3) |
| Pin 7: Reset | Digital Pin 10 (PB6) |
| Pin 8: Floppy Drive Light |  Digital Pin 14 (PF7) |
>

To use an 18 pin original ST connector, just connect pins 1-8, using the blocked/missing pin as pin 2.
The other pins are unused for keyboard purposes. (They contain the 9pin mouse/keyboard inputs from the main PCB)


To use an RJ11 based keyboard (Mega ST, Mega STE, TT)
| Atari RJ11 Keyboard Connector  | Arduino |
| ------------------------------ |:-------:|
| Pin 1: 5V | 5V |
| Pin 2: 5V | 5V |
| Pin 3: RX | TX (PD3) |
| Pin 4: TX | RX (PD2) |
| Pin 5: 0V | GND |
| Pin 6: 0V | GND |




See http://www.kevinpeat.com/atari_pi_reworked.html for my hybrid Atari ST Raspberry Pi project.

Copyright Kevin Peat 2017
kevin 'at' kevinpeat.com

### 2021-05-18 (MF)

* Switched over development from the Arduino IDE to PlatformIO using Visual Studio Code.
* Program/sketch is now main.cpp instead of USB_HID_Keyboard.ino
* The Arduino keyboard.h treats values below 128 as printable and uses an ascii lookup table. It add 136 to the value to overcome this. Therefore keycodes above 120 cannot be used.
* The ( and ) numperpad keys are keycodes 182 and 183.
* Fixed the incorrect keycodes for all the numberpad keys except the two above.
* Changed Numberpad Enter key to scancode 88: Keypad Enter
* Changed the \ | key to scancode 49: Keyboard \ and |
* Changed the #~ key to scancode 50: Keyboard Non-US # and ~
* Changed Help to F11 and Undo to F12.
* Keycode list: from https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf

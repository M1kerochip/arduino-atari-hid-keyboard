Interface Atari ST Keyboard to USB HID Keyboard
===============================================

This code runs on an Arduino Leonardo to provide an interface between 
an Atari ST keyboard (520 STFM keyboard tested, others *may* work) and 
a USB HID keyboard device.

The purpose of this is to allow a small form factor PC (Raspberry Pi in
my case) to be installed in an Atari ST case and to use the original 
ST keyboard.

Developed for use with an Arduino Leonardo as it is able to act directly 
as a USB keyboard controller so doesn't require the Arduino firmware to 
be modified as some of the other Arduinos (eg. Uno) would do.

The Atari keyboard connector is wired to the Arduino as follows:

> Atari 8-pin Keyboard Connector     Arduino
> ------------------------------     -------
> 1 0V                               GND <br>
> 2 (Blanked off) <br>
> 3 Not used <br>
> 4 5V                               5V <br>
> 5 RX                               RX1 (0) <br>
> 6 TX                               TX1 (1) <br>
> 7 Reset                            4 (can be changed in code) <br>
> 8 Not used <br>
>

See http://www.kevinpeat.com/atari_pi_reworked.html for my hybrid Atari
ST Raspberry Pi project.


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

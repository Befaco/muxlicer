# Muxlicer v1.5

This code is made for Befaco's Muxlicer module. A sequential signal processor divided in three main blocks: a Digital Step Controller, a Gate Generator and an Analog Switch (a.k.a. Mux/DeMux)
Find further information [here](https://www.befaco.org/muxlicer-2/)
This code will work in any hardware revision from PCB v1.3. Note this code will have issues in PCBs 1.2 and below.

## Changes in V1.5:

Improvement: Addres knob "Run" range is now bigger so it maches better the pannel graphics and is less voltage sensitive

Improvement: Defaul clock mode changed to: Clock out stop when Muxlicer stop 

Bug fixes: Clock out is totally silence when it should be, Reduced Ripple at CV out, Phantom trigs on MEX eliminated
(Bug fix by Andy B. Many thanks Andy!)


## Dependencies:

[TimerOne](http://playground.arduino.cc/Code/Timer1) Also Available from Libraries manager.

[ClickEncoder](https://github.com/0xPIT/encoder/)

## Uploading the firmware

1. Get an ICSP Programmer 
We normally use an USBasp programmer like this one https://www.ebay.com/itm/USBASP-USB-ISP-Programmer-for-Atmel-AVR-ATMega328-ATMega32U4-Arduino-/322662323277

2. Download Arduino IDE
Go to the official Arduino website https://www.arduino.cc/en/Main/Software and download the latest version of Arduino IDE for your operating system and install it.

3. Download the firmware:   Go to (https://github.com/Befaco/muxlicer/releases/tag/v1.5) and open the muxlicer.ino file on Arduino IDE

4. Download the libraries: 
 
[TimerOne](http://playground.arduino.cc/Code/Timer1) .  Also Available from Libraries manager.

[ClickEncoder](https://github.com/0xPIT/encoder/)

and install them following the instructions from this link https://www.arduino.cc/en/Guide/Libraries

5. Connecting the module:
Connect the programmer to the ICSP conector in the back of the module. Pay special attention to the pinout when you plug the module. You can check this link for more information about ICSP https://www.arduino.cc/en/Tutorial/ArduinoISP

6. Upgrading:
Press "Open" and search the file "muxlicer.ino" located on the "MUXLICER" folder. Go to "Sketch" and hit "Upload using programmer".

If everything goes well you should see a "Done uploading" message in a few seconds. If something happens during the upgrade, check your Arduino settings following the instructions of this link https://www.arduino.cc/en/Guide/Troubleshooting

## Credits.

Coded By Eloi Flowers from Winter modular and Geremy Bernstain
Several improvements by Andy Butler.






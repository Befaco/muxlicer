# Muxlicer v1.4

This code is made for Befaco's Muxlicer module. A sequential signal processor divided in three main blocks: a Digital Step Controller, a Gate Generator and an Analog Switch (a.k.a. Mux/DeMux)
Find further information [here](https://www.befaco.org/muxlicer-2/)
This code will work in any hardware revision from PCB v1.3. Note this code will have issues in PCBs 1.2 and below.

## Changes:

Changed input clock behaviour. This had an incompatibility with hardware v1.1. 


## Dependencies:

[TimerOne](http://playground.arduino.cc/Code/Timer1) Also Available from Libraries manager.
[ClickEncoder](https://github.com/0xPIT/encoder/)

## Uploading the firmware

1. Get an ICSP Programmer 
We normally use an USBasp programmer like this one https://www.ebay.com/itm/USBASP-USB-ISP-Programmer-for-Atmel-AVR-ATMega328-ATMega32U4-Arduino-/322662323277

2. Download Arduino IDE
Go to the official Arduino website https://www.arduino.cc/en/Main/Software and download the latest version of Arduino IDE for your operating system and install it.

3. Download the firmware 
clone or download thisrepository. Go to https://github.com/Befaco/muxlicer and press "clone or download" to download the repository to your computer. Extract the zip file and copy "firmware" folder to your sketchbook folder. 

4. Install the libraries
Download the libraries  (Rotary encoder) and install them following the instructions from this link https://www.arduino.cc/en/Guide/Libraries

5. Connecting the module
Connect the programmer to the ICSP conector in the back of the module. Pay special attention to the pinout when you plug the module. You can check this link for more information about ICSP https://www.arduino.cc/en/Tutorial/ArduinoISP

6. Upgrading
Press "Open" and search the file "muxlicer.ino" located on the "BURST" folder. Go to "Sketch" and hit "Upload using programmer".

If everything goes well you should see a "Done uploading" message in a few seconds. If something happens during the upgrade, check your Arduino settings following the instructions of this link https://www.arduino.cc/en/Guide/Troubleshooting

## Credits.

Coded By Eloi Flowers from Winter modular.






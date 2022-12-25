# 1. Switches, sensors
## 1.1 magnetic reed switches
 - D7 (GPIO13) door top reed switch (to GND, internal pullup)
 - D6 (GPIO12) door bottom reed switch (to GND, internal pullup)

## 1.2 temperature sensors
D3 (GPIO0) OneWire -> DS18B20 - OneWire requires a single 4.7K pullup resistor, connected between the pin and your power supply. When using very long wires, or with counterfeit DS18B20 chips and 3.3V power, a resistor in the 1K to 2.7K range may be required (i use 4k7 resistor).

## 1.3 action switch
D5 (GPIO14) action switch (to GND, internal pullup)
   - short - open, stop, close door
   - pressed for min 3 sec - turn acess poit (AP) on
   - pressed for 10 sec - reset to factory settings
   
## 1.4 LDR sensor


# 2. Motor shield
## 2.0 Installation, connection

Connect first shield, turn on, addres will be changed from 0x30 to 0x2D.

Turn off, add second board and turn on. Addres stay 0x30.

 - First shield 
   - channel A 1-2 is used for motor.
   - channel B is used for leds, hen light has higher priority als coop light
     - 1 -> coop light -> GND
     - 2 -> hen light -> GND
 - Second shield
   - channel A
     - 1 si used for fan
       - A1 -> fan -> GND
     - 2 is used for heater
       - A2 -> heater -> GND
   - channel B
     - 1 is used for water heater
       - 1 -> wtr heater -> GND 
     - 2 is for feeder (beta testing)
       - 2 -> feeder -> GND
      
![image](https://github.com/RastoH/eXtended-Coop-Controller/blob/main/hardware/mozok.jpg)
![image](https://github.com/RastoH/eXtended-Coop-Controller/blob/main/hardware/motor.jpg)

## 2.1 Wemos motor shield V1 (TB6612FNG)
### discontinued
 - Power supply voltage: VM=2.5VDC to 15.0V max
 - Output current: Iout=1.2A(average) / 3.2A (peak)
 
![1](https://github.com/RastoH/eXtended-Coop-Controller/blob/main/hardware/Motorshields.png)
![2](https://github.com/RastoH/eXtended-Coop-Controller/blob/main/hardware/motor_v1.0.0_set.jpg?raw=true)
### Video of the driver's work:
 - https://youtu.be/gah3zCJPPUc  
 - https://youtu.be/8P2wTgeavdo  

# Note!!! :
"WEMOS MOTOR V1" requires a firmware update. Factory installed firmware has reliability issues like I2C bus stuck. The firmware requires regular USB2TTL.
[More information!](https://hackaday.io/project/18439-motor-shield-reprogramming)  

WEMOS MOTOR Pin|GPIO|Component|USB2TTL
:-:|:-:|:-:|:-:
D1|5|RX|TX
D2|4|TX|RX
3V3|-|-|3V3
GND|-|-|GND

**!!! And be sure to short-circuit the RTS and 3V pins together on "Wemos motor shield V1".**

1. Download here for firmware "motor_shield.bin":
 - https://cdn.hackaday.io/files/18439788894176/motor_shield.bin   
 or   
 - https://github.com/TrDA-hab/Projects/blob/master/DC_motor/motor_shield.bin   

2. Download "STM32Flash" from here:
 - https://sourceforge.net/projects/stm32flash/files/

3. Run these commands in the Windows console (use your COM port number!):  
`stm32flash.exe COM4`  
`stm32flash.exe -k COM4`  
`stm32flash.exe -f -v -w motor_shield.bin COM4`  

![1](https://raw.githubusercontent.com/TrDA-hab/Projects/master/DC_motor/STM32flash-1.jpg)  
![2](https://raw.githubusercontent.com/TrDA-hab/Projects/master/DC_motor/STM32flash-2.jpg)  
![3](https://raw.githubusercontent.com/TrDA-hab/Projects/master/DC_motor/STM32flash-3.jpg)  
  
**!!! After finishing the firmware, disconnect all wires (including the 3V and RTS pin), connect the shield to the ESP device and it should work!**

## 2.2 LOLIN motor shield V2 (TB6612FNG)
### firmware revision 1
 - Power supply voltage: VM=15V max (2.5VDC to 15.0V max)
 - Output current: Iout=1.2A(average) / 3.2A (peak)

![1](https://www.wemos.cc/en/latest/_static/d1_shields/motor_v2.0.0_1_16x16.jpg)
https://www.wemos.cc/en/latest/d1_mini_shield/motor.html

## 2.3 LOLIN motor shield V2 (HR8833)
### firmware revision 3
 - Power supply voltage: VM=3-10V
 - Output current: Iout=1.5A(average) / 2.5A (peak)

![1](https://www.wemos.cc/en/latest/_static/d1_shields/hr8833_v1.0.0_1_16x16.jpg)
https://www.wemos.cc/en/latest/d1_mini_shield/hr8833_motor.html   
https://aliexpress.com/item/1005003697301523.html   

## 2.4 LOLIN motor shield V2 (AT8870)
### firmware revision 4
 - Power supply voltage: VM=6.5-38V
 - Output current: Iout=2A(average) / 3.6A (peak)

![1](https://www.wemos.cc/en/latest/_static/d1_shields/at8870_v1.0.0_1_16x16.jpg)   
https://www.wemos.cc/en/latest/d1_mini_shield/at8870_motor.html   
https://aliexpress.com/item/1005003800396010.html   


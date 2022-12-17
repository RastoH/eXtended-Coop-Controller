
# 1. Wemos motor shield V1 (TB6612FNG)

![1](https://github.com/RastoH/eXtended-Coop-Controller/blob/main/hardware/Motorshields.png)
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

# 2. LOLIN motor shield V2 (TB6612FNG)
### firmware version 2

![1](https://www.wemos.cc/en/latest/_static/d1_shields/motor_v2.0.0_1_16x16.jpg)
https://www.wemos.cc/en/latest/d1_mini_shield/motor.html

# 3. LOLIN motor shield V2 (HR8833)
### firmware version 3

![1](https://www.wemos.cc/en/latest/_static/d1_shields/hr8833_v1.0.0_1_16x16.jpg)
https://www.wemos.cc/en/latest/d1_mini_shield/hr8833_motor.html   
https://aliexpress.com/item/1005003697301523.html   

# 4. LOLIN motor shield V2 (AT8870)
### firmware version 4

![1](https://www.wemos.cc/en/latest/_static/d1_shields/at8870_v1.0.0_1_16x16.jpg)   
https://www.wemos.cc/en/latest/d1_mini_shield/at8870_motor.html   
https://aliexpress.com/item/1005003800396010.html   


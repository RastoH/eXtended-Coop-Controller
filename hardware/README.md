Wemos motor shield V1
    TB6612FNG 
    
Note!!! :

"WEMOS MOTOR V1" requires a firmware update. Factory installed firmware has reliability issues like I2C bus stuck. The firmware requires regular USB2TTL.
More information!
WEMOS MOTOR Pin 	GPIO 	Component 	USB2TTL
D1 	5 	RX 	TX
D2 	4 	TX 	RX
3V3 	- 	- 	3V3
GND 	- 	- 	GND

!!! And be sure to short-circuit the RTS and 3V pins together on "Wemos motor shield V1".

    Download here for firmware "motor_shield.bin":

    https://cdn.hackaday.io/files/18439788894176/motor_shield.bin
    or
    https://github.com/TrDA-hab/Projects/blob/master/DC_motor/motor_shield.bin

    Download "STM32Flash" from here:

    https://sourceforge.net/projects/stm32flash/files/

    Run these commands in the Windows console (use your COM port number!):
    stm32flash.exe COM4
    stm32flash.exe -k COM4
    stm32flash.exe -f -v -w motor_shield.bin COM4


!!! After finishing the firmware, disconnect all wires (including the 3V and RTS pin), connect the shield to the ESP device and it should work!



LOLIN motor shield V2
    firmware 2: TB6612FNG
    firmware 3: HR8833
    firmware 4: AT8870
    
    

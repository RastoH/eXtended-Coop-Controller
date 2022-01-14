# eXtended Coop Controller
Control chicken coop with ESP8266 - automatic chicken coop door, Hen Light, Coop Light, Heater, Fan, water .... 

## Features
- Door modes
	- Manual
	- Light - Dusk-to-Dawn (a.k.a. Dawn-to-Dusk) (LUX) (LDR on A0 or bh1750 on I2C bus)
	- Time (Schedule)
	- Automatic - Calculated Sunset and Sunrise times for latitude/longitude
- Freeze Protect
	- Door stays closed on those winter days when it is dangerously cold - ensuring the safety of your poultry investment.
	- freeze protect temperature
- Quarantine
	- quarantine end date
- Hen light
- Coop light
- Coop fan
- Coop heat

Door operation settings are programmable using Web App or REST-like API over the Internet by connecting to the xCctrl WiFi Module.
- Override your coop door Select Open, Close or Stop from the interface.

## Web App

### Real-time status information (Web App)
 - Date and Time
 - Outside, Indoor, Water Temperature
 - Ambient Light Value (0% - total darknes to 100% - sunlight shining on sensor)
 - Door Control Settings
 - Freeze Protect Settings
 - Door Status
 - System Status
 
### System configuration (WEB) 
 - Security
 - NTP, RTC, DST, DST region - EU or US
 - WiFi network
 - [ThingSpeak](https://thingspeak.com/channels/636967)
 - Dynamic DNS
 
### Proposal
There are many things to think about when keeping chickens. You have to get up with the sunrise to let them out and put them in at sunset. 

You have to give them water and feed them every day. I am trying to lessen the workload with automation.

The sunrise and sunset times in automatic mode are calculated every day on 0:00 from actual system date, these times are then used to open and close the door automatically. (required DS3231 or NTP)

- Control the door and check how much water is left through a basic website hosted on the chip itself (HTML, and RESTful)
- Prevent over-closing or over-opening
- Can recover from power cut outs
- Can recover from wifi chip reboots or network disruption
- Flash LED every few seconds to indicate that the door is closed
- Fox proof: Can’t be pushed open while closed

The inside temperature,outside temperature and ambient light are measured every minute, these values are then uploaded to ThingSpeak.  

A heat lamp (heater) is turned on if the coop gets too cold and a fan is turned on if it gets too hot.
 - kurenie sa zapne pri nizkej vnutornej teplote a zatvorenych dvierkach.
 - vetranie za zapne, ak bude vonkajsia teplota nizsia ako vnutorna a dvierka su otvorene.

* There is an automatic watering system that always keeps the chickens supply topped up.

All of the communication with the user is done through a website run on the ESP8266.
 
The user can open and close the door, turn a light on or off, , and feed the chickens from here.

Environment measurements is achieved using ThingSpeak server.

If you open the door manually with the pushbutton in the evening when the system thinks it should be closed, the door will remain open for max 1 minuts before it will automatically close


## Components
- ESP8266 board [LOLIN D1 mini](https://www.wemos.cc/en/latest/d1/d1_mini.html)
- I2C motor driver shield
	- WEMOS v1
	- [LOLIN v2 (tested)](https://www.wemos.cc/en/latest/d1_mini_shield/motor.html), [AT8870 Motor Shield](https://www.wemos.cc/en/latest/d1_mini_shield/at8870_motor.html), [HR8833 Motor Shield](https://www.wemos.cc/en/latest/d1_mini_shield/hr8833_motor.html)
	- drv8830 (deprecated)
- DS18B20 - Inside, outside, water temp
- LDR or bh1750 - ambient light intensity
- DS3231 RTC module
- Led strip 12v for Coop and Hen light
- 12V Power supply (12V 10Ah batery + 40W solar)
- [DC Power Shield](https://www.wemos.cc/en/latest/d1_mini_shield/dc_power.html)
- Box (optional) 
- low speed gear motor with long output shaft 25mmx4mm 25GA370, 12V, 35 RPM
- magnetic reed switches

## Pin
- A0    LDR (to 3.3V)
- D1	SCL I2C
- D2	SDA I2C
- D3	OneWire -> DS18B20 - OneWire requires a single 4.7K pullup resistor, connected between the pin and your power supply. When using very long wires, or with counterfeit DS18B20 chips and 3.3V power, a resistor in the 1K to 2.7K range may be required. 
- D4    builtin led (reverse polaryty to 3.3V)
- D5  	action switch (to GND)
	- short - open, stop, close door
	- pressed for min 3 sec - turn acess poit (AP) on
	- pressed for 10 sec - reset to factory settings 
- D6	door bottom reed switch (to GND)
- D7	door top reed switch (to GND)

## Demo
[![DEMO](http://img.youtube.com/vi/uT8rBUs3wm0/0.jpg)](https://youtu.be/uT8rBUs3wm0) 

[This licence expressly prohibits commercial use](https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode)

# eXtended Coop Controller
Control chicken coop with ESP8266 - Door, Hen Light, Coop Light, Heater, Fan, water .... 

## Features
- Door modes
	- Manual
	- Light - Dusk-to-Dawn (a.k.a. Dawn-to-Dusk) (LUX) (LDR on A0 or bh1750 on I2C bus)
	- Time (Schedule)
	- Automatic - Calculated Sunset and Sunrise times
- Freeze Protect
	- Door stays closed on those winter days when it is dangerously cold - ensuring the safety of your poultry investment.
	- freeze protect temperature
- Quarantine
	- quarantine end date
- Coop fan
- Coop heat
- Hen light
- Coop light

Door operation settings are programmable using Web App or REST-like API over the Internet by connecting to the xCctrl WiFi Module.
- Override your coop door Select Open, Close or Stop from the user menu.

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


The sunrise and sunset times in automatic mode are calculated every day on 0:00 from actual system date, these times are then used to open and close the door automatically.

The inside temperature,outside temperature and ambient light are measured every minute, these values are then uploaded to ThingSpeak.  

A heat lamp (heater) is turned on if the coop gets too cold and a fan is turned on if it gets too hot.
 - kurenie sa zapne pri nizkej vnutornej teplote a zatvorenych dvierkach.
 - vetranie za zapne, ak bude vonkajsia teplota nizsia ako vnutorna a dvierka su otvorene.

* There is an automatic watering system that always keeps the chickens supply topped up.

All of the communication with the user is done through a website run on the ESP8266.
 
The user can open and close the door, turn a light on or off, /*watch the stream from a camera in the coop, and feed the chickens */from here.
Finally, there is a separate web app run using nodeJS where the user can graph a specific range of environment measurements. This is achieved using ThingSpeak setver.



If you open the door manually with the pushbutton in the evening when the system thinks it should be closed, the door will remain open for 2 minutes before it will automatically close


## Components
- ESP8266 board [LOLIN D1 mini](https://www.wemos.cc/en/latest/d1/d1_mini.html)
- I2C motor driver shield
	- WEMOS v1
	- [LOLIN v2 (tested)](https://www.wemos.cc/en/latest/d1_mini_shield/motor.html)
	- drv8830
- DS18B20 - Inside, outside, water temp
- LDR or bh1750 - ambient light intensity
- DS3231 RTC module
- Led strip 12v for Coop and Hen light
- 12V Power supply (12V 10Ah batery + 40W solar)
- [DC Power Shield](https://www.wemos.cc/en/latest/d1_mini_shield/dc_power.html)
- Box (optional) 
- low speed gear motor with long output shaft 25mmx4mm 25GA370, 12V, 35 RPM
- magnetic red switches

## Pin
- A0    LDR
- D1	SCL I2C
- D2	SDA I2C
- D3	OneWire bus -> for DS18B20
- D4    builtin led
- D5  	action switch
- D6	door bottom switch
- D7	door top switch

## Demo
[![DEMO](http://img.youtube.com/vi/uT8rBUs3wm0/0.jpg)](https://youtu.be/uT8rBUs3wm0) 

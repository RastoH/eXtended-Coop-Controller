# 1. Main page
![1](https://github.com/RastoH/eXtended-Coop-Controller/blob/main/webUI/main1.JPG)
![2](https://github.com/RastoH/eXtended-Coop-Controller/blob/main/webUI/main2.JPG)
![3](https://github.com/RastoH/eXtended-Coop-Controller/blob/main/webUI/main_without.JPG)

# 2. Settings page
## 2.1 Door settings

#### 2.1.1 Door modes 

#### 2.1.1.1 Manual

#### 2.1.1.2 Light 

#### 2.1.1.3 Time

#### 2.1.1.4 Automatic mode
![image](https://github.com/RastoH/eXtended-Coop-Controller/blob/main/webUI/door_mode_automatic.jpg)
The location of interest, in decimal degrees.    

***Latitude*** is a coordinate that specifies the north–south position of a point on the surface of the Earth or another celestial body.
> Latitude ranges	from -90 (south pole) to 90 (north pole).

***Longitude*** is a geographic coordinate that specifies the east–west position of a point on the surface of the Earth, or another celestial body.
 > Longitude ranges	from -180 (west of Greenwich) to 180 (east of Greenwich).
  
### 2.1.2 Freeze protect

### 2.1.3 Quarantine

## 2.2 Accessory settings
#### 2.2.1.1 Hen light 
![image](https://github.com/RastoH/eXtended-Coop-Controller/blob/main/webUI/hen_light.png)
#### 2.2.1.2 Coop light 
#### 2.2.1.3 Vent Fan (Cooling)
#### 2.2.1.4 Heater
### 2.2.2 Water heater 
![image](https://github.com/RastoH/eXtended-Coop-Controller/blob/main/webUI/water_heater.jpg)
## 2.3 System config

### 2.3.1 Door name
### 2.3.2 Security
![image](https://github.com/RastoH/eXtended-Coop-Controller/blob/main/webUI/security.png)
### 2.3.3 Date-Time
![image](https://github.com/RastoH/eXtended-Coop-Controller/blob/main/webUI/date-time.jpg)
### 2.3.4 WiFi network
![1](https://github.com/RastoH/eXtended-Coop-Controller/blob/main/webUI/wifi_sta_client.png)
![2](https://github.com/RastoH/eXtended-Coop-Controller/blob/main/webUI/wifi_Client_State.png)
![3](https://github.com/RastoH/eXtended-Coop-Controller/blob/main/webUI/wifi_Soft-AP_State.png)
![4](https://github.com/RastoH/eXtended-Coop-Controller/blob/main/webUI/wifi_WiFi_State.png)
### 2.3.5 ThingSpeak
![image](https://github.com/RastoH/eXtended-Coop-Controller/blob/main/webUI/thingspeak.png)

## 2.4 System info
### 2.4.1 Thermometers
#### assigning the thermometer to the measured location
![image](https://github.com/RastoH/eXtended-Coop-Controller/blob/main/webUI/Thermomoters.png)

REST API GET -> http://192.168.0.43/temp?sensor1=2&sensor2=3&sensor3=1

Mainly needed when gradually adding sensors.
Dallas temperature sensors DS18B20 are always detected at start-up according to the serial number in ascending order.

# 4. Update (web OTA)
**/update**

![image](https://github.com/RastoH/eXtended-Coop-Controller/blob/main/webUI/updater.png)
# 5. REST API

/state
GET
  state info

/settings
GET
  door settings

/config
GET
  controller config

/wifi
/rescan
GET
  wifi info

/thngspk
GET
  ThingSpeak config, info

/info
GET
  xctrl info

/update
  OTA, update firmware, filesystem page

/reset
  reset to factory dwfault

/restart
  restart system

/log
/logold
GET
  loger file for debug

/crash
GET
  crash info

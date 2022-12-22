# 1. Main page

# 2. Settings page
## 2.1 Door settings

#### 2.1.1 Door modes 

#### 2.1.1.1 Manual

#### 2.1.1.2 Light 

#### 2.1.1.3 Time

#### 2.1.1.4 Automatic mode
The location of interest, in decimal degrees.    

***Latitude*** is a coordinate that specifies the north–south position of a point on the surface of the Earth or another celestial body.
> Latitude ranges	from -90 (south pole) to 90 (north pole).

***Longitude*** is a geographic coordinate that specifies the east–west position of a point on the surface of the Earth, or another celestial body.
 > Longitude ranges	from -180 (west of Greenwich) to 180 (east of Greenwich).
  
### 2.1.2 Freeze protect

### 2.1.3 Quarantine

## 2.2 Accessory settings
### 2.2.1 Hen light 
### 2.2.2 Coop light 
### 2.2.3 Vent Fan (Cooling)
### 2.2.4 Hen light 

### 2.3 Water heater 

## 2.2 System config

### 2.2.1 Door name
### 2.2.2 Security
### 2.2.3 Date-Time
### 2.2.4 WiFi network
### 2.2.5 ThingSpeak

## 2.3 System info

# 4. Update (web OTA)
/update

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

/accessory
GET
  accessory settings

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

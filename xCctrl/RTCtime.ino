// the code in this file is copied from https://github.com/trunet/DS3231RTC
// the reason is that if I keep it in the library as a class, is in conflict with some other library and crashes my esp8266
// If I put it in a separate file, like bellow, it works well.

// registers
#define DS3231_CONTROL_ADDR         0x0E
#define DS3231_STATUS_ADDR          0x0F

#define DS3231_RTC_ADDR 0x68

/*bool rtc_detect() {
  Wire.beginTransmission(DS3231_RTC_ADDR);
  if (!Wire.endTransmission()) {
    return true;
  }
  return false;
  }*/

// PUBLIC FUNCTIONS
time_t rtc_get() { // Aquire data from buffer and convert to time_t
  tmElements_t tm;
  rtc_read(tm);
  return (makeTime(tm));
}

void  rtc_set(time_t t) {
  tmElements_t tm;
  breakTime(t, tm);
  tm.Second |= 0x80;  // stop the clock
  rtc_write(tm);
  tm.Second &= 0x7f;  // start the clock
  rtc_write(tm);
}

// Aquire data from the RTC chip in BCD format
void rtc_read(tmElements_t &tm) {
  Wire.beginTransmission(DS3231_RTC_ADDR);
  Wire.write((uint8_t)0);
  Wire.endTransmission();

  // request the 7 data fields   (secs, min, hr, dow, date, mth, yr)
  Wire.requestFrom(DS3231_RTC_ADDR, tmNbrFields);

  tm.Second = rtc_bcd2dec(Wire.read() & 0x7f);
  tm.Minute = rtc_bcd2dec(Wire.read());
  tm.Hour = rtc_bcd2dec(Wire.read() & 0x3f);  // mask assumes 24hr clock
  tm.Wday = rtc_bcd2dec(Wire.read());
  tm.Day = rtc_bcd2dec(Wire.read());
  tm.Month = rtc_bcd2dec(Wire.read());
  tm.Year = y2kYearToTm((rtc_bcd2dec(Wire.read())));
}

void rtc_write(tmElements_t &tm) {
  Wire.beginTransmission(DS3231_RTC_ADDR);
  Wire.write((uint8_t)0); // reset register pointer

  Wire.write(rtc_dec2bcd(tm.Second));
  Wire.write(rtc_dec2bcd(tm.Minute));
  Wire.write(rtc_dec2bcd(tm.Hour));      // sets 24 hour format
  Wire.write(rtc_dec2bcd(tm.Wday));
  Wire.write(rtc_dec2bcd(tm.Day));
  Wire.write(rtc_dec2bcd(tm.Month));
  Wire.write(rtc_dec2bcd(tmYearToY2k(tm.Year)));

  Wire.endTransmission();
}

float rtc_getTemp() {
  //temp registers (11h-12h) get updated automatically every 64s
  Wire.beginTransmission(DS3231_RTC_ADDR);
  Wire.write(0x11);  //DS3231_TEMPERATUREREG
  Wire.endTransmission();
  Wire.requestFrom(DS3231_RTC_ADDR, 2);

  if (Wire.available()) {
    int8_t msb = Wire.read(); //2's complement int portion
    uint8_t lsb = Wire.read(); //fraction portion
    //temp3231 = (tMSB & B01111111); //do 2's math on Tmsb
    //temp3231 += ((tLSB >> 6) * 0.25); //only care about bits 7 & 8
    //temp3231 = ((((short)tMSB << 8) | (short)tLSB) >> 6) / 4.0;
    //temp3231 = (temp3231 * 1.8) + 32.0;
    //return temp3231;
    return (float)msb + (lsb >> 6) / 4.0; //((((short)tMSB << 8) | (short)tLSB) >> 6) / 4.0
  }
  else {
    //oh noes, no data!
    return -127.0;
  }
  return 0;
}
// PRIVATE FUNCTIONS

// Convert Decimal to Binary Coded Decimal (BCD)
uint8_t rtc_dec2bcd0(uint8_t num) {
  return ((num / 10 * 16) + (num % 10));
}
uint8_t rtc_dec2bcd(uint8_t n) {   // version4
  uint16_t a = n;
  byte b = (a * 26) >> 8; // b = (a*13) >> 7  was OK but slower!
  return  n + b * 6;
}
uint8_t DS3232RTC_dec2bcd(uint8_t n) {
  return n + 6 * (n / 10);
}
#define uRTCLIB_decToBcd(val) ((uint8_t) ((val / 10 * 16) + (val % 10)))
uint8_t RTCx_decToBcd(uint8_t b) {
  return ( ((b / 10) << 4) + (b % 10) );
}
inline uint8_t decToBcd(uint8_t val) {
  return (val / 10 * 16) + (val % 10);
}

// Convert Binary Coded Decimal (BCD) to Decimal
uint8_t rtc_bcd2dec0(uint8_t num) {
  return ((num / 16 * 10) + (num % 16));
}
uint8_t rtc_bcd2dec(uint8_t val) {
  return val - 6 * (val >> 4);
}
#define uRTCLIB_bcdToDec(val) ((uint8_t) ((val / 16 * 10) + (val % 16)))
uint8_t RTCx_bcdToDec(uint8_t b) {
  return ( ((b >> 4) * 10) + (b % 16) );
}
inline uint8_t bcdToDec(uint8_t val) {
  return (val / 16 * 10) + (val % 16);
}

// PROTECTED FUNCTIONS

void rtc_set_sreg(uint8_t val) {
  Wire.beginTransmission(DS3231_RTC_ADDR);
  Wire.write(DS3231_STATUS_ADDR);
  Wire.write(val);
  Wire.endTransmission();
}

uint8_t rtc_get_sreg() {
  uint8_t rv;

  Wire.beginTransmission(DS3231_RTC_ADDR);
  Wire.write(DS3231_STATUS_ADDR);
  Wire.endTransmission();

  Wire.requestFrom(DS3231_RTC_ADDR, 1);
  rv = Wire.read();

  return rv;
}

void rtc_set_creg(uint8_t val) {
  Wire.beginTransmission(DS3231_RTC_ADDR);
  Wire.write(DS3231_CONTROL_ADDR);
  Wire.write(val);
  Wire.endTransmission();
}

uint8_t rtc_get_creg() {
  Wire.beginTransmission(DS3231_RTC_ADDR);
  Wire.write(DS3231_CONTROL_ADDR);
  Wire.endTransmission();

  Wire.requestFrom(DS3231_RTC_ADDR, 1);
  uint8_t rv = Wire.read();

  return rv;
}

bool rtc_lostPower() {
  uint8_t status = rtc_get_sreg();
  return ((status & 0B10000000) == 0B10000000);
  //  return (read_i2c_register(DS3231_ADDRESS, DS3231_STATUSREG) >> 7);
}

void rtc_lostPowerClear() {
  uint8_t status = rtc_get_sreg();
  status &= 0b01111111;
  rtc_set_sreg(status);
}

bool rtc_Battery() {
  uint8_t status = rtc_get_creg();
  // Return the status bit as a bool, to check against values of Control Register (0Eh)
  status &= 0b10000000;
  return (status == 0b10000000);
}

void rtc_enableBattery() {
  uint8_t status = rtc_get_creg();
  status &= 0b01111111;
  rtc_set_creg(status);
}

void rtc_disableBattery() {
  uint8_t status = rtc_get_creg();
  status |= 0b10000000;
  rtc_set_creg(status);
}


/*
  bool uRTCLib::lostPower() {
        uint8_t status;

  switch (_model) {
    case URTCLIB_MODEL_DS1307:
      uRTCLIB_YIELD
      URTCLIB_WIRE.beginTransmission(_rtc_address);
      URTCLIB_WIRE.write(0X00);
      URTCLIB_WIRE.endTransmission();
      uRTCLIB_YIELD
      URTCLIB_WIRE.requestFrom(_rtc_address, 1);
      status = URTCLIB_WIRE.read();
      uRTCLIB_YIELD
      return ((status & 0B10000000) == 0B10000000);
      break;

    // case URTCLIB_MODEL_DS3231: // Commented out because it's default mode
    // case URTCLIB_MODEL_DS3232: // Commented out because it's default mode
    default:
      uRTCLIB_YIELD
      URTCLIB_WIRE.beginTransmission(_rtc_address);
      URTCLIB_WIRE.write(0X0F);
      URTCLIB_WIRE.endTransmission();
      uRTCLIB_YIELD
      URTCLIB_WIRE.requestFrom(_rtc_address, 1);
      status = URTCLIB_WIRE.read();
      uRTCLIB_YIELD
      return ((status & 0B10000000) == 0B10000000);
      break;
  }
  }
*/

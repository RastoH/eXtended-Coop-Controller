#ifdef NTP_SUPPORT

// now convert NTP time into everyday time:
// Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
#define SEVENTY_YEARS 2208988800UL

#ifdef USE_ASYNC
AsyncUDP *_ntp_udp  = NULL;
#else
// WiFiUDP _ntp_udp;                         // A UDP instance to let us send and receive packets over UDP
WiFiUDP *_ntp_udp = NULL;
#endif

#define NTP_PACKET_SIZE           48              // NTP time stamp is in the first 48 bytes of the message
#define NTP_DEFAULT_LOCAL_PORT    2390  //1337     //1337  // 2390  // local port to listen on for UDP packets 
#define NTP_DEFAULT_PORT          123            // Default local udp port. Select a different one if neccesary (usually not needed)
/** SNTP receive timeout - in milliseconds
 * Also used as retry timeout - this shouldn't be too low.
 * Default is 3 seconds.
 */
#define NTP_MAX_WAIT_TIME        3000
#define NTP_updateLongInterval   600 * 60 *1000  // In ms   / kazdu hodinu a pol - 90minut
#define NTP_updateShortInterval  30 * 60 * 1000  // In ms  / kazdych 30 minut

enum NTP_TIME_STATE {
  ntp_idle = 0,
  ntp_triger = 1,
  ntp_init = 2,
  ntp_send = 3,
  ntp_receive = 4,
  ntp_stop = 5
} NTP_state = ntp_idle;

unsigned long NTP_lastUpdate     = 0;      // In ms
unsigned long NTP_requestSendTime = 0;
time_t NTP_currentEpoch = 1451602800;

int ntpTimeOffset     = 0;

bool ntpTime_sendPacket() {
  uint8_t ntpPacketBuffer[NTP_PACKET_SIZE];         //Buffer to store request message
  memset(ntpPacketBuffer, 0, NTP_PACKET_SIZE);      // set all bytes in the buffer to 0
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
//ntpPacketBuffer[0] = 0b00011011; //  (0x1B) for NTPv3 
// or
// ntpPacketBuffer[0] = 0b00100011;` (0x23) for NTPv4 
  ntpPacketBuffer[0] = 0b11100011;   // LI, Version, Mode
  ntpPacketBuffer[1] = 0;     // Stratum, or type of clock
  ntpPacketBuffer[2] = 6;     // Polling Interval (9 = 2^9 secs = ~9 mins, close to our 10 min default)
  ntpPacketBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  ntpPacketBuffer[12] = 49;
  ntpPacketBuffer[13] = 0x4E;
  ntpPacketBuffer[14] = 49;
  ntpPacketBuffer[15] = 52;
  /*
    ntpPacketBuffer[12] = 'X';      // "kiss code", see RFC5905
    ntpPacketBuffer[13] = 'E';      // (codes starting with 'X' are not interpreted)
    ntpPacketBuffer[14] = 'Z';
    ntpPacketBuffer[15] = 'T';
  */
  // all NTP fields have been given values, now you can send a packet requesting a timestamp:
#ifdef USE_ASYNC
  AsyncUDPMessage ntpPacket = AsyncUDPMessage();
  ntpPacket.write(ntpPacketBuffer, NTP_PACKET_SIZE);
  if (_ntp_udp->send(ntpPacket)) {
#else

  while (_ntp_udp->parsePacket() > 0) {          // Discard any previously received packets
    yield();
  }

  if (_ntp_udp->beginPacket(settings.ntpServerName, NTP_DEFAULT_PORT) == 1) {  //NTP requests are to port 123
    _ntp_udp->write(ntpPacketBuffer, NTP_PACKET_SIZE);
    _ntp_udp->endPacket();
#endif
    NTP_requestSendTime = millis();
    //Logger_debug("NTP - send OK");
    return true;
  }
  else {
    Logger_error("NTP - send to %s error", settings.ntpServerName);
    return false;
  }
}

/*
  void NTP_forceUpdate() {
  Logger_info("NTP - forceUpdate");
        stav.NTP_updated = false;

  if (ntpTime_init()) {
    if (ntpTime_sendPacket()) {
      Logger_info("NTP - forceUpdate - request send - wait to response");
      int cb = _ntp_udp->parsePacket();
      //unsigned long aktualne = (millis() - NTP_requestSendTime);
      while (cb == 0 && ((millis() - NTP_requestSendTime) < NTP_MAX_WAIT_TIME)) {
        delay(100);
        aktualne = millis() - NTP_requestSendTime;
        cb = _ntp_udp->parsePacket();
      }
      if (cb == 0) {
        Logger_error("NTP - forceUpdate - TIMEOUT");
      } else {
        NTP_currentEpoch = ntpTime_decodeMessage();
        if (stav.rtc_installed)
          rtc_set(NTP_currentEpoch); // +daylightSaving
        setTime(NTP_currentEpoch);
        stav.NTP_updated = true;
        Logger_info("NTP - forceUpdate updated.");
      }
    }
    ntpTime_stop();
  }
  NTP_lastUpdate = millis();
  }
*/

void NTP_loop(bool ntp_force) {
  switch (NTP_state) {
    case ntp_idle:
      {
        //        if (!stav.NTP_updated  && (NTP_lastUpdate == 0)) {
        //          NTP_state = ntp_triger;
        //          break;
        //        }
        if (ntp_force) {
          NTP_state = ntp_triger;
          return;
        }
        uint32_t last_run = millisLastRun(millis(), NTP_lastUpdate);

        if (stav.NTP_updated && (last_run <  NTP_updateLongInterval)) return;
        if (!stav.NTP_updated && (last_run < NTP_updateShortInterval)) return;
        NTP_state = ntp_triger;
      }
      break;
    case ntp_triger:  // Starting UDP
      if (WiFi_connectedSTA() && strlen(settings.ntpServerName)) {
        NTP_state = ntp_init;
        stav.NTP_updated = false;
      } else {
        // Logger_error("NTP - no WiFi STA CONNECTED // no ntp server name?");
        NTP_state = ntp_idle;
      }
      break;
    case ntp_init :
      if (ntpTime_init()) {
        NTP_state = ntp_send;
      } else {
        Logger_error("NTP - UDP init error");
        NTP_state = ntp_stop;
      }
      break;
    case ntp_send:
      if (ntpTime_sendPacket()) {
        NTP_state = ntp_receive;
      } else {
        NTP_state = ntp_stop;
        Logger_error("NTP - send packet to %s error", settings.ntpServerName);
      }
      break;
    case ntp_receive:
      {
        unsigned long aktualny = millisLastRun(millis(), NTP_requestSendTime);
#ifndef USE_ASYNC
        int cbp = _ntp_udp->parsePacket();
        if (cbp >= NTP_PACKET_SIZE && aktualny < NTP_MAX_WAIT_TIME) {
          NTP_state = ntp_stop;
          uint8_t ntpPacketBuffer[NTP_PACKET_SIZE]; //Buffer to store response message
          _ntp_udp->read (ntpPacketBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
          NTP_currentEpoch = ntpTimeDecodeMessage(ntpPacketBuffer);   //ntpTime_decodeMessage();                // We've received a packet, read the data from it
          setTime(NTP_currentEpoch);
          Logger_info("NTP - clock synced with %s", settings.ntpServerName);
          if (stav.rtc_installed) {
            rtc_set(NTP_currentEpoch); // +daylightSaving
            //Logger_info("NTP update - RTC synced with NTP.");
          }
          stav.NTP_updated = true;
          NTP_state = ntp_stop;
          //Logger_info("NTP - updated");
        } else
#endif
          if (aktualny > NTP_MAX_WAIT_TIME) {
            NTP_state = ntp_stop;
            Logger_error("NTP - TIMEOUT");
          }
      }
      break;
    case ntp_stop:
      ntpTime_stop();
      NTP_lastUpdate = millis();
      NTP_state = ntp_idle;
      break;
  }
}

//#ifdef USE_ASYNC
time_t ntpTimeDecodeMessage(uint8_t *messageBuffer) {
  if ((messageBuffer[0] & 0b11000000) == 0b11000000) {
    // Leap-Indicator: unknown (clock unsynchronized)
    // See: https://github.com/letscontrolit/ESPEasy/issues/2886#issuecomment-586656384
    //AddLog(LOG_LEVEL_DEBUG, PSTR("NTP: IP %_I unsynched"), (uint32_t)time_server_ip);
    //ntp_server_id++;                            // Next server next time
    return 0;
  }

  // convert four bytes starting at location 40 to a long integer
  uint32_t secsSince1900 = (uint32_t)messageBuffer[40] << 24;
  secsSince1900 |= (uint32_t)messageBuffer[41] << 16;
  secsSince1900 |= (uint32_t)messageBuffer[42] << 8;
  secsSince1900 |= (uint32_t)messageBuffer[43];

  //DEBUGLOG ("Secs: %lu \n", secsSince1900);

  if (secsSince1900 == 0) {
    //DEBUGLOG ("--Timestamp is Zero\n");
    return 0;
  }
  //time_t timeTemp = secsSince1900 - SEVENTY_YEARS;
  return secsSince1900 - SEVENTY_YEARS;  //timeTemp;
}

/*
  time_t ntpTime_decodeMessage() {
  uint8_t NTPpacketBuffer[NTP_PACKET_SIZE]; //Buffer to store request message
  // We've received a packet, read the data from it
  _ntp_udp->read(NTPpacketBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
  //the timestamp starts at byte 40 of the received packet and is four bytes,
  // or two words, long. First, extract the two words:
  unsigned long highWord = word(NTPpacketBuffer[40], NTPpacketBuffer[41]);
  unsigned long lowWord = word(NTPpacketBuffer[42], NTPpacketBuffer[43]);
  // combine the four bytes (two words) into a long integer
  // this is NTP time (seconds since Jan 1 1900):
  unsigned long secsSince1900 = highWord << 16 | lowWord;

  //prepare timestamps
  //  uint32_t highWord, lowWord;
  highWord = ( NTPpacketBuffer[16] << 8 | NTPpacketBuffer[17] ) & 0x0000FFFF;
  lowWord = ( NTPpacketBuffer[18] << 8 | NTPpacketBuffer[19] ) & 0x0000FFFF;
  uint32_t reftsSec = highWord << 16 | lowWord;       // reference timestamp seconds

  highWord = ( NTPpacketBuffer[32] << 8 | NTPpacketBuffer[33] ) & 0x0000FFFF;
  lowWord = ( NTPpacketBuffer[34] << 8 | NTPpacketBuffer[35] ) & 0x0000FFFF;
  uint32_t rcvtsSec = highWord << 16 | lowWord;       // receive timestamp seconds

  highWord = ( NTPpacketBuffer[40] << 8 | NTPpacketBuffer[41] ) & 0x0000FFFF;
  lowWord = ( NTPpacketBuffer[42] << 8 | NTPpacketBuffer[43] ) & 0x0000FFFF;
  // uint32_t secsSince1900 = highWord << 16 | lowWord;      // transmit timestamp seconds

  highWord = ( NTPpacketBuffer[44] << 8 | NTPpacketBuffer[45] ) & 0x0000FFFF;
  lowWord = ( NTPpacketBuffer[46] << 8 | NTPpacketBuffer[47] ) & 0x0000FFFF;
  uint32_t fraction = highWord << 16 | lowWord;       // transmit timestamp fractions

  if ((NTPpacketBuffer[1] < 1) or (NTPpacketBuffer[1] > 15) or (reftsSec == 0) or (rcvtsSec == 0) or (rcvtsSec > secsSince1900)) {
    // we got invalid packet
    //logger_error triggerError(INVALID_DATA);
    Logger_error("NTP - invalid data");
    return 0;
  }

  // subtract seventy years:
  unsigned long timeTemp = secsSince1900 - SEVENTY_YEARS;  // + (settings.timeZone * SECS_PER_HOUR / *SECONDS_PER_HOUR* /);
  //  if (settings.DST && summertime(year (timeTemp), month (timeTemp), day (timeTemp), hour (timeTemp), settings.DSTregion))
  //if (settings.DST && isSummerTimePeriod())
  //  timeTemp += SECS_PER_HOUR;  //DEBUGLOG ("Summer Time\n");
  return timeTemp;
  }
*/

unsigned long ntpTime_getTime() {
  return (settings.timeZone * SECS_PER_HOUR) + // User offset
         NTP_currentEpoch + // Epoc returned by the NTP server
         ((millis() - NTP_lastUpdate) / 1000); // Time since last update
}

void ntpTime_setTime() {
  setTime(ntpTime_getTime());
}

// Starting UDP
bool ntpTime_init() {
  if (_ntp_udp) {
    return false;
  }
#ifdef USE_ASYNC
  _ntp_udp = new AsyncUDP();

  if (!_ntp_udp)
    return false;

  IPAddress timeServerIP; //NTP server IP address
  WiFi.hostByName(settings.ntpServerName, timeServerIP);
  if (_ntp_udp->connect(timeServerIP, NTP_DEFAULT_PORT)) {
    _ntp_udp->onPacket([](AsyncUDPPacket ntpPacket) {
      uint8_t *ntpPacketBuffer;
      if (NTP_state == ntp_receive) {
        int size = ntpPacket.length ();
        if (size >= NTP_PACKET_SIZE) {
          ntpPacketBuffer = ntpPacket.data();
          NTP_currentEpoch = ntpTimeDecodeMessage(ntpPacketBuffer);

          if (stav.rtc_installed) {
            rtc_set(NTP_currentEpoch); // +daylightSaving
            Logger_info("NTP update - RTC synced with NTP.");
          }
          setTime(NTP_currentEpoch);
          Logger_info("NTP - update - Local clock synced with NTP");
          stav.NTP_updated = true;
          NTP_state = ntp_stop;
          Logger_info("NTP - updated");
        }
      }
    });
#else
  _ntp_udp = new WiFiUDP();

  if (!_ntp_udp)
    return false;

  if (_ntp_udp->begin(NTP_DEFAULT_LOCAL_PORT)) {
    if (_ntp_udp->parsePacket() > 0)
      _ntp_udp->flush(); // discard any previously received packets
#endif
    //Logger_debug("NTP - init OK");
    return true;
  } else {
    Logger_error("NTP - init error");
    return false;
  }
}

bool ntpTime_stop() {
  if (!_ntp_udp)
    return false;

#ifdef USE_ASYNC
  _ntp_udp->close();
  if (_ntp_udp) {
    AsyncUDP * _udp  = _ntp_udp;
    _ntp_udp = NULL;
    delete _udp;
  }
#else
  if (_ntp_udp->parsePacket() > 0)
    _ntp_udp->flush();
  _ntp_udp->stop();

  if (_ntp_udp) {
    WiFiUDP * _udp  = _ntp_udp;
    _ntp_udp = NULL;
    delete _udp;
  }
#endif
  if (_ntp_udp) {
    Logger_error("NTP not stopped");
    return false;
  }
  return true;
}

#endif

bool ntpSynced() {
  return stav.NTP_updated;
  //return (year(now()) >= 2020);
}

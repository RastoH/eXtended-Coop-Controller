#ifdef ThingSpeak

static inline char const *bool2str(_Bool b) {
  return "false\0true" + 6 * b;
}

void handleGETthngspk() {
  char buff[256];
  int len;

  len = sprintf_P(buff,
                  PSTR("{\"thngChannel\":\"%u\",\"thngReadAPI\":\"%s\",\"thngWriteAPI\":\"%s\",\"thngState\":\"%d\",\"thngEnabled\":%s}"),
                  settings.thngChannel,
                  settings.thngReadAPIkey,
                  settings.thngWriteAPIkey,
                  stav.tspk_lastSendResponse,
                  bool2str(settings.thngE));

  server_sendHeader();
  webServer.setContentLength(len);
  webServer.send(200, F("application/json"), buff);
  webServer.client().stop();                 // Stop is needed because we sent no content length
}


#define TS_OK_SUCCESS              200     // OK / Success
#define TS_ERR_BADAPIKEY           400     // Incorrect API key (or invalid ThingSpeak server address)
#define TS_ERR_BADURL              404     // Incorrect API key (or invalid ThingSpeak server address)
#define TS_ERR_OUT_OF_RANGE        -101    // Value is out of range or string is too long (> 255 bytes)
#define TS_ERR_INVALID_FIELD_NUM   -201    // Invalid field number specified
#define TS_ERR_SETFIELD_NOT_CALLED -210    // setField() was not called before writeFields()
#define TS_ERR_CONNECT_FAILED      -301    // Failed to connect to ThingSpeak
#define TS_ERR_UNEXPECTED_FAIL     -302    // Unexpected failure during write to ThingSpeak
#define TS_ERR_BAD_RESPONSE        -303    // Unable to parse response
#define TS_ERR_TIMEOUT             -304    // Timeout waiting for server to respond
#define TS_ERR_NOT_INSERTED        -401    // Point was not inserted (most probable cause is the rate limit of once every 15 seconds) 

#define THINGSPEAK_URL  "api.thingspeak.com"                    /*"http://api.thingspeak.com"*/
#if ASYNC_TCP_SSL_ENABLED
#define THINGSPEAK_PORT_NUMBER 443
#else
#define THINGSPEAK_PORT_NUMBER 80
#endif
//IPAddress THINGSPEAK_IP(184, 106, 153, 149);

#define THINGSPEAK_TIMEOUT_MS_SERVERRESPONSE 5000  // Wait up to five seconds for server to respond

const char THINGSPEAK_REQUEST_TEMPLATE[] PROGMEM =
  "POST /update HTTP/1.1\r\n"
  "Host: api.thingspeak.com\r\n"
  "Server: eXtended Coop Controller\r\n"
  "User-Agent: xcctrl/ESP8266\r\n"
  "X-THINGSPEAKAPIKEY: %s\r\n"
  "Connection: close\r\n"
  //"Accept: application/json\r\n"  //skuska
  //"Cache-Control: no-cache\r\n"   //skuska
  "Content-Type: application/x-www-form-urlencoded\r\n"
  "Content-Length: %d\r\n\r\n"
  "%s\r\n";

//#define ASYNC_TCP_SSL_ENABLED false
#include <SyncClient.h>  // #include <ESPAsyncTCP.h>
//static
SyncClient * _tspk_client = NULL;
//WiFiClient * _tspk_client;  // for ThingSpeak NEW

void Thing_Speak_loop(bool _ts_force) {
  switch (stav.tspk_state) {
    case ts_idle: {
        if (!settings.thngE) return;
        if (!_ts_force) return;

        //Logger_setup("TSPK - force");
        if (strlen(settings.thngWriteAPIkey) != 0) {
          stav.tspk_state = ts_trigger;
          stav.tspk_lastSendResponse = 1;
          return;
        } else {
          stav.tspk_lastSendResponse = TS_ERR_BADAPIKEY; // Incorrect API key
          Logger_error("TSPK - no api/write key!");
          settings.thngE = false;
          return;
        }
      }
      break;
    case ts_trigger : {
        if (WiFi_connectedSTA()) {
          stav.tspk_state = ts_init;
          stav.tspk_lastSendResponse = 2;
          return;
        }
        stav.tspk_state = ts_idle;
        stav.tspk_lastSendResponse = -2;
      }
      break;
    case ts_init:
      {
        stav.tspk_lastSendResponse = 3;
        //Logger_setup("TSPK - setup..");
        if (_tspk_init()) {
          stav.tspk_state = ts_connect;
          return;
        } else {
          Logger_error("TSPK - init");
          stav.tspk_state = ts_stop;
          stav.tspk_lastSendResponse = -3;
          return;
        }
      }
      break;
    case ts_connect :
      {
        //Logger_debug("TSPK - connect..");
        if (_tspk_connect()) {
          //Logger_info("TSPK: connect Success.");
          stav.tspk_lastSendResponse = 4;
          stav.tspk_state = ts_send;
          return;
        } else {
          Logger_error("TSPK: connect Failed.");
          stav.tspk_lastSendResponse = TS_ERR_CONNECT_FAILED;
          stav.tspk_state = ts_stop;
          return;
        }
      }
      break;
    case ts_send:
      {
        stav.tspk_state = ts_receive;
        //Logger_debug("TSPK: send");
        if (!_tspk_send()) {
          Logger_error("TSPK: unexpected send fail");
          stav.tspk_lastSendResponse = TS_ERR_UNEXPECTED_FAIL;
          stav.tspk_state = ts_stop;
          return;
        } else {
          //Logger_debug("TSPK: send OK");
          stav.tspk_lastSendResponse = 5;
          _tspk_client->flush();
          stav.tspk_sendMillis = millis();
          stav.tspk_state = ts_receive;
          return;
        }
      }
      break;
    case ts_receive:
      {
        if ((stav.tspk_sendMillis + THINGSPEAK_TIMEOUT_MS_SERVERRESPONSE >= millis()) && (_tspk_client->available())) {  // prisla odpoved
          //Logger_info("TSPK: response");
          stav.tspk_lastSendResponse = TS_ERR_BAD_RESPONSE;
          if (!_tspk_receive())
            Logger_error("TSPK: TS_ERR_BAD_RESPONSE / Unable to parse response");
          stav.tspk_state = ts_stop;
          return;
        } else if (stav.tspk_sendMillis + THINGSPEAK_TIMEOUT_MS_SERVERRESPONSE < millis()) { // ukonci   time out
          stav.tspk_lastSendResponse = TS_ERR_TIMEOUT;
          Logger_error("TSPK: response timeout");
          stav.tspk_state = ts_stop;
          return;
        }
      }
      break;
    case ts_stop:
      {
        _tspk_stop();
        stav.tspk_state = ts_idle;
        return;
      }
      break;
    case ts_setup:
      {
        stav.tspk_state = ts_idle;
      }
      break;
  }
}

bool _tspk_init() {
  if (_tspk_client) //client already exists
    return false;

  _tspk_client = new SyncClient();

  if (!_tspk_client)
    return false;

  return true;
}

bool _tspk_connect() {
  //return _tspk_client->connect(THINGSPEAK_URL, THINGSPEAK_PORT_NUMBER);
  //return _tspk_client->connect(THINGSPEAK_IP, THINGSPEAK_PORT_NUMBER);

  if (!_tspk_client->connect(THINGSPEAK_URL, THINGSPEAK_PORT_NUMBER)) {
    //  if (!_tspk_client->connect(THINGSPEAK_IP, THINGSPEAK_PORT_NUMBER)) {
    return false;
  }
  return true;
}

bool _tspk_stop() {
  if (!_tspk_client)
    return true;

  _tspk_client->stop();

  if (_tspk_client) {
    SyncClient * client = _tspk_client;
    _tspk_client = NULL;
    delete client;
  }

  if (_tspk_client)
    return false;

  return true;
}

bool _tspk_send() {
  uint32_t esp_free_heap = ESP.getFreeHeap();

  char temp[256];

  if (stav.outsideTeplota != -127) {
    sprintf_P(temp, PSTR("field1=%.1f"), stav.outsideTeplota);
  }
  if (stav.insideTeplota != -127) {
    if (strlen(temp))
      sprintf_P(temp + strlen(temp), PSTR("&"));
    sprintf_P(temp + strlen(temp), PSTR("field2=%.1f"), stav.insideTeplota);
  }
  if (strlen(temp))
    sprintf_P(temp + strlen(temp), PSTR("&"));

  sprintf_P(temp + strlen(temp), PSTR("field3=%.1f&field4=%s&field5=%d&field6=%f&field7=%u&field8=%u&headers=false&created_at=%s"),
            stav.lightSensor_value,
            (stav.door_status == DOOR_STATE_OPEN) ? "1" : (stav.door_status == DOOR_STATE_CLOSED) ? "0" : (stav.door_status >= DOOR_STATE_ERROR) ? "-1" : "0.5", // stav.door_status
            stav.lightSensor.actual,
            stav.lux,
            stav.loop_load_avg,
            esp_free_heap,
            ISO_DateTime(false).c_str());
  /*
    if (stav.door_status >= DOOR_STATE_ERROR) {
      postMessage.concat(F("&status = \""));
      switch (stav.door_status) {
        case DOOR_STATE_ERROR:
          postMessage.concat(F("door ERROR - short switches"));
          break;
        case DOOR_STATE_ERROR_TIMEOUT:
          postMessage.concat(F("door ERROR Timeout"));
          break;
        case DOOR_STATE_ERROR_REOPEN:
          postMessage.concat(F("door ERROR Reopening"));
          break;
        case DOOR_STATE_ERROR_FALL:
          postMessage.concat(F("door ERROR Door Fall!"));
          break;
        case DOOR_STATE_ERROR_TIMEOUT_OPEN:
          postMessage.concat(F("door ERROR Open Timeout"));
          break;
        case DOOR_STATE_ERROR_TIMEOUT_CLOSE:
          postMessage.concat(F("door ERROR Close Timeout"));
          break;
        case DOOR_STATE_WARNING_PREDATOR:
          postMessage.concat(F("door WARNING door not closed (predator?)"));
          break;
        case DOOR_STATE_MOTOR:
          postMessage.concat(F("door MOTOR ERROR motor not installed? WEMOS, LOLIN, drv8830"));
          break;
      }
      postMessage.concat(F("\""));
    }
  */
  //Logger_info("TSPK: postMessage length %d   '%s'", postMessage.length(), postMessage.c_str());

  char buff[strlen_P(THINGSPEAK_REQUEST_TEMPLATE) + strlen(temp) + strlen(settings.thngWriteAPIkey) + 5];
  size_t _out = snprintf_P(buff, sizeof(buff), THINGSPEAK_REQUEST_TEMPLATE, settings.thngWriteAPIkey, strlen(temp), temp);
  if (strlen(temp) > 255)
    Logger_error("TSPK: postmessage length > 255");
  //postMessage.clear();
  //Logger_info("%s", buffer);
  //Logger_info("TSPK - print..");
  //send the request
  //_tspk_client->setTimeout(uint32_t seconds);   THINGSPEAK_TIMEOUT_MS_SERVERRESPONSE
  return _tspk_client->write((uint8_t*)buff, _out);
  //_tspk_client->print(buffer);
}

bool _tspk_receive() {
  //unsigned long _content_length;
  while (_tspk_client->available()) {
    size_t len = _tspk_client->available();
    if (len > 0) {
      int headerSeparator = -1;
      String headerLine = _tspk_client->readStringUntil('\n');
      //      headerLine.trim(); // remove \r
      //      Logger_web("TSPK1: [handleHeaderResponse=>] '%s'", headerLine.c_str());

      if (headerLine.startsWith(F("HTTP/1."))) {
        constexpr auto httpVersionIdx = sizeof "HTTP/1." - 1;
        //bool _canReuse = (headerLine[httpVersionIdx] != '0');    //  version of HTTP/1.x
        int _returnCode = headerLine.substring(httpVersionIdx + 2, headerLine.indexOf(' ', httpVersionIdx + 2)).toInt();
        //Logger_web("TSPK: [http] return code: '%d' => '%s' ", _returnCode, headerLine.c_str());
        //if (_returnCode == TS_OK_SUCCESS)
        stav.tspk_lastSendResponse = _returnCode;
        headerLine.trim();
        if (stav.tspk_lastSendResponse != TS_OK_SUCCESS)
          Logger_error("TSPK: [HEADER] '%s'", headerLine.c_str());
        continue;
      } else if ((headerSeparator = headerLine.indexOf(':')) > 0) {
        String headerName = headerLine.substring(0, headerSeparator);
        String headerValue = headerLine.substring(headerSeparator + 1);
        headerValue.trim();
        
        if (stav.tspk_lastSendResponse != TS_OK_SUCCESS)
          Logger_error("TSPK: [HEADER] '%s' : '%s'",  headerName.c_str(), headerValue.c_str());

        //Logger_web("TSPK: [handleHeaderResponse header] '%s' = '%s'",);
        //if (headerName.equalsIgnoreCase(F("Content-Length"))) {
        //_content_length = headerValue.toInt();
        //Logger_web("TSPK: [Content-Length]:   %d", headerValue.toInt());
        //}
        if (headerName.equalsIgnoreCase(F("Status"))) {
          int _separator = headerValue.indexOf(" ");
          // get the substrings for hour, minute second:
          String _return_code = headerValue.substring(0, _separator);
          String _return_text = headerValue.substring(_separator + 1);
          //Logger_web("TSPK: [Status]  code: '%s'   text: '%s'", _return_code.c_str(), _return_text.c_str());
          if (stav.tspk_lastSendResponse != _return_code.toInt()) {
            Logger_error("TSPK: [Status] return code mismatch");
          }
          //if (stav.tspk_lastSendResponse != TS_OK_SUCCESS) {
          //  Logger_error("TSPK: [Status '%s'] '%s'", _return_code.c_str(), _return_text.c_str());
          //}
          //Logger_web("TSPK: [Status '%s'] '%s'", _return_code.c_str(), _return_text.c_str());
        }
        continue;
      }
      headerLine.trim(); // remove \r
      if (headerLine.isEmpty()) {
        //Logger_info("TSPK: [handleHeaderResponse] empty line - wait for response in next line");
      } else {
        if (stav.tspk_lastSendResponse != TS_OK_SUCCESS) {
          Logger_info("TSPK: [RESPONSE] '%s'", headerLine.c_str());
          //Logger_info("TSPK response number of records on server: '%s'", headerLine.c_str());
        }
        //if (_content_length != headerLine.length())
        //Logger_error("TSPK: [RESPONSE] length mishmash '%s'", headerLine.c_str());
      }
    }
  }
  if (stav.tspk_lastSendResponse == 0) {
    stav.tspk_lastSendResponse = TS_ERR_BAD_RESPONSE;    // Unable to parse response
    return false;
  }
  return true;
}

#endif


/*
  DDNS based on EasyDDNS 1.5.2 & Ioan Ghip
*/
#ifdef DYNDNS
#include <ESP8266HTTPClient.h>

#define ddns_update_interval 1800000

unsigned long ddns_previousMillis = 0;

String ddns_checkIP() {
  HTTPClient http;
  String new_ip = F("http://ipv4bot.whatismyipaddress.com/");
  http.begin(F("http://ipv4bot.whatismyipaddress.com/")/*new_ip*/);
  new_ip = "";
  int httpCode = http.GET();
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      new_ip = http.getString();
    }
  }
  /*else {
    http.end();
    http.free()
    return new_ip;
    }*/
  http.end();
  return new_ip;
}

String ddns_updateURL(String new_ip) {
  String update_url = "";
  switch (settings.ddnschoice) {
    case 0: break;
    case 1:
      update_url = F("http://www.duckdns.org/update?domains={DOMAIN}&token={USER}&ip={IP}");
      break;
    case 2:
      update_url = F("http://{USER}:{PASS}@dynupdate.no-ip.com/nic/update?hostname={DOMAIN}&myip={IP}");
      break;
    case 3:
      update_url = F("http://{USER}:{PASS}@members.dyndns.org/v3/update?hostname={DOMAIN}&myip={IP}");
      break;
    case 4:
      update_url = F("http://api.dynu.com/nic/update?hostname={DOMAIN}&myip={IP}&username={USER}&password={PASS}");
      break;
    case 5:
      update_url = F("http://dynamic.name-services.com/interface.asp?command=SetDnsHost&HostName={DOMAIN}&Zone={USER}&DomainPassword={PASS}&Address={IP}");
      break;
  }
  update_url.replace(F("{IP}"), new_ip);
  update_url.replace(F("{DOMAIN}"), String(settings.ddnsdomain));
  update_url.replace(F("{USER}"), String(settings.ddnsuser));
  update_url.replace(F("{PASS}"), String(settings.ddnspass));
  return update_url;
}

void ddns_update(String updateUrl) {
  /*
    dyndns.replace("{DYNDNSIP}", dyndnsServerIP.toString());
    dyndns.replace("{DYNDNSDOMAIN}", String(configuration.dyndnsname));
    dyndns.replace("{IP}", WiFi.localIP().toString());
    http.begin(dyndns); //HTTP
    http.setAuthorization(configuration.dyndnsuser, configuration.dyndnspass);
    int httpCode = http.GET();
    DEBUG_println(httpCode);
    if (httpCode == HTTP_CODE_OK)
    {
    String payload = http.getString();
    Logger_error(F("ThingSpeak - request send error"));
    DEBUG_println(payload);
    }
  */
}

void ddns_loop() {
  if (settings.ddnschoice == 0) {
    return;
  }

  unsigned long currentMillis = millis(); // Calculate Elapsed Time & Trigger

  if (currentMillis - ddns_previousMillis < ddns_update_interval) {
    return;
  }
  ddns_previousMillis = currentMillis;

  String act_ip = ddns_checkIP();
  if (act_ip == stav.ddns_ip) {
    return;
  }
  yield();
  HTTPClient http;
  //  String update_Url = ddns_updateURL(act_ip);
  //  http.begin(update_Url);
  http.begin(ddns_updateURL(act_ip));
  //  http.setAuthorization(configuration.dyndnsuser, configuration.dyndnspass);
  yield();
  int httpCode = http.GET();
  yield();
  if (httpCode > 0) {
    stav.ddns_ip = act_ip;
    String payload = http.getString();
    yield();
    Logger_info("%s", payload.c_str());
  }
  http.end();
}
#endif

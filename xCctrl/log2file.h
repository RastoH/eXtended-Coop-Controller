#define LOGGER_FILE_NAME        "/debug.txt"
#define LOGGER_FILE_NAME_OLD    "/debug_old.txt"
#define LOGGER_FILE_MAXSIZE         131072  //32768 // 32k


/*********************************************************************************************\
   Logger
  \*********************************************************************************************/

#ifdef Debug_XCTRL

#include <LittleFS.h>
static File LoggerFile;

//#define DEBUG_ESP_CORE
//#define DEBUG_ESP_WIFI
//#define DEBUG_ESP_HTTP_SERVER
//#define DEBUG_OUTPUT LoggerFile
//#define DEBUG_ESP_PORT LoggerFile

//DEBUG_HTTPCLIENT(fmt, ...) DEBUG_ESP_PORT.printf_P( (PGM_P)PSTR(fmt), ## __VA_ARGS__ )
#define Logger_Debug(_1, ...)     { Logger_checkFile(); LoggerFile.printf_P( (PGM_P)PSTR("[%s, line %u] " _1 "\n"), __func__, __LINE__ ,##__VA_ARGS__);} // this saves around 5K RAM...  39,604 K ram left
#define Logger_Debug_raw(_1, ...) { Logger_checkFile(); LoggerFile.printf_P( (PGM_P)PSTR(_1  "\n") , ##__VA_ARGS__);}
#define Logger_log(_1, ...)       { Logger_checkFile(); LoggerFile.printf_P( (PGM_P)PSTR("[%s, line %u]\n"), __func__, __LINE__ );}
#define Logger_debug(_1, ...)     { Logger_checkFile(); LoggerFile.printf_P( (PGM_P)PSTR("[DEBUG %s] " _1 "\n"), Logger_stamp().c_str() ,##__VA_ARGS__);}
#define Logger_setup(_1, ...)     { Logger_checkFile(); LoggerFile.printf_P( (PGM_P)PSTR("[SETUP %s] " _1 "\n"), Logger_stamp().c_str() ,##__VA_ARGS__);}
#define Logger_web(_1, ...)       { Logger_checkFile(); LoggerFile.printf_P( (PGM_P)PSTR("[WEB %s] " _1 "\n"), Logger_stamp().c_str() ,##__VA_ARGS__);}
#define Logger_wifi(_1, ...)      { Logger_checkFile(); LoggerFile.printf_P( (PGM_P)PSTR("[WiFi %s] " _1 "\r\n"), Logger_stamp().c_str() ,##__VA_ARGS__);}
#define Logger_info(_1, ...)      { Logger_checkFile(); LoggerFile.printf_P( (PGM_P)PSTR("[INFO %s] " _1 "\n"), Logger_stamp().c_str() ,##__VA_ARGS__);}
#define Logger_warning(_1, ...)   { Logger_checkFile(); LoggerFile.printf_P( (PGM_P)PSTR("[WARNING %s] " _1 "\n"), Logger_stamp().c_str() ,##__VA_ARGS__);}
#define Logger_error(_1, ...)     { Logger_checkFile(); LoggerFile.printf_P( (PGM_P)PSTR("[ERROR %s] " _1 "\n"), Logger_stamp().c_str() ,##__VA_ARGS__);}
#define Logger_critical(_1, ...)  { Logger_checkFile(); LoggerFile.printf_P( (PGM_P)PSTR("[CRITICAL %s] " _1 "\n"), Logger_stamp().c_str() ,##__VA_ARGS__);}
#else
#define Logger_debug(...) { }  // leaves 40,740 K, so flash debug uses 1.1K of ram... 
#define Logger_Debug_raw(_1, ...) { }
#define Logger_log(_1, ...) { }
#define Logger_Debug(_1, ...) { }
#define Logger_setup(_1, ...) { }
#define Logger_web(_1, ...) { }
#define Logger_wifi(_1, ...) { }
#define Logger_info(_1, ...) { }
#define Logger_warning(_1, ...) { }
#define Logger_error(_1, ...) { }
#define Logger_critical(_1, ...) { }
#endif
/*
   void fatal   (const char *format, va_list logVariables);
  void error   (const char *format, va_list logVariables);
  void warning (const char *format, va_list logVariables);
  void notice  (const char *format, va_list logVariables);
  void trace   (const char *format, va_list logVariables);
  void verbose (const char *format, va_list logVariables);

  %[flags][width][.precision][length]specifier

**** Wildcards

  %s  replace with an string (char*)
  %c  replace with an character
  %d  replace with an integer value
  %l  replace with an long value
  %x  replace and convert integer value into hex
  %X  like %x but combine with 0x123AB
  %b  replace and convert integer value into binary
  %B  like %x but combine with 0b10100011
  %t  replace and convert boolean value into "t" or "f"
  %T  like %t but convert into "true" or "false"



  where the format string can be used to format the log variables

  %s  display as string (char*)
  %S  display as string from flash memory (__FlashStringHelper* or char[] PROGMEM)
  %c  display as single character
  %d, %i  display as integer value
  %l  display as long value
  %x  display as hexadecimal value
  %X  display as hexadecimal value prefixed by `0x`
  %b  display as  binary number
  %B  display as  binary number, prefixed by `0b'
  %t  display as boolean value "t" or "f"
  %T  display as boolean value "true" or "false"
  %D,%F display as double value
  %n  nothing printed

  The sprintf function will write the formatting string and the variables into the "data" character array.
   You provide a formatting string template, that contains placeholders for variables that you plan to insert.
   These placeholders have a percentage sign (%) prefix.  Eg.  %s, %d, %f etc.
   The number of placeholders must match the number of variables.
   The variables are inserted at the placeholder position, and formatted based on the type of placeholder used.
   %d, %i = signed integer               %f = floating point number
   %s = string                     %.1f = float to 1 decimal place
   %c = character                  %.3f = float to 3 decimal places
   %e = scientific notation          %g = shortest representation of %e or %f
   %u = unsigned integer             %o = unsigned octal
   %x = unsigned hex (lowercase)     %X = unsigned hex (uppercase)
   %hd = short int                  %ld = long int
   %lld = long long int
   =============================================================================  */

String Logger_stamp() {
#ifdef Debug_XCTRL
  char timestamp[50];
  tmElements_t tmstruct;
  breakTime(now(), tmstruct);
  if ((tmstruct.Year + 1970) >= 2018) {

    sprintf_P(timestamp, PSTR("%4u-%02d-%02dT%02d:%02d:%02dM%lu"), (tmstruct.Year + 1970), tmstruct.Month, tmstruct.Day,
              tmstruct.Hour, tmstruct.Minute, tmstruct.Second, millis());

  } else {
    sprintf_P(timestamp, PSTR("N/A %lu"), millis());
  }
  return timestamp;
#else
  return F("N/A ");
#endif
}


bool Logger_close() {
  ///////////////////////////////////////////////////////////////////////
#ifdef Debug_XCTRL
  if (!LoggerFile)
    return false;
  LoggerFile.flush();
  LoggerFile.close();
  return true;
#endif
  return false;
}

bool Logger_init() {
#ifdef Debug_XCTRL
  //LoggerFile = LittleFS.open(LOGGER_FILE_NAME, "a+");
  //Logger_checkFile();
  //return true;

  if (!LoggerFile)
    LoggerFile = LittleFS.open(F(LOGGER_FILE_NAME), "a+");
  if (!LoggerFile) {
    //Logger_error("Could not open LittleFS log file!");
    return false;
  }
  //Logger_checkFile();
  //Logger_info("Log file successfully opened");
  return true;
#endif
  return false;
}

bool Logger_checkFile() {
#ifdef Debug_XCTRL
  if (!LoggerFile)
    Logger_init();
  if (!LoggerFile)
    return false;
  if (LoggerFile.size() > LOGGER_FILE_MAXSIZE) {
    Logger_close();
    if (LittleFS.exists(F(LOGGER_FILE_NAME_OLD))) {
      LittleFS.remove(F(LOGGER_FILE_NAME_OLD));
    }
    LittleFS.rename(F(LOGGER_FILE_NAME), F(LOGGER_FILE_NAME_OLD));
    Logger_init();
    if (!LoggerFile) {
      // println("ERROR Could not open LittleFS log file!");
      return false;
    } else
      LoggerFile.println(F("---=== New Log File ===---  [old log file: debug_old.txt]"));
  }
  return true;
#else
  return false;
#endif
}

/*

   typedef enum {
  DEBUG,
  INFO,
  WARNING,
  ERROR,
  CRITICAL,
  SETUP,
  WIFI,
  WEB
  } LogSeverity;


  String Logger_buildHeader(LogSeverity severity) {
  #ifdef Debug_XCTRL
  String retval;
  switch (severity) {
    case DEBUG:
      retval = F("[DEBUG ");
      break;
    case INFO:
      retval = F("[INFO ");
      break;
    case WARNING:
      retval = F("[WARNING ");
      break;
    case ERROR:
      retval = F("[ERROR ");
      break;
    case CRITICAL:
      retval = F("[CRITICAL ");
      break;
    case SETUP:
      retval = F("[SETUP ");
      break;
    case WIFI:
      retval = F("[WiFi ");
      break;
    case WEB:
      retval = F("[WEB ");
      break;
    default:
      retval = F("[UNKNOWN ");
      break;
  }
  retval += getTimestampAsString() + String(millis()) + "] ";
  return retval;
  #else
  return F("[N/A] ");
  #endif
  }

  void Logger_printLn(LogSeverity severity, const char *fmt) {
  #ifdef Debug_XCTRL
  if (!Logger_checkFile())
    return;
  LoggerFile.print(Logger_buildHeader(severity));
  LoggerFile.println(fmt);
  #endif
  }
*/

/*void Logger_log(LogSeverity severity, const __FlashStringHelper *message) {
  Logger_printLn(severity, String(message).c_str());
  }
  void Logger_log(LogSeverity severity, const String &message) {
  Logger_printLn(severity, message.c_str());
  }
  void Logger_log(LogSeverity severity, const char *message) {
  Logger_printLn(severity, message);
  }
*/

/*
  void Logger_printLn(LogSeverity severity,  const char *fmt, ...) {
  void Logger_printLn(LogSeverity severity, const String &fmt, ...) {
  void Logger_printLn(LogSeverity severity, const __FlashStringHelper *fmt, ...) {


  void Logger_printf(LogSeverity severity, const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  char temp[64];
  char* buffer = temp;
  size_t len = vsnprintf(temp, sizeof(temp), format, arg);
  va_end(arg);
  //String logMsg;
  if (len > sizeof(temp) - 1) {
    buffer = new char[len + 1];
    if (!buffer) {
      return;// *this;
    }
    va_start(arg, format);
    vsnprintf(buffer, len + 1, format, arg);
    va_end(arg);
  }
  //logMsg = String(buffer);
  //  Logger_printLn(severity, buffer // logMsg);
  if (buffer != temp) {
    delete[] buffer;
  }
  }
*/
/*
  void Logger_wifi(const String &msg) {  Logger_printLn(WIFI, msg.c_str());  }
  void Logger_setup(const String &msg) {  Logger_printLn(SETUP, msg.c_str());  }
  void Logger_debug(const String &msg) {  Logger_printLn(DEBUG, msg.c_str());  }
  void Logger_info(const String &msg) {  Logger_printLn(INFO, msg.c_str());  }
  void Logger_warning(const String &msg) {  Logger_printLn(WARNING, msg.c_str());  }
  void Logger_error(const String &msg) {  Logger_printLn(ERROR, msg.c_str());  }
  void Logger_critical(const String &msg) {  Logger_printLn(CRITICAL, msg.c_str());  }
*/
//String Logger_getLogs() {
//  return Logger_getLogs(false);
//}

/*String Logger_getLogs(bool asHTML) {
  String retval;
  if (!LoggerFile || !LoggerFile.size()) {
    return retval;
  }
  LoggerFile.seek(0, SeekSet);
  while (LoggerFile.available()) {
    retval += (char)LoggerFile.read();
  }
  LoggerFile.seek(0, SeekEnd);
  return retval;
  }
*/

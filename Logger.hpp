#ifndef LOGGER_HPP_
#define LOGGER_HPP_

#include <Arduino.h>
#include <ChRt.h>
#include "CONFIG.h"


class Logger {
public:
    enum LogLevel {
        Debug = 0, Info = 1, Warn = 2, Error = 3, Off = 4, Cons = 5
    };
    Logger();
    void debug(const char *, ...);
    void info(const char *, ...);
    void warn(const char *, ...);
    void error(const char *, ...);
    void console(const char *, ...);
    void setLoglevel(LogLevel);
    LogLevel getLogLevel();
    uint32_t getLastLogTime();
    boolean isDebug();
private:
    LogLevel logLevel;
    uint32_t lastLogTime;
    MUTEX_DECL(serial_MTX);
    //mutex_t serial_MTX;
    void log(LogLevel, const char *format, va_list);
    void logMessage(const char *format, va_list args);
};

//export the logger
extern Logger log_inst;

#define LOG_DEBUG log_inst.debug
#define LOG_INFO log_inst.info
#define LOG_WARN log_inst.warn
#define LOG_ERROR log_inst.error
#define LOG_CONSOLE log_inst.console
//#define LOG_CONSOLE SERIALCONSOLE.print

#endif /* LOG_HPP_ */

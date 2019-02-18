#ifndef LOG_HPP_
#define LOG_HPP_

#include <Arduino.h>
#include <ChRt.h>
#include "CONFIG.h"


class Logger2 {
public:
    enum LogLevel {
        Debug = 0, Info = 1, Warn = 2, Error = 3, Off = 4, Cons = 5
    };
    //Logger2(LogLevel);
    Logger2();
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

#endif /* LOG_HPP_ */

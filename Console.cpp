#include "Console.h"
#include "CONFIG.h"
#include <stdio.h>
#include <stdarg.h>

void Console::task(){// This is a Task.

  Console::printMenu();
  Logger::console(">> ");
  for (;;)
  {
    //wakeTime += MS2ST(50);
    //chThdSleepUntil(wakeTime);
    chThdSleepMilliseconds(35); // + 15 of the console timeout
    //Logger::console("aaa\n");
    doConsole();
  }
}

void Console::doConsole(){
  static unsigned char y[32] = {0};
  static unsigned char yptr = 0;
  static unsigned char numB = 0;
  if (SERIALCONSOLE.available()) { 
    numB = SERIALCONSOLE.readBytesUntil('\n', &y[yptr], 31 - yptr);
    Logger::console("%s",&y[yptr]);
    yptr += numB;
    if ((y[yptr-1] == '\n') || (y[yptr-1] == '\r')){
      switch (y[0]){
        case '1':
          Logger::console("Enable debug mode to see stack usage for all tasks\n");
          break;
          
        case '2':
          Logger::console("Option %s\n",y);
          break;
          
        case 'v':
          if (y[1] >= 0x30 && y[1] <= 0x35){
            Logger::setLoglevel((Logger::LogLevel)(y[1] - 0x30));
            Logger::debug("logLevel set to :%d\n", Logger::getLogLevel());
            Logger::info("logLevel set to :%d\n", Logger::getLogLevel());
            Logger::warn("logLevel set to :%d\n", Logger::getLogLevel());
            Logger::error("logLevel set to :%d\n", Logger::getLogLevel());
            Logger::console("logLevel set to :%d\n", Logger::getLogLevel());
          } else {
            Logger::console("logLevel out of bounds (0-5)\n");
          }
          break;
          
        case 'h':
        case '?':
          Console::printMenu();
          break;
          
        case '\n':
        case '\r':
        default:
          break;   
      }
    } else if (yptr <30){
      return;
    }

    //at this point prepare for a new command
    for (int i = 0; i < 32; i++){
      y[i] = 0;
    }
    yptr = 0;
    Logger::console(">> ");
  }   
}

void Console::printMenu() {   
    Logger::console("\n*************SYSTEM MENU *****************\n");
    Logger::console("Enable line endings of some sort (LF, CR, CRLF)\n");
    Logger::console("Most commands case sensitive\n\n");
    Logger::console("GENERAL SYSTEM CONFIGURATION\n\n");
    Logger::console("   h or ? = help (displays this message)\n");
    Logger::console("   1 = display stack high watermark for this task\n");
    Logger::console("   vX (X=0:debug, X=1:info, X=2:warn, X=3:error, X=4:console)\n");
    Logger::console("\n");
}

void Console::init(){
  // initialize serial communication at 9600 bits per second:
  SERIALCONSOLE.begin(9600);

  /*
  while (!SERIALCONSOLE) {
    ; // wait for serial port to connect. Needed for native USB, on LEONARDO, MICRO, YUN, and other 32u4 based boards.
  }*/
  
  SERIALCONSOLE.setTimeout(15);
}

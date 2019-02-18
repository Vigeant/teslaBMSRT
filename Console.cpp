#include "Console.h"
#include "CONFIG.h"
#include <stdio.h>
#include <stdarg.h>

void Console::task(){// This is a Task.

  Console::printMenu();
  LOG_CONSOLE(">> ");
  for (;;)
  {
    //wakeTime += MS2ST(50);
    //chThdSleepUntil(wakeTime);
    chThdSleepMilliseconds(35); // + 15 of the console timeout
    //LOG_CONSOLE("aaa\n");
    doConsole();
  }
}

void Console::doConsole(){
  static unsigned char y[32] = {0};
  static unsigned char yptr = 0;
  static unsigned char numB = 0;
  if (SERIALCONSOLE.available()) { 
    numB = SERIALCONSOLE.readBytesUntil('\n', &y[yptr], 31 - yptr);
    LOG_CONSOLE("%s",&y[yptr]);
    yptr += numB;
    if ((y[yptr-1] == '\n') || (y[yptr-1] == '\r')){
      switch (y[0]){
        case '1':
          LOG_CONSOLE("Enable debug mode to see stack usage for all tasks\n");
          break;
          
        case '2':
          LOG_CONSOLE("Option %s\n",y);
          break;
          
        case 'v':
          if (y[1] >= 0x30 && y[1] <= 0x35){
            log_inst.setLoglevel((Logger2::LogLevel)(y[1] - 0x30));
            LOG_DEBUG("logLevel set to :%d\n", log_inst.getLogLevel());
            LOG_INFO("logLevel set to :%d\n", log_inst.getLogLevel());
            LOG_WARN("logLevel set to :%d\n", log_inst.getLogLevel());
            LOG_ERROR("logLevel set to :%d\n", log_inst.getLogLevel());
            LOG_CONSOLE("logLevel set to :%d\n", log_inst.getLogLevel());
          } else {
            LOG_CONSOLE("logLevel out of bounds (0-5)\n");
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
    LOG_CONSOLE(">> ");
  }   
}

void Console::printMenu() { 
 
    LOG_CONSOLE("\n*************SYSTEM MENU *****************\n");
    LOG_CONSOLE("Enable line endings of some sort (LF, CR, CRLF)\n");
    LOG_CONSOLE("Most commands case sensitive\n\n");
    LOG_CONSOLE("GENERAL SYSTEM CONFIGURATION\n\n");
    LOG_CONSOLE("   h or ? = help (displays this message)\n");
    LOG_CONSOLE("   1 = display stack high watermark for this task\n");
    LOG_CONSOLE("   vX (X=0:debug, X=1:info, X=2:warn, X=3:error, X=4:console)\n");
    LOG_CONSOLE("\n");
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

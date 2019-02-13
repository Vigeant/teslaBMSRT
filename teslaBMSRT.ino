#include <ChRt.h>

#include "Console.h"
#include "Logger.h"
#include "Oled.h"

// define Tasks
// priority 1 = low, 100 = high

//------------------------------------------------------------------------------
// ConsoleTask
//
// Runs the console UI mainly for debugging and later for config
// Period = 50 ms
static unsigned int consoleTaskPriority = 25;
//------------------------------------------------------------------------------
static THD_WORKING_AREA(waConsoleTask, 256);
static THD_FUNCTION(ConsoleTask, arg) {
  (void)arg;
  Console::task();
}

//------------------------------------------------------------------------------
// OledTask
//
// Runs the Oled task that simply prints values on the OLED for prod
// Period = 100 ms
static unsigned int OledTaskPriority = 10;
//------------------------------------------------------------------------------
static THD_WORKING_AREA(waOledTask, 256);
static THD_FUNCTION(OledTask, arg) {
  (void)arg;
  
  systime_t wakeTime = chVTGetSystemTime();
  for (;;) // A Task shall never return or exit.
  {
    wakeTime += MS2ST(100);
    chThdSleepUntil(wakeTime);
    MyOled::task();
  }
}

//------------------------------------------------------------------------------
// DebugTask
//
// Runs the debug task that simply prints the unused stack space of each task
// Period = 1000 ms
static unsigned int DebugTaskPriority = 3;
//------------------------------------------------------------------------------
static THD_WORKING_AREA(waDebugTask, 256);
static THD_FUNCTION(DebugTask, arg) {
  (void)arg;
  
  systime_t wakeTime = chVTGetSystemTime();
  // digital pin 2 has a pushbutton attached to it. Give it a name:
  //uint8_t pushButton = 2;
  // make the pushbutton's pin an input:
  //pinMode(pushButton, INPUT);

  for (;;) // A Task shall never return or exit.
  {
    // Sleep for one second.
    wakeTime += MS2ST(1000);
    chThdSleepUntil(wakeTime);

    Logger::debug("DebugTask   | unsused stack | %d\n", chUnusedThreadStack(waDebugTask, sizeof(waDebugTask)));
    Logger::debug("OledTask    | unsused stack | %d\n", chUnusedThreadStack(waOledTask, sizeof(waOledTask)));
    Logger::debug("ConsoleTask | unsused stack | %d\n", chUnusedThreadStack(waConsoleTask, sizeof(waConsoleTask)));
    Logger::debug("==================================\n", 1);
  }
}






//------------------------------------------------------------------------------
// 
void chSetup() {

  chThdCreateStatic(waDebugTask, sizeof(waDebugTask),
                          NORMALPRIO + DebugTaskPriority, DebugTask, NULL);
                          
  chThdCreateStatic(waOledTask, sizeof(waOledTask),
                          NORMALPRIO + OledTaskPriority, OledTask, NULL);

  chThdCreateStatic(waConsoleTask, sizeof(waConsoleTask),
                          NORMALPRIO + consoleTaskPriority, ConsoleTask, NULL); 
}

// the setup function runs once when you press reset or power the board
void setup() {
  Console::init();
  MyOled::init();
  chBegin(chSetup);
  // chBegin() resets stacks and should never return.
  while (true) {}
}

void loop()
{
  // Empty. Things are done in Tasks.
}

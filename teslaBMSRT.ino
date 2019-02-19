#include <ChRt.h>

#include "Cons.hpp"
#include "Logger.hpp"
#include "Oled.hpp"
#include "Controller.hpp"

// define Tasks
// priority 1 = low, 100 = high

//------------------------------------------------------------------------------
// ConsoleTask
//
// Runs the console UI mainly for debugging and later for config
// Period = 50 ms
//------------------------------------------------------------------------------
static unsigned int consoleTaskPriority = 25;
//instantiate the console
static Cons cons_inst;
static THD_WORKING_AREA(waConsoleTask, 2048);
static THD_FUNCTION(ConsoleTask, arg) {
  (void)arg;
  cons_inst.printMenu();
  LOG_CONSOLE(">> ");
  for (;;)
  {
    //wakeTime += MS2ST(50);
    //chThdSleepUntil(wakeTime);
    chThdSleepMilliseconds(35); // + 15 of the Cons timeout
    //LOG_CONSOLE("aaa\n");
    cons_inst.doConsole();
  }
}

//------------------------------------------------------------------------------
// ControllerTask
//
// Runs the BMS state machine controlling all usefull activities
// Period = 1000 ms
static unsigned int ControllerTaskPriority = 5;
//------------------------------------------------------------------------------
static Controller controller_inst;
static THD_WORKING_AREA(waControllerTask, 2048);
static THD_FUNCTION(ControllerTask, arg) {
  (void)arg;
  
  systime_t wakeTime = chVTGetSystemTime();
  for (;;) // A Task shall never return or exit.
  {
    wakeTime += MS2ST(1000);
    chThdSleepUntil(wakeTime);
    controller_inst.doController();
  }
}

//------------------------------------------------------------------------------
// OledTask
//
// Runs the Oled task that simply prints values on the OLED for prod
// Period = 900 ms
static unsigned int OledTaskPriority = 10;
//------------------------------------------------------------------------------
static Oled oled_inst(&controller_inst);
static THD_WORKING_AREA(waOledTask, 2048);
static THD_FUNCTION(OledTask, arg) {
  (void)arg;
  
  systime_t wakeTime = chVTGetSystemTime();
  for (;;) // A Task shall never return or exit.
  {
    wakeTime += MS2ST(900);
    chThdSleepUntil(wakeTime);
    oled_inst.doOled();
  }
}

//------------------------------------------------------------------------------
// DebugTask
//
// Runs the debug task that simply prints the unused stack space of each task
// Period = 2000 ms
static unsigned int DebugTaskPriority = 3;
//------------------------------------------------------------------------------
static THD_WORKING_AREA(waDebugTask, 2048);
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
    wakeTime += MS2ST(2000);
    chThdSleepUntil(wakeTime);

    LOG_DEBUG("DebugTask   | unsused stack | %d\n", chUnusedThreadStack(waDebugTask, sizeof(waDebugTask)));
    LOG_DEBUG("OledTask    | unsused stack | %d\n", chUnusedThreadStack(waOledTask, sizeof(waOledTask)));
    LOG_DEBUG("ControllerTa| unsused stack | %d\n", chUnusedThreadStack(waControllerTask, sizeof(waControllerTask)));
    LOG_DEBUG("ConsoleTask | unsused stack | %d\n", chUnusedThreadStack(waConsoleTask, sizeof(waConsoleTask)));
    LOG_DEBUG("==================================\n", 1);
  }
}






//------------------------------------------------------------------------------
// 
void chSetup() {

  chThdCreateStatic(waDebugTask, sizeof(waDebugTask),
                          NORMALPRIO + DebugTaskPriority, DebugTask, NULL);
                         
  chThdCreateStatic(waOledTask, sizeof(waOledTask),
                          NORMALPRIO + OledTaskPriority, OledTask, NULL);

  chThdCreateStatic(waControllerTask, sizeof(waControllerTask),
                          NORMALPRIO + ControllerTaskPriority, ControllerTask, NULL);
                         
  chThdCreateStatic(waConsoleTask, sizeof(waConsoleTask),
                          NORMALPRIO + consoleTaskPriority, ConsoleTask, NULL); 
}

// the setup function runs once when you press reset or power the board
void setup() {
  //Controller::init();
  
  chBegin(chSetup);
  // chBegin() resets stacks and should never return.
  while (true) {}
}

void loop()
{
  // Empty. Things are done in Tasks.
}

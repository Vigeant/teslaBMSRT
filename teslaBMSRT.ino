#include <ChRt.h>
#include "Cons.hpp"
#include "Logger.hpp"
#include "Oled.hpp"
#include "Controller.hpp"


//instantiate the console
static Controller controller_inst;
static Cons cons_inst;
static Oled oled_inst(&controller_inst);


// a softer sleep_until function that will survive missed deadlines
uint8_t sleepUntil(systime_t *previous, const systime_t period)
{
  systime_t future = *previous + MS2ST(period);
  chSysLock();
  systime_t now = chVTGetSystemTime();
  int mustDelay = now < *previous ?
    (now < future && future < *previous) :
    (now < future || future < *previous);
  if (mustDelay)
    chThdSleepS(future - now);
  chSysUnlock();
  *previous = future;
  return mustDelay;
}


// define Tasks
// priority 1 = low, 100 = high

//------------------------------------------------------------------------------
// ConsoleTask
//
// Runs the console UI mainly for debugging and later for config
// Period = 100 ms
//------------------------------------------------------------------------------
static unsigned int consoleTaskPriority = 50;
static THD_WORKING_AREA(waConsoleTask, 2048);
static THD_FUNCTION(ConsoleTask, arg) {
  (void)arg;
  cons_inst.printMenu();
  LOG_CONSOLE(">> ");
  systime_t time = chVTGetSystemTime();
  for (;;)
  {
    if (!sleepUntil(&time, 100)) {LOG_ERROR("ConsoleTask missed a deadline\n" );}
    cons_inst.doConsole();
  }
}

//------------------------------------------------------------------------------
// ControllerTask
//
// Runs the BMS state machine controlling all usefull activities
// Period = 4000 ms
static unsigned int ControllerTaskPriority = 40;
//------------------------------------------------------------------------------
static THD_WORKING_AREA(waControllerTask, 2048);
static THD_FUNCTION(ControllerTask, arg) {
  (void)arg;
  
  systime_t time = chVTGetSystemTime();
  for (;;)
  {
    if (!sleepUntil(&time, 4000)) {LOG_ERROR("ControllerTask missed a deadline\n");}
    controller_inst.doController();
  }
}

//------------------------------------------------------------------------------
// OledTask
//
// Runs the Oled task that simply prints values on the OLED for prod
// Period = 4000 ms
static unsigned int OledTaskPriority = 30;
//------------------------------------------------------------------------------
static THD_WORKING_AREA(waOledTask, 2048);
static THD_FUNCTION(OledTask, arg) {
  (void)arg;
  
  systime_t time = chVTGetSystemTime();
  for (;;)
  {
    if (!sleepUntil(&time, 4000)) {LOG_ERROR("OledTask missed a deadline\n");}
    oled_inst.doOled();
  }
}

//------------------------------------------------------------------------------
// DebugTask
//
// Runs the debug task that simply prints the unused stack space of each task
// Period = 10000 ms
static unsigned int DebugTaskPriority = 20;
//------------------------------------------------------------------------------
static THD_WORKING_AREA(waDebugTask, 2048);
static THD_FUNCTION(DebugTask, arg) {
  (void)arg;
  
  systime_t time = chVTGetSystemTime();
  for (;;)
  {
    if (!sleepUntil(&time, 10000)) {LOG_ERROR("OledTask missed a deadline\n");}

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

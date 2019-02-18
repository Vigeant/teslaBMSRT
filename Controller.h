#include <Arduino.h>
#include <ChRt.h>
#include "CONFIG.h"

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

class Controller {
public:
  enum ControllerState {
      IDLE = 0, CHARGING = 1, DRIVE = 2
  };  
  static void task();
  static void init();
  static ControllerState getState();
  
private:

  //initialization functions
  static void initBMSManager(); //reset all boards and assign address to each board

  //run-time functions
  static void gatherModuleData(); //gathers all the data from the boards and populates the BMSModel
  static void balanceCells(); //balances the cells according to thresholds in the CONFIG.h file
   

  static ControllerState state; 
  static void idle();
  static void charging();
  static void drive();

};

#endif /* CONTROLLER_H_ */

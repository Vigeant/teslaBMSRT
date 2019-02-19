#include <Arduino.h>
#include <ChRt.h>
#include "CONFIG.h"

#ifndef CONTROLLER_HPP_
#define CONTROLLER_HPP_

class Controller {
public:
  enum ControllerState {
      IDLE = 0, CHARGING = 1, DRIVE = 2
  };  
  void doController();
  Controller();
  ControllerState getState();
  
private:

  //initialization functions
  void initBMSManager(); //reset all boards and assign address to each board

  //run-time functions
  void gatherModuleData(); //gathers all the data from the boards and populates the BMSModel
  void balanceCells(); //balances the cells according to thresholds in the CONFIG.h file
   

  ControllerState state; 
  void idle();
  void charging();
  void drive();

};

#endif /* CONTROLLER_H_ */

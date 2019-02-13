#include <Arduino.h>
#include <ChRt.h>
#include "CONFIG.h"
#include "Logger.h"

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

  static ControllerState state; 
  static void idle();
  static void charging();
  static void drive();

};

#endif /* CONTROLLER_H_ */

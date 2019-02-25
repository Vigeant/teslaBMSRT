#include <Arduino.h>
#include <ChRt.h>
#include "CONFIG.h"
#include "BMSModuleManager.hpp"

#ifndef CONTROLLER_HPP_
#define CONTROLLER_HPP_

class Controller {
  public:
    enum ControllerState {
      INIT = 0, IDLE = 1, CHARGING = 2, RUN = 3
    };
    void doController();
    Controller();
    ControllerState getState();
    BMSModuleManager* getBMSPtr();

  private:

    BMSModuleManager bms;

    //initialization functions
    void initBMSManager(); //reset all boards and assign address to each board

    //run-time functions
    void syncModuleDataObjects(); //gathers all the data from the boards and populates the BMSModel object instances
    void balanceCells(); //balances the cells according to thresholds in the BMSModuleManager

    void assertFaultLine();
    void clearFaultLine();
    void cycleCharger();

    ControllerState state;
    void idle();
    void charging();
    void run();

};

#endif /* CONTROLLER_H_ */

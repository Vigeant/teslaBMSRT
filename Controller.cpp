#include "Controller.hpp"

//Controller::ControllerState Controller::state;

//------------------------------------------------------------------------------
// Controller::task()
//
// Orchestrates the activities within the BMS via a state machine.
//------------------------------------------------------------------------------
void Controller::doController(){
  const int stateticks = 1;
  static int ticks = 0;
  Controller::gatherModuleData();
  switch (state){
    case IDLE:
      Controller::idle();
      if (ticks >= stateticks){
        ticks = 0;
        state = CHARGING;
      }
      break;
    case CHARGING:
      Controller::charging();
      if (ticks >= stateticks){
        ticks = 0;
        state = DRIVE;
      }
      break;
    case DRIVE:
      Controller::drive();
      if (ticks >= stateticks){
        ticks = 0;
        state = IDLE;
      }
      break;
    default:
      break;
  }
  ticks++;
}


Controller::Controller(){
  Controller::initBMSManager();
  
  state = IDLE;
}

//initialization functions
//reset all boards and assign address to each board and configure their thresholds
void Controller::initBMSManager(){
  
}

//run-time functions
//gathers all the data from the boards and populates the BMSModel
void Controller::gatherModuleData(){
  
}
//balances the cells according to thresholds in the CONFIG.h file
void Controller::balanceCells(){
  
}



void Controller::idle(){
  
}

void Controller::charging(){
  
}

void Controller::drive(){
  
}

Controller::ControllerState Controller::getState(){
  return state;
}

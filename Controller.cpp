#include "Controller.hpp"

//Controller::ControllerState Controller::state;

//------------------------------------------------------------------------------
// Controller::task()
//
// Orchestrates the activities within the BMS via a state machine.
//------------------------------------------------------------------------------
void Controller::doController() {
  const int stateticks = 1;
  static int ticks = 0;
  switch (state) {
    case INIT:
      initBMSManager();
      state = IDLE;
      break;
    case IDLE:
      idle();
      if (ticks >= stateticks) {
        ticks = 0;
        state = CHARGING;
      }
      break;
    case CHARGING:
      charging();
      if (ticks >= stateticks) {
        ticks = 0;
        state = DRIVE;
      }
      break;
    case DRIVE:
      drive();
      if (ticks >= stateticks) {
        ticks = 0;
        state = IDLE;
      }
      break;
    default:
      break;
  }
  ticks++;
}


Controller::Controller() {
  state = INIT;
}

//initialization functions
//reset all boards and assign address to each board and configure their thresholds
void Controller::initBMSManager() {
  bms.renumberBoardIDs();

}

//run-time functions
//gathers all the data from the boards and populates the BMSModel
void Controller::syncModuleDataObjects() {
  bms.getAllVoltTemp();

}
//balances the cells according to thresholds in the CONFIG.h file
void Controller::balanceCells() {

}



void Controller::idle() {
  syncModuleDataObjects();

}

void Controller::charging() {
  syncModuleDataObjects();

}

void Controller::drive() {
  syncModuleDataObjects();

}

Controller::ControllerState Controller::getState() {
  return state;
}

BMSModuleManager* Controller::getBMSPtr() {
  return &bms;
}

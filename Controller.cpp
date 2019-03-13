#include "Controller.hpp"

//Controller::ControllerState Controller::state;

//------------------------------------------------------------------------------
// doController
//
// Orchestrates the activities within the BMS via a state machine.
//------------------------------------------------------------------------------
void Controller::doController() {
  const int stateticks = 1;
  static int ticks = 0;
  switch (state) {
	  
    case INIT:
      initBMSManager();
      state = STANDBY;
      break;
	  
	  
    case STANDBY:
      standby();
      if (ticks >= stateticks) {
        ticks = 0;
        state = CHARGING;
      }
      break;
	  
	  
    case CHARGING:
      charging();
      if (ticks >= stateticks) {
        ticks = 0;
        state = RUN;
      }
      break;
	  
	  
    case RUN:
      run();
      if (ticks >= stateticks) {
        ticks = 0;
        state = STANDBY;
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
  bms.clearFaults();
}

void Controller::assertFaultLine() {
  //TODO assert fault line
}

void Controller::clearFaultLine() {
  //TODO assert fault line
}

void Controller::cycleCharger() {
  //TODO cycle charger
}

//run-time functions
//gathers all the data from the boards and populates the BMSModel
void Controller::syncModuleDataObjects() {
  bool nofault = true;
  bms.getAllVoltTemp();
  if ( bms.getHighCellVolt() > OVER_V_SETPOINT ) {
    LOG_ERROR("OVER_V_SETPOINT: %fV, highest cell:%fV\n", OVER_V_SETPOINT, bms.getHighCellVolt());
    nofault = false;
  }
  if ( bms.getLowCellVolt() < UNDER_V_SETPOINT ) {
    LOG_ERROR("UNDER_V_SETPOINT: %fV, lowest cell:%fV\n", UNDER_V_SETPOINT, bms.getLowCellVolt());
    nofault = false;
  }
  if ( bms.getHighTemperature() > OVER_T_SETPOINT ) {
    LOG_ERROR("OVER_T_SETPOINT: %fV, highest module:%fV\n", UNDER_V_SETPOINT, bms.getHighTemperature());
    nofault = false;
  }
  if ( bms.getLowTemperature() < UNDER_T_SETPOINT ) {
    LOG_ERROR("UNDER_T_SETPOINT: %fV, lowest module:%fV\n", UNDER_T_SETPOINT, bms.getLowTemperature());
    nofault = false;
  }

  if (nofault) {
	clearFaultLine();
    bms.clearFaults();
  } else {
	assertFaultLine();
    
  }

}
//balances the cells according to thresholds in the CONFIG.h file
void Controller::balanceCells() {
  //balance for 1 second given that the controller wakes up every second.
  //TODO balancing disabled here to avoid discharging the batteries.
  //void balanceCells(4);
}


/*
   standby state is when the boat is not charging and not driving.
   The boat may be connected to a charger but it has reached full charge.
   In that state, it monitors for a voltage change to switch to a new state.
*/
void Controller::standby() {
  const int ticksCount = 10;
  static int ticks = 0;
  syncModuleDataObjects();

  //detect if charging
  if (ticks > ticksCount){
    
  }
  //detect if driving
  //detect if standby


}

void Controller::charging() {
  syncModuleDataObjects();

}

void Controller::run() {
  syncModuleDataObjects();

}

Controller::ControllerState Controller::getState() {
  return state;
}

BMSModuleManager* Controller::getBMSPtr() {
  return &bms;
}

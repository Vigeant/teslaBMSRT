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

  //figure out state transition
  switch (state) {

    case INIT:
      if (ticks >= 1) {
        ticks = 0;
        state = STANDBY;
      }
      break;

    case STANDBY:
      if (ticks >= stateticks) {
        ticks = 0;
        state = STANDBY_DC2DC;
      }
      break;

    case STANDBY_DC2DC:
      if (ticks >= stateticks) {
        ticks = 0;
        state = CHARGING;
      }
      break;
      
    case CHARGING:
      if (ticks >= stateticks) {
        ticks = 0;
        state = CHARGER_CYCLE;
      }
      break;

    case CHARGER_CYCLE:
      if (ticks >= stateticks) {
        ticks = 0;
        state = RUN;
      }
      break;
      
    case RUN:
      if (ticks >= stateticks) {
        ticks = 0;
        state = STANDBY;
      }
      break;


    default:
      break;
  }

  //execute state
  switch (state) {

    case INIT:
      init();
      break;

    case STANDBY:
      standby();
      break;

    case STANDBY_DC2DC:
      standbyDC2DC();
      break;
      
    case CHARGING:
      charging();
      break;

    case CHARGER_CYCLE:
      cargerCycle();
      break;
      
    case RUN:
      run();
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
void Controller::init() {
  pinMode(OUTL_12V_BAT_CHRG, OUTPUT);
  pinMode(OUTPWM_PUMP, OUTPUT); //PWM use analogWrite(OUTPWM_PUMP, 0-255);
  pinMode(INL_BAT_MON_FAULT, INPUT_PULLUP);
  pinMode(INL_EVSE_DISC, INPUT_PULLUP);
  pinMode(INH_RUN, INPUT_PULLDOWN);
  pinMode(INA_12V_BAT, INPUT);  // [0-1023] = analogRead(INA_12V_BAT)
  pinMode(OUTL_EVCC_ON, OUTPUT);
  pinMode(OUTL_NO_FAULT, OUTPUT);

  bms.renumberBoardIDs();
  bms.clearFaults();
}

void Controller::assertFaultLine() {
  //TODO assert fault line
}

void Controller::clearFaultLine() {
  //TODO assert fault line
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
  syncModuleDataObjects();
  digitalWrite(OUTL_EVCC_ON, HIGH);
  digitalWrite(OUTL_NO_FAULT, LOW);
  digitalWrite(OUTL_12V_BAT_CHRG, HIGH);
}

void Controller::standbyDC2DC() {
  syncModuleDataObjects();
  digitalWrite(OUTL_EVCC_ON, HIGH);
  digitalWrite(OUTL_NO_FAULT, LOW);
  digitalWrite(OUTL_12V_BAT_CHRG, LOW);
}

void Controller::charging() {
  syncModuleDataObjects();
  digitalWrite(OUTL_EVCC_ON, LOW);
  digitalWrite(OUTL_NO_FAULT, LOW);
  digitalWrite(OUTL_12V_BAT_CHRG, LOW);
}

void Controller::cargerCycle() {
  syncModuleDataObjects();
  digitalWrite(OUTL_EVCC_ON, HIGH);
  digitalWrite(OUTL_NO_FAULT, LOW);
  digitalWrite(OUTL_12V_BAT_CHRG, LOW);
}

void Controller::run() {
  syncModuleDataObjects();
  digitalWrite(OUTL_EVCC_ON, HIGH);
  digitalWrite(OUTL_NO_FAULT, LOW);
  digitalWrite(OUTL_12V_BAT_CHRG, LOW);

}

Controller::ControllerState Controller::getState() {
  return state;
}

BMSModuleManager* Controller::getBMSPtr() {
  return &bms;
}

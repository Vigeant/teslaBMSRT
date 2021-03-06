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
  pinMode(INL_BAT_PACK_FAULT, INPUT_PULLUP);
  pinMode(INL_BAT_MON_FAULT, INPUT_PULLUP);
  pinMode(INL_EVSE_DISC, INPUT_PULLUP);
  pinMode(INH_RUN, INPUT_PULLDOWN);
  pinMode(INA_12V_BAT, INPUT);  // [0-1023] = analogRead(INA_12V_BAT)
  pinMode(OUTL_EVCC_ON, OUTPUT);
  pinMode(OUTL_NO_FAULT, OUTPUT);

  //faults
  faultModuleLoop = false;
  faultBatMon = false;
  faultBMSSerialComms = false;
  faultBMSOV = false;
  faultBMSUV = false;
  faultBMSOT = false;
  faultBMSUT = false;
  fault12VBatOV = false;
  fault12VBatUV = false;

  //sticky faults
  sFaultModuleLoop = false;
  sFaultBatMon = false;
  sFaultBMSSerialComms = false;
  sFaultBMSOV = false;
  sFaultBMSUV = false;
  sFaultBMSOT = false;
  sFaultBMSUT = false;
  sFault12VBatOV = false;
  sFault12VBatUV = false;

  isFaulted = false;
  stickyFaulted = false;



  chargerInhibit = false;
  powerLimiter = false;

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
  float bat12vVoltage;
  bms.getAllVoltTemp();

  if (bms.getLineFault()) {
    if (!faultBMSSerialComms) {
      LOG_ERROR("Serial communication with battery modules lost!\n");
    }
    faultBMSSerialComms = true;
  } else {
    if (faultBMSSerialComms) LOG_INFO("Serial communication with battery modules re-established!\n");
    faultBMSSerialComms = false;
  }

  if (digitalRead(INL_BAT_PACK_FAULT) == LOW) {
    if (!faultModuleLoop) {
      LOG_ERROR("One or more BMS modules have asserted the fault loop!\n");
    }
    faultModuleLoop = true;
  } else {
    if (faultModuleLoop) LOG_INFO("All modules have deasserted the fault loop\n");
    faultModuleLoop = false;
  }

  if ( bms.getHighCellVolt() > OVER_V_SETPOINT ) {
    if (!faultBMSOV) {
      LOG_ERROR("OVER_V_SETPOINT: %fV, highest cell:%fV\n", OVER_V_SETPOINT, bms.getHighCellVolt());
    }
    faultBMSOV = true;
  } else {
    if (faultBMSOV) LOG_INFO("All cells are back under OV threshold\n");
    faultBMSOV = false;
  }

  if ( bms.getLowCellVolt() < UNDER_V_SETPOINT ) {
    if (!faultBMSUV) {
      LOG_ERROR("UNDER_V_SETPOINT: %fV, lowest cell:%fV\n", UNDER_V_SETPOINT, bms.getLowCellVolt());
    }
    faultBMSUV = true;
  } else {
    if (faultBMSUV) LOG_INFO("All cells are back over UV threshold\n");
    faultBMSUV = false;
  }

  if ( bms.getHighTemperature() > OVER_T_SETPOINT ) {
    if (!faultBMSOT) {
      LOG_ERROR("OVER_T_SETPOINT: %fV, highest module:%fV\n", UNDER_V_SETPOINT, bms.getHighTemperature());
    }
    faultBMSOT = true;
  } else {
    if (faultBMSOT) LOG_INFO("All modules are back under the OT threshold\n");
    faultBMSOT = false;
  }

  if ( bms.getLowTemperature() < UNDER_T_SETPOINT ) {
    if (!faultBMSUT) {
      LOG_ERROR("UNDER_T_SETPOINT: %fV, lowest module:%fV\n", UNDER_T_SETPOINT, bms.getLowTemperature());
    }
    faultBMSUT = true;
  } else {
    if (faultBMSUT) LOG_INFO("All modules are back over the UT threshold\n");
    faultBMSUT = false;
  }

  bat12vVoltage = (float)analogRead(INA_12V_BAT) / BAT12V_SCALING_DIVISOR ;
  //I think this can trigger a hard fault due to the displaying of the float.
  LOG_DEBUG("before print\n");
  LOG_INFO("bat12vVoltage: %f\n", bat12vVoltage);
  LOG_DEBUG("after print\n");
  //Serial.print("blah blahblah blahblah blahblah blahblah blahblah blahblah blahblah blahblah blahblah blah \n");
  //String* AAA = new String("blah");
  //LOG_INFO("bat12vVoltage: %d\n", analogRead(INA_12V_BAT));

  //trigger hardfault
  //int a = *(int*)0xffffffff;
  //LOG_INFO("bat12vVoltage: 0x%x\n", a);
  
  //int a = 0;
  //LOG_INFO("bat12vVoltage: %d\n", 8/a);

  if ( bat12vVoltage > BAT12V_OVER_V_SETPOINT ) {
    if (!fault12VBatOV) {
      LOG_ERROR("12VBAT_OVER_V_SETPOINT: %fV, V:%fV\n", BAT12V_OVER_V_SETPOINT, bat12vVoltage);
    }
    fault12VBatOV = true;
  } else {
    if (fault12VBatOV) LOG_INFO("12V battery back under the OV threshold\n");
    fault12VBatOV = false;
  }

  if ( bat12vVoltage < BAT12V_UNDER_V_SETPOINT ) {
    if (!fault12VBatUV) {
      LOG_ERROR("12VBAT_UNDER_V_SETPOINT: %fV, V:%fV\n", BAT12V_UNDER_V_SETPOINT, bat12vVoltage);
    }
    fault12VBatUV = true;
  } else {
    if (fault12VBatUV) LOG_INFO("12V battery back over the UV threshold\n");
    fault12VBatUV = false;
  }

  chargerInhibit = faultModuleLoop || faultBatMon || faultBMSSerialComms || faultBMSOV || faultBMSOT;
  powerLimiter = faultModuleLoop || faultBatMon || faultBMSSerialComms || faultBMSUV || faultBMSOT;
  isFaulted =  chargerInhibit || faultBMSUV || faultBMSUT || fault12VBatOV || fault12VBatUV;

  //update stiky faults
  sFaultModuleLoop |= faultModuleLoop;
  sFaultBatMon |= faultBatMon;
  sFaultBMSSerialComms |= faultBMSSerialComms;
  sFaultBMSOV |= faultBMSOV;
  sFaultBMSUV |= faultBMSUV;
  sFaultBMSOT |= faultBMSUV;
  sFaultBMSUT |= faultBMSUT;
  sFault12VBatOV |= fault12VBatOV;
  sFault12VBatUV |= fault12VBatUV;

  stickyFaulted |= isFaulted;
  bms.clearFaults();
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
  digitalWrite(OUTL_NO_FAULT, chargerInhibit);
  digitalWrite(OUTL_12V_BAT_CHRG, HIGH);
}

void Controller::standbyDC2DC() {
  syncModuleDataObjects();
  digitalWrite(OUTL_EVCC_ON, HIGH);
  digitalWrite(OUTL_NO_FAULT, chargerInhibit);
  digitalWrite(OUTL_12V_BAT_CHRG, LOW);
}

void Controller::charging() {
  syncModuleDataObjects();
  digitalWrite(OUTL_EVCC_ON, LOW);
  digitalWrite(OUTL_NO_FAULT, chargerInhibit);
  digitalWrite(OUTL_12V_BAT_CHRG, LOW);
}

void Controller::cargerCycle() {
  syncModuleDataObjects();
  digitalWrite(OUTL_EVCC_ON, HIGH);
  digitalWrite(OUTL_NO_FAULT, chargerInhibit);
  digitalWrite(OUTL_12V_BAT_CHRG, LOW);
}

void Controller::run() {
  syncModuleDataObjects();
  digitalWrite(OUTL_EVCC_ON, HIGH);
  digitalWrite(OUTL_NO_FAULT, powerLimiter);
  digitalWrite(OUTL_12V_BAT_CHRG, LOW);

}

Controller::ControllerState Controller::getState() {
  return state;
}

BMSModuleManager* Controller::getBMSPtr() {
  return &bms;
}

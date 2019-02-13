#include "Controller.h"

Controller::ControllerState Controller::state;

void Controller::task(){
  const int stateticks = 10;
  static int ticks = 0;
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

void Controller::init(){
  state = IDLE;
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

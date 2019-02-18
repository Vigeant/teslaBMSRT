#include <Arduino.h>
#include <ChRt.h>
#include "CONFIG.h"
#include "Controller.h"
#include <TeensyView.h>  // Include the SFE_TeensyView library

class Oled {
public:

  Oled(Controller*);
  void doOled();
  //static void init();

private:
  enum formatState {
      FMT1 = 0, FMT2 = 1, FMT3 = 2
  };
  formatState state;
  Controller* controller_inst_ptr;
  void printFormat1();
  void printFormat2();
  void printFormat3();

  void printCentre(const char*  , int);
  //void printCentre(String  , int);
};

#include <Arduino.h>
#include <ChRt.h>
#include "CONFIG.h"
#include <TeensyView.h>  // Include the SFE_TeensyView library

class MyOled {
public:
  
  static void task();
  static void init();

private:
  enum formatState {
      FMT1 = 0, FMT2 = 1
  };
  static formatState state;
  static void printFormat1();
  static void printFormat2();

};

#include <Arduino.h>
#include <ChRt.h>
#include "CONFIG.h"
#include "Controller.h"
#include <TeensyView.h>  // Include the SFE_TeensyView library

class Oled {
public:
  
  static void task();
  static void init();

private:
  enum formatState {
      FMT1 = 0, FMT2 = 1, FMT3 = 2
  };
  static formatState state;
  static void printFormat1();
  static void printFormat2();
  static void printFormat3();

  static void printCentre(const char*  , int);
  //static void printCentre(String  , int);

};

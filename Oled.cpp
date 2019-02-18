#include "Oled.h"

//auto idleString = String("IDLE");
///////////////////////////////////
// TeensyView Object Declaration //
///////////////////////////////////
//Standard
#define PIN_RESET 15
#define PIN_DC    5
#define PIN_CS    10
#define PIN_SCK   13
#define PIN_MOSI  11

static TeensyView oled(PIN_RESET, PIN_DC, PIN_CS, PIN_SCK, PIN_MOSI);
Oled::formatState Oled::state;

void Oled::init(){
  oled.begin();    // Initialize the OLED
  oled.clear(ALL); // Clear the display's internal memory
  oled.display();  // Display what's in the buffer (splashscreen)
  //delay(1000);     // Delay 1000 ms
  oled.clear(PAGE); // Clear the buffer.
  state = FMT3;
}

void Oled::printFormat1(){
  const int row0 = 0;
  const int row1 = 42;
  const int row2 = 85;

  oled.clear(PAGE);            // Clear the display
  oled.setCursor(row0, 0);        // Set cursor to top-left
  oled.setFontType(1);         // Smallest font
  oled.print("Vcur");          // Print "A0"
  oled.setCursor(row1, 0);
  oled.print("Vdif");
  oled.setCursor(row2, 0);
  oled.print("Tcur");

  oled.setFontType(2);         // 7-segment font
  oled.setCursor(row0, 16);
  oled.print(analogRead(A0));  // Print a0 reading
  oled.setCursor(row1, 16);
  oled.print(analogRead(A0));
  oled.setCursor(row2, 16);
  oled.print(12.3);
  oled.display();
}

void Oled::printFormat2(){
  int row0 = 0;
  int row1 = 42;
  int row2 = 85;

  oled.clear(PAGE);            // Clear the display
  oled.setCursor(row0, 0);        // Set cursor to top-left
  oled.setFontType(1);         // Smallest font
  oled.print("Vmax");          // Print "A0"
  oled.setCursor(row1, 0);
  oled.print("Vlow");
  oled.setCursor(row2, 0);
  oled.print("Tmax");

  oled.setFontType(2);         // 7-segment font
  oled.setCursor(row0, 16);
  oled.print(analogRead(A0));  // Print a0 reading
  oled.setCursor(row1, 16);
  oled.print(analogRead(A0));
  oled.setCursor(row2, 16);
  oled.print(12.3);
  oled.display();
}

void Oled::printFormat3(){
  switch(Controller::getState()){
    case Controller::IDLE:
      Oled::printCentre("IDLE", 1);
      break;
    case Controller::CHARGING:
      Oled::printCentre("CHARGING", 1);
      break;
    case Controller::DRIVE:
      Oled::printCentre("DRIVE", 1);
      break;
    default:
      Oled::printCentre("default", 1);
      break;
  }
}

void Oled::task(){
  const int stateticks = 6;
  static int ticks = 0;
  switch (state){
    case FMT1:
      Oled::printFormat1();
      if (ticks >= stateticks){
        ticks = 0;
        state = FMT2;
      }
      break;
    case FMT2:
      Oled::printFormat2();
      if (ticks >= stateticks){
        ticks = 0;
        state = FMT3;
      }
      break;
    case FMT3:
      Oled::printFormat3();
      if (ticks >= stateticks){
        ticks = 0;
        state = FMT1;
      }
      break;
    default:
      break;
  }
  ticks++;
  
}

// Center and print a small values string
// This function is quick and dirty. Only works for titles one
// line long.
void Oled::printCentre(const char* value, int font)
{
  int middleX = oled.getLCDWidth() / 2;
  int middleY = oled.getLCDHeight() / 2;

  oled.clear(PAGE);
  oled.setFontType(font);
  // Try to set the cursor in the middle of the screen
  oled.setCursor(middleX - (oled.getFontWidth() * (strlen(value) / 2)),
                 middleY - (oled.getFontHeight() / 2));
  // Print the title:
  oled.print(value);
  oled.display();
}
/*
// Center and print a small values string
// This function is quick and dirty. Only works for titles one
// line long.
void Oled::printCentre(const String value, int font)
{
  int middleX = oled.getLCDWidth() / 2;
  int middleY = oled.getLCDHeight() / 2;

  oled.clear(PAGE);
  oled.setFontType(font);
  // Try to set the cursor in the middle of the screen
  oled.setCursor(middleX - (oled.getFontWidth() * (value.length() / 2)),
                 middleY - (oled.getFontHeight() / 2));
  // Print the title:
  oled.print(value);
  oled.display();
}*/

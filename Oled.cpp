#include "Oled.hpp"
#include "Logger.hpp"

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


/*

Vbat      Le voltage total courant des batteries (168V nominal)
Vmax    Le voltage maximal total atteint par les batteries depuis le dernier reset ( 168.4V pour s’assurer que les limites de charge fonctionnent)
Vmin     Le voltage minimal total atteint par les batteries (117.6V pour s’assurer qu’on ne décharge pas trop)
VChi      Le voltage courant de la cellule la plus élevée (4.1V max)
VClo      Le voltage courant de la cellule la plus basse (2.8V ???)
VCdiff   La différence courante entre la cellule la plus élevée et la plus basse (200 à 300 mV selon la stratégie de balancing)
VCmax  Le voltage maximal atteint par une cellule depuis le dernier reset (4.1V max)
VCmin  Le voltage minimal atteint par une cellule depuis le dernier reset (2.8 min)
VCdmax              La différence maximale historique pour valider la stratégie de balancing (même que précédent)
Tbat      La température courante moyenne des senseurs de température (41C)
Tmax    La température maximale mesurée par un capteur depuis le dernier reset (???)

 

Des idées de séquence, mais tu peux améliorer à ta guise…
Valeurs courantes :

Vbat
Tbat

VChi      ou VCdiff au lieu de ces deux valeurs…
VClo

Valeurs historiques :
VCmax
VCmin

VCdmax
Tmax
 */


Oled::Oled(Controller* cont_inst_ptr) {
  oled.begin();    // Initialize the OLED
  oled.clear(ALL); // Clear the display's internal memory
  oled.display();  // Display what's in the buffer (splashscreen)
  //delay(1000);     // Delay 1000 ms
  oled.clear(PAGE); // Clear the buffer.
  state = FMT5;
  controller_inst_ptr = cont_inst_ptr;
}

/*
Vbat
Tbat
*/
void Oled::printFormat1() {
  const int col0 = 0;
  const int col1 = oled.getLCDWidth() / 2;

  oled.clear(PAGE);            // Clear the display
  oled.setCursor(col0, 0);     // Set cursor to top-left
  oled.setFontType(1);         // Smallest font
  oled.print("Vbat");
  oled.setCursor(col1, 0);
  oled.print("Tbat");

  oled.setFontType(2);         // 7-segment font
  oled.setCursor(col0, oled.getLCDHeight() / 2);
  oled.print(controller_inst_ptr->getBMSPtr()->getPackVoltage());
  oled.setCursor(col1, oled.getLCDHeight() / 2);
  oled.print(controller_inst_ptr->getBMSPtr()->getAvgTemperature());
  oled.display();
}

/*
VChi      ou VCdiff au lieu de ces deux valeurs…
VClo
*/
void Oled::printFormat2() {
  const int col0 = 0;
  const int col1 = oled.getLCDWidth() / 2;

  oled.clear(PAGE);            // Clear the display
  oled.setCursor(col0, 0);        // Set cursor to top-left
  oled.setFontType(1);         // Smallest font
  oled.print("VChi");          // Print "A0"
  oled.setCursor(col1, 0);
  oled.print("VClo");

  oled.setFontType(2);         // 7-segment font
  oled.setCursor(col0, oled.getLCDHeight() / 2);
  oled.print(controller_inst_ptr->getBMSPtr()->getHighCellVolt());  // Print a0 reading
  oled.setCursor(col1, oled.getLCDHeight() / 2);
  oled.print(controller_inst_ptr->getBMSPtr()->getLowCellVolt());
  oled.display();
}

/*
VCmax
VCmin

*/
void Oled::printFormat3() {
  const int col0 = 0;
  const int col1 = oled.getLCDWidth() / 2;

  oled.clear(PAGE);            // Clear the display
  oled.setCursor(col0, 0);        // Set cursor to top-left
  oled.setFontType(1);         // Smallest font
  oled.print("VCmin");          
  oled.setCursor(col1, 0);
  oled.print("VCmax");

  oled.setFontType(2);         // 7-segment font
  oled.setCursor(col0, oled.getLCDHeight() / 2);
  oled.print(controller_inst_ptr->getBMSPtr()->getHistLowestCellVolt());
  oled.setCursor(col1, oled.getLCDHeight() / 2);
  oled.print(controller_inst_ptr->getBMSPtr()->getHistHighestCellVolt());
  oled.display();
}

/*
VCdmax
Tmax
*/
void Oled::printFormat4() {
  const int col0 = 0;
  const int col1 = oled.getLCDWidth() / 2;

  oled.clear(PAGE);            // Clear the display
  oled.setCursor(col0, 0);        // Set cursor to top-left
  oled.setFontType(1);         // Smallest font
  oled.print("VCdmax");          
  oled.setCursor(col1, 0);
  oled.print("Tmax");

  oled.setFontType(2);         // 7-segment font
  oled.setCursor(col0, oled.getLCDHeight() / 2);
  oled.print(controller_inst_ptr->getBMSPtr()->getHistLowestCellVolt());
  oled.setCursor(col1, oled.getLCDHeight() / 2);
  oled.print(controller_inst_ptr->getBMSPtr()->getHistHighestCellVolt());
  oled.display();
}

void Oled::printFormat5() {
  switch (controller_inst_ptr->getState()) {
    case Controller::INIT:
      Oled::printCentre("INIT", 1);
      break;
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

void Oled::doOled() {
  const int stateticks = 6;
  static int ticks = 0;
  switch (state) {
    case FMT1:
      Oled::printFormat1();
      if (ticks >= stateticks) {
        ticks = 0;
        state = FMT2;
      }
      break;
    case FMT2:
      Oled::printFormat2();
      if (ticks >= stateticks) {
        ticks = 0;
        state = FMT3;
      }
      break;
    case FMT3:
      Oled::printFormat3();
      if (ticks >= stateticks) {
        ticks = 0;
        state = FMT4;
      }
      break;
    case FMT4:
      Oled::printFormat4();
      if (ticks >= stateticks) {
        ticks = 0;
        state = FMT5;
      }
      break;
    case FMT5:
      Oled::printFormat5();
      if (ticks >= stateticks) {
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

#pragma once

#include <Arduino.h>

///////////////////////////////////
// Teensy pin configuration      //
///////////////////////////////////

//side 1
//#define SERIAL1_RX        0
//#define SERIAL1_TX        1
#define FAULTLOOP             2     //GPIO2
//#define CAN_TX            3
//#define CAN_RX            4
#define OLED_PIN_DC       5
//#define GPIO6             6
#define SERIAL3_RX        7
#define SERIAL3_TX        8
//#define GPIO9             9     //alternate serial2_rx
#define OLED_PIN_CS       10    //alternate serial2_tx
#define OLED_PIN_MOSI     11
//#define GPIO12            12

//side 2
#define OLED_PIN_SCK      13    //LED
#define OLED_PIN_RESET    15



//Set to the proper port for your USB connection - SerialUSB on Due (Native) or Serial for Due (Programming) or Teensy
#define SERIALCONSOLE   Serial



// victron serial VE direct bus config
//#define VE Serial2



#define EEPROM_VERSION      0x14    //update any time EEPROM struct below is changed.
#define EEPROM_PAGE         0


#define VERSION 1
#define CANBUS_SPEED 500000
#define CANBUS_ADDRESS 1
//ground the fault line if this threshold is crossed
#define OVER_V_SETPOINT 4.2f
//ground the fault line if this threshold is crossed
#define UNDER_V_SETPOINT 3.0f
//stop charging (if possible)
#define MAX_CHARGE_V_SETPOINT 4.1f
//cycle charger to force a chargin cycle
#define CHARGER_CYCLE_V_SETPOINT 3.9f
//issue a warning on OLED and serial console if a cell is that close to a OV or UV fault.
#define WARN_CELL_V_OFFSET 0.1f
//start balancing when high and low cell reach this delta
#define CELLS_V_DELTA 0.2f
#define OVER_T_SETPOINT 65.0f
#define UNDER_T_SETPOINT -10.0f
//issue a warning on OLED and serial console if T is that close to a OT or UT fault.
#define WARN_T_OFFSET 5.0f
//start balancing when highest cell reaches this setpoint (taken from tom debree)
#define BALANCE_V_SETPOINT 3.9f
//balance all cells above the lowest cell by this offset (taken from tom debree)
#define BALANCE_CELL_V_OFFSET 0.04f

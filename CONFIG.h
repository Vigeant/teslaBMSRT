#pragma once

#include <Arduino.h>

//Set to the proper port for your USB connection - SerialUSB on Due (Native) or Serial for Due (Programming) or Teensy
#define SERIALCONSOLE   Serial



// victron serial VE direct bus config
#define VE Serial2



#define EEPROM_VERSION      0x14    //update any time EEPROM struct below is changed.
#define EEPROM_PAGE         0

typedef struct {
  uint8_t version;
  uint8_t checksum;
  uint32_t canSpeed;
  uint8_t batteryID;  //which battery ID should this board associate as on the CAN bus
  uint8_t logLevel;
  float OverVSetpoint;
  float UnderVSetpoint;
  float ChargeVsetpoint;
  float DischVsetpoint;
  float ChargeHys;
  float StoreVsetpoint;
  float WarnOff;
  float OverTSetpoint;
  float UnderTSetpoint;
  float ChargeTSetpoint;
  float DisTSetpoint;
  float WarnToff;
  float CellGap;
  uint8_t IgnoreTemp;
  float IgnoreVolt;
  float balanceVoltage;
  float balanceHyst;
  int Scells;
  int Pstrings;
  int CAP;
  int chargecurrentmax;
  int chargecurrentend;
  int discurrentmax ;
  int socvolt[4];
  int invertcur;
  int cursens;
  int voltsoc;
  int Pretime;
  int conthold;
  int Precurrent;
  float convhigh;
  float convlow;
  uint16_t offset1;
  uint16_t offset2;
  int balanceDuty;
  int ESSmode;
  int gaugelow;
  int gaugehigh;
  int ncur;
  int chargertype;
  int chargerspd;
  uint16_t UnderDur;
  uint16_t CurDead;
  float DisTaper;
} EEPROMSettings;

//#include "config.h"
#include "BMSModuleManager.hpp"
//#include "BMSUtil.h"
#include "Logger.hpp"

//extern EEPROMSettings settings;

BMSModuleManager::BMSModuleManager()
{
  for (int i = 0; i < MAX_MODULE_ADDR; i++) {
    modules[i].setAddress(i + 1);
  }
  lowestPackVolt = 1000.0f;
  highestPackVolt = 0.0f;
  lowestPackTemp = 200.0f;
  highestPackTemp = -100.0f;
  isFaulted = false;
  Pstring = 1;
}

void BMSModuleManager::resetModuleRecordedValues()
{
  for (int y = 0; y < MAX_MODULE_ADDR; y++)
  {
    modules[y].resetRecordedValues();
  }
}

void BMSModuleManager::balanceCells(uint8_t duration)
{
  uint8_t balance = 0;//bit 0 - 5 are to activate cell balancing 1-6

  for (int y = 0; y < MAX_MODULE_ADDR; y++)
  {
    if (modules[y].getAddress() > 0)
    {
      balance = 0;
      for (int i = 0; i < 6; i++)
      {
        if (getLowCellVolt() < modules[y].getCellVoltage(i))
        {
          balance = balance | (1 << i);

        }
      }
      LOG_DEBUG("module %d - 0x%02X\n", modules[y].getAddress(), balance);
      (void) modules[y].balanceCells(balance, duration);
    }
  }
}

/*
   Force all modules to reset back to address 0 then set them all up in order so that the first module
   in line from the master board is 1, the second one 2, and so on.
*/
void BMSModuleManager::renumberBoardIDs()
{
  int16_t err;
  uint8_t buff[30];

  //Reset all boards
  for (int y = 0; y < MAX_MODULE_ADDR; y++) {
    modules[y].setAddress(0);
  }

  numFoundModules = 0;
  LOG_INFO("\n\nReseting all boards\n\n");
  if ((err = BMSDW(BROADCAST_ADDR, 0x3C, 0xA5)) < 0) {
    BMSD_LOG_ERR(BROADCAST_ADDR, err, "Broadcasting reset");
  }

  //assign address to boards that respond to address 0
  for (int y = 0; y < MAX_MODULE_ADDR; y++) {
    //for (int y = 0; y < 2; y++) {
    LOG_INFO("sending read on address 0\n");
    //check if a board responds to address 0
    if ((err = BMSDR(0, 0, 1, buff)) != READ_CRC_FAIL) {
      if (err == READ_RECV_LEN_MISMATCH) {
        LOG_INFO("Did not get a response on address 0... done assigning addresses\n", y);
        break;
      } else if (err < 0) {
        //retry
        y--;
        continue;
      }

    }
    LOG_INFO("Got a response to address 0\n");

    //write address register
    LOG_INFO("Assigning it address 0x%02X\n", y + 1);
    if ((err = BMSDW(0, REG_ADDR_CTRL, (y + 1) | 0x80)) < 0 ) {
      BMSD_LOG_ERR(y + 1, err, "write address register");
    }

    modules[y].setAddress(y + 1);
    LOG_INFO("Address %d assigned\n", modules[y].getAddress());
    numFoundModules++;
  }
}

/*
  After a RESET boards have their faults written due to the hard restart or first time power up, this clears their faults
*/
void BMSModuleManager::clearFaults()
{
  int16_t err;

  //reset alerts status
  if ((err = BMSDW(BROADCAST_ADDR, REG_ALERT_STATUS, 0xFF)) < 0) {
    BMSD_LOG_ERR(BROADCAST_ADDR, err, "reset alerts status");
  }

  //clear alerts status
  if ((err = BMSDW(BROADCAST_ADDR, REG_ALERT_STATUS, 0)) < 0) {
    BMSD_LOG_ERR(BROADCAST_ADDR, err, "clear alerts status");
  }

  //reset faults status
  if ((err = BMSDW(BROADCAST_ADDR, REG_FAULT_STATUS, 0xFF)) < 0) {
    BMSD_LOG_ERR(BROADCAST_ADDR, err, "reset faults status");
  }

  //clear faults status
  if ((err = BMSDW(BROADCAST_ADDR, REG_FAULT_STATUS, 0)) < 0) {
    BMSD_LOG_ERR(BROADCAST_ADDR, err, "clear faults status");
  }

  //wtf!!!
  isFaulted = false;
}

/*
  Puts all boards on the bus into a Sleep state, very good to use when the vehicle is at rest state.
  Pulling the boards out of sleep only to check voltage decay and temperature when the contactors are open.
*/
void BMSModuleManager::sleepBoards() {
  int16_t err;
  //put boards to sleep
  if ((err = BMSDW(BROADCAST_ADDR, REG_IO_CTRL, 0x04)) < 0) {
    BMSD_LOG_ERR(BROADCAST_ADDR, err, "put boards to sleep");
  }
}

/*
  Wakes all the boards up and clears thier SLEEP state bit in the Alert Status Registery
*/
void BMSModuleManager::wakeBoards()
{
  int16_t err;
  //wake boards up
  if ((err = BMSDW(BROADCAST_ADDR, REG_IO_CTRL, 0x00)) < 0) {
    BMSD_LOG_ERR(BROADCAST_ADDR, err, "wake boards up");
  }

  //reset faults
  if ((err = BMSDW(BROADCAST_ADDR, REG_ALERT_STATUS, 0x04)) < 0) {
    BMSD_LOG_ERR(BROADCAST_ADDR, err, "wake boards up reset faults");
  }

  //clear faults
  if ((err = BMSDW(BROADCAST_ADDR, REG_ALERT_STATUS, 0x00)) < 0) {
    BMSD_LOG_ERR(BROADCAST_ADDR, err, "wake boards up clear faults");
  }
}

void BMSModuleManager::getAllVoltTemp() {
  int16_t err;
  float tempPackVolt = 0.0f;
  //stop balancing
  if ((err = BMSDW(BROADCAST_ADDR, REG_BAL_CTRL, 0x00)) < 0) {
    BMSD_LOG_ERR(BROADCAST_ADDR, err, "getAllVoltTemp, stop balancing");
  }
  

  for (int y = 0; y < MAX_MODULE_ADDR; y++)
  {
    if (modules[y].getAddress() > 0) {
      LOG_DEBUG("Module %i exists. Reading voltage and temperature values\n", modules[y].getAddress());
      modules[y].updateInstanceWithModuleValues();
      LOG_DEBUG("Module voltage: %f\n", modules[y].getModuleVoltage());
      LOG_DEBUG("Lowest Cell V: %f     Highest Cell V: %f\n", modules[y].getLowCellV(), modules[y].getHighCellV());
      LOG_DEBUG("Temp1: %f       Temp2: %f\n", modules[y].getTemperature(0), modules[y].getTemperature(1));
      tempPackVolt += modules[y].getModuleVoltage();
      if (modules[y].getLowTemp() < lowestPackTemp) lowestPackTemp = modules[y].getLowTemp();
      if (modules[y].getHighTemp() > highestPackTemp) highestPackTemp = modules[y].getHighTemp();
    }
  }



  tempPackVolt = tempPackVolt / Pstring;
  if (packVolt > highestPackVolt) highestPackVolt = tempPackVolt;
  if (packVolt < lowestPackVolt) lowestPackVolt = tempPackVolt;



  //WTF!!! this does not belong here!!!
  /*
    if (digitalRead(11) == LOW) {
    if (!isFaulted) LOG_ERROR("One or more BMS modules have entered the fault state!\n");
    isFaulted = true;
    }
    else
    {
    if (isFaulted) LOG_INFO("All modules have exited a faulted state\n");
    isFaulted = false;
    }
  */

  HighCellVolt = 0.0;
  for (int y = 0; y < MAX_MODULE_ADDR; y++)
  {
    if (modules[y].getAddress() > 0) {
      if (modules[y].getHighCellV() >  HighCellVolt)  HighCellVolt = modules[y].getHighCellV();
    }
  }
  LowCellVolt = 5.0;
  for (int y = 0; y < MAX_MODULE_ADDR; y++)
  {
    if (modules[y].getAddress() > 0) {
      if (modules[y].getLowCellV() <  LowCellVolt)  LowCellVolt = modules[y].getLowCellV();
    }
  }


  //save values to objects
  packVolt = tempPackVolt;
  
}

float BMSModuleManager::getLowCellVolt()
{
  return LowCellVolt;
}

float BMSModuleManager::getHighCellVolt()
{
  return HighCellVolt;
}

float BMSModuleManager::getPackVoltage()
{
  return packVolt;
}

float BMSModuleManager::getLowVoltage()
{
  return lowestPackVolt;
}

float BMSModuleManager::getHighVoltage()
{
  return highestPackVolt;
}

void BMSModuleManager::setBatteryID(int id)
{
  batteryID = id;
}

void BMSModuleManager::setPstrings(int Pstrings)
{
  Pstring = Pstrings;
}

float BMSModuleManager::getAvgTemperature()
{
  float avg = 0.0f;
  highTemp = -100;
  lowTemp = 999;
  int y = 0; //counter for modules below -70 (no sensors connected)
  for (int x = 0; x < MAX_MODULE_ADDR; x++)
  {
    if (modules[x].getAddress() > 0)
    {
      if (modules[x].getAvgTemp() > -70)
      {
        avg += modules[x].getAvgTemp();
      }
      else
      {
        y++;
      }
    }
  }
  avg = avg / (float)(numFoundModules - y);

  return avg;
}

float BMSModuleManager::getHighTemperature()
{
  highTemp = -100;
  for (int x = 0; x < MAX_MODULE_ADDR; x++)
  {
    if (modules[x].getAddress() > 0)
    {
      if (modules[x].getAvgTemp() > -70)
      {
        if (modules[x].getAvgTemp() > highTemp)
        {
          highTemp = modules[x].getAvgTemp();
        }
      }
    }
  }

  return highTemp;
}

float BMSModuleManager::getLowTemperature()
{
  lowTemp = 999;
  for (int x = 0; x < MAX_MODULE_ADDR; x++)
  {
    if (modules[x].getAddress() > 0)
    {
      if (modules[x].getAvgTemp() > -70)
      {
        if (modules[x].getAvgTemp() < lowTemp)
        {
          lowTemp = modules[x].getAvgTemp();
        }
      }
    }
  }
  return lowTemp;
}

float BMSModuleManager::getAvgCellVolt()
{
  float avg = 0.0f;
  for (int x = 0; x < MAX_MODULE_ADDR; x++)
  {
    if (modules[x].getAddress() > 0) avg += modules[x].getAverageV();
  }
  avg = avg / (float)numFoundModules;

  return avg;
}

void BMSModuleManager::printPackSummary()
{
  uint8_t faults;
  uint8_t alerts;
  uint8_t COV;
  uint8_t CUV;

  LOG_CONSOLE("");
  LOG_CONSOLE("");
  LOG_CONSOLE("");
  LOG_CONSOLE("Modules: %i    Voltage: %fV   Avg Cell Voltage: %fV     Avg Temp: %fC ", numFoundModules,
              getPackVoltage(), getAvgCellVolt(), getAvgTemperature());
  LOG_CONSOLE("");
  for (int y = 0; y < MAX_MODULE_ADDR; y++)
  {
    if (modules[y].getAddress() > 0)
    {
      faults = modules[y].getFaults();
      alerts = modules[y].getAlerts();
      COV = modules[y].getCOVCells();
      CUV = modules[y].getCUVCells();

      LOG_CONSOLE("                               Module #%i\n", y);

      LOG_CONSOLE("  Voltage: %fV   (%fV-%fV)     Temperatures: (%fC-%fC)\n", modules[y].getModuleVoltage(),
                  modules[y].getLowCellV(), modules[y].getHighCellV(), modules[y].getLowTemp(), modules[y].getHighTemp());
      if (faults > 0)
      {
        LOG_CONSOLE("  MODULE IS FAULTED:\n");
        if (faults & 1)
        {
          LOG_CONSOLE("    Overvoltage Cell Numbers (1-6): ");
          for (int i = 0; i < 6; i++)
          {
            if (COV & (1 << i))
            {
              LOG_CONSOLE("%d ", i + 1);
            }
          }
          LOG_CONSOLE("\n");
        }
        if (faults & 2)
        {
          LOG_CONSOLE("    Undervoltage Cell Numbers (1-6): ");
          for (int i = 0; i < 6; i++)
          {
            if (CUV & (1 << i))
            {
              LOG_CONSOLE("%d ", i + 1);
            }
          }
          LOG_CONSOLE("\n");
        }
        if (faults & 4)
        {
          LOG_CONSOLE("    CRC error in received packet\n");
        }
        if (faults & 8)
        {
          LOG_CONSOLE("    Power on reset has occurred\n");
        }
        if (faults & 0x10)
        {
          LOG_CONSOLE("    Test fault active\n");
        }
        if (faults & 0x20)
        {
          LOG_CONSOLE("    Internal registers inconsistent\n");
        }
      }
      if (alerts > 0)
      {
        LOG_CONSOLE("  MODULE HAS ALERTS:\n");
        if (alerts & 1)
        {
          LOG_CONSOLE("    Over temperature on TS1\n");
        }
        if (alerts & 2)
        {
          LOG_CONSOLE("    Over temperature on TS2\n");
        }
        if (alerts & 4)
        {
          LOG_CONSOLE("    Sleep mode active\n");
        }
        if (alerts & 8)
        {
          LOG_CONSOLE("    Thermal shutdown active\n");
        }
        if (alerts & 0x10)
        {
          LOG_CONSOLE("    Test Alert\n");
        }
        if (alerts & 0x20)
        {
          LOG_CONSOLE("    OTP EPROM Uncorrectable Error\n");
        }
        if (alerts & 0x40)
        {
          LOG_CONSOLE("    GROUP3 Regs Invalid\n");
        }
        if (alerts & 0x80)
        {
          LOG_CONSOLE("    Address not registered\n");
        }
      }
      if (faults > 0 || alerts > 0) LOG_CONSOLE("\n");
    }
  }
}

void BMSModuleManager::printPackDetails(int digits)
{
  //uint8_t faults;
  //uint8_t alerts;
  //uint8_t COV;
  //uint8_t CUV;
  int cellNum = 0;

  LOG_CONSOLE("\n");
  LOG_CONSOLE("\n");
  LOG_CONSOLE("\n");
  LOG_CONSOLE("Modules: %i Cells: %i Strings: %i  Voltage: %fV   Avg Cell Voltage: %fV  Low Cell Voltage: %fV   High Cell Voltage: %fV Delta Voltage: %zmV   Avg Temp: %fC \n", numFoundModules, seriescells(),
              Pstring, getPackVoltage(), getAvgCellVolt(), LowCellVolt, HighCellVolt, (HighCellVolt - LowCellVolt) * 1000, getAvgTemperature());
  LOG_CONSOLE("\n");
  for (int y = 0; y < MAX_MODULE_ADDR; y++)
  {
    if (modules[y].getAddress() > 0)
    {
      //faults = modules[y].getFaults();
      //alerts = modules[y].getAlerts();
      //COV = modules[y].getCOVCells();
      //CUV = modules[y].getCUVCells();

      LOG_CONSOLE("Module #%d\n", modules[y].getAddress());
      if (y < 10) LOG_CONSOLE(" ");
      LOG_CONSOLE("  %fV\n", modules[y].getModuleVoltage());
      for (int i = 0; i < 6; i++)
      {
        if (cellNum < 10) LOG_CONSOLE(" ");
        LOG_CONSOLE("  Cell%d: %fV\n", cellNum++, modules[y].getCellVoltage(i));
      }
      LOG_CONSOLE("  Neg Term Temp: %fC\t", modules[y].getTemperature(0));
      LOG_CONSOLE("Pos Term Temp: %fC\n", modules[y].getTemperature(1));
    }
  }
}

void BMSModuleManager::printAllCSV()
{
  for (int y = 0; y < MAX_MODULE_ADDR; y++)
  {
    if (modules[y].getAddress() > 0)
    {
      LOG_CONSOLE("%d", modules[y].getAddress());
      LOG_CONSOLE(",");
      for (int i = 0; i < 6; i++)
      {
        LOG_CONSOLE("%d,", modules[y].getCellVoltage(i));
      }
      LOG_CONSOLE("%f,", modules[y].getTemperature(0));
      LOG_CONSOLE("%f\n", modules[y].getTemperature(1));
    }
  }
}

/* 
 * Adapted from the work of Tom Debree
 * https://github.com/tomdebree/TeslaBMSV2/blob/master/BMSUtil.h
 */
#include "BMSDriver.hpp"
#include "Logger.hpp"


#define MAX_PAYLOAD 128
#define ILLEGAL_READ_LEN -2
#define READ_CRC_FAIL -3
#define READ_RECV_MODADDR_MISMATCH -4
#define READ_RECV_ADDR_MISMATCH -5
#define READ_RECV_LEN_MISMATCH -6

BMSDriver::BMSDriver() {
  SERIALBMS.begin(612500);
}

int16_t BMSDriver::read(const uint8_t moduleAddress, const uint8_t readAddress, const uint8_t readLen, uint8_t* recvBuff ) {
  uint8_t byteIndex = 0;
  uint8_t maxLen = readLen + 3;//[modAddr][readAddr][data][CRC]
  uint8_t buff[MAX_PAYLOAD];

  //check if the read is larger than our recv buffer
  if (maxLen > MAX_PAYLOAD) return ILLEGAL_READ_LEN;

  //clean buffers
  memset(buff, 0,  sizeof(buff));
  memset(recvBuff, 0,  readLen);

  //sending read command on serial port
  LOG_DEBUG("Reading module:%3d, addr:0x%02x, len:%d\n", moduleAddress, readAddress, readLen );
  SERIALBMS.write(moduleAddress);
  SERIALBMS.write(readAddress);
  SERIALBMS.write(readLen);

  //receiving answer
  //read the data in the buffer
  for (byteIndex = 0; SERIALBMS.available() && (byteIndex < maxLen); byteIndex++) {
    buff[byteIndex] = SERIALBMS.read();
  }

  //empty out the serial read buffer
  if (maxLen == byteIndex)
  {
    LOG_WARN("SERIALBMS still had crap in its buffer after reading readLen | Reading module:%3d, addr:0x%02x, len:%d\n", moduleAddress, readAddress, readLen );
    while (SERIALBMS.available()) SERIALBMS.read();
  } else {
    LOG_ERROR("READ_RECV_LEN_MISMATCH | Reading module:%3d, addr:0x%02x, len:%d\n", moduleAddress, readAddress, readLen );
    return READ_RECV_LEN_MISMATCH;
  }

  //verify the CRC
  if (genCRC(buff, maxLen - 1) != buff[maxLen - 1]){
    LOG_ERROR("READ_CRC_FAIL | Reading module:%3d, addr:0x%02x, len:%d\n", moduleAddress, readAddress, readLen );
    return READ_CRC_FAIL;
  }

  //success! remove 3 bytes protocol wrapper around payload
  memcpy(recvBuff, &buff[2], readLen); //[modAddr][readAddr][data][CRC] -> [data]
  LOG_DEBUG("Received:");
  if (log_inst.getLogLevel() == Logger::Debug) {
    for (int i = 0; i < readLen; i++) {
      if (i % 16 == 0) LOG_CONSOLE("\n%04x\t", i);
      LOG_CONSOLE("%02X ", recvBuff[i]);
    }
    LOG_CONSOLE("\n");
  }
  return byteIndex;
}


int16_t BMSDriver::write(const uint8_t moduleAddress, const uint8_t writeAddress, const uint8_t sendByte) {
  uint8_t byteIndex = 0;
  uint8_t maxLen = 4;//[modAddr][readAddr][data][CRC]
  uint8_t sendBuff[4];
  uint8_t recvBuff[4];

  //clean buffer
  memset(sendBuff, 0,  sizeof(sendBuff));
  memset(recvBuff, 0,  sizeof(recvBuff));

  //sending read command on serial port
  LOG_DEBUG("Writing module:%3d, addr:0x%02x, byte:%d\n", moduleAddress, writeAddress, sendByte );
  sendBuff[0] = moduleAddress | 1;
  sendBuff[1] = writeAddress;
  sendBuff[2] = sendByte;
  sendBuff[3] = genCRC(sendBuff, 3);
  SERIALBMS.write(sendBuff, sizeof(sendBuff));

  //receiving answer
  //read the data in the buffer
  for (byteIndex = 0; SERIALBMS.available() && (byteIndex < maxLen); byteIndex++) {
    recvBuff[byteIndex] = SERIALBMS.read();
  }

  //empty out the serial read buffer
  if (maxLen == byteIndex)
  {
    LOG_WARN("SERIALBMS still had crap in its buffer after reading readLen | Writing module:%3d, addr:0x%02x, byte:%d\n", moduleAddress, writeAddress, sendByte );
    while (SERIALBMS.available()) SERIALBMS.read();
  } else {
    LOG_ERROR("READ_RECV_LEN_MISMATCH | Writing module:%3d, addr:0x%02x, byte:%d\n", moduleAddress, writeAddress, sendByte );
    return READ_RECV_LEN_MISMATCH;
  }

  //verify the CRC
  if (sendBuff[maxLen - 1]!= recvBuff[maxLen - 1]){
    LOG_ERROR("READ_CRC_FAIL | Writing module:%3d, addr:0x%02x, byte:%d\n", moduleAddress, writeAddress, sendByte );
    return READ_CRC_FAIL;
  }

  //success! remove 3 bytes protocol wrapper around payload
  LOG_DEBUG("Write Successful!\n");
  return byteIndex;
}


uint8_t BMSDriver::genCRC(const uint8_t * buf, const uint8_t bufLen) {
  uint8_t generator = 0x07;
  uint8_t crc = 0;

  for (int x = 0; x < bufLen; x++)
  {
    crc ^= buf[x]; /* XOR-in the next input byte */

    for (int i = 0; i < 8; i++)
    {
      if ((crc & 0x80) != 0)
      {
        crc = (uint8_t)((crc << 1) ^ generator);
      }
      else
      {
        crc <<= 1;
      }
    }
  }

  return crc;
}

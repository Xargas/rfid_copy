
#include "globals.h"

MFRC522 globalRfid(SDA_NSS, RST);
byte globalBuffer[BUFFER_MAX_SECTORS][BUFFER_MAX_SECTOR_SIZE] = {0u};
bool globalSkipAuth = false;

MFRC522::MIFARE_Key globalKey = keyFactorFF;

byte globalBuffer_0[BUFFER_BLOCK_SIZE * 4] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x07, 0x80, 0x69, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

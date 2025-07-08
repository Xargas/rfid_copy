
#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <Arduino.h>
#include <MFRC522.h>

#include "wiring.h"
#include "rfid_constants.h"

extern MFRC522 globalRfid;
extern byte globalBuffer[BUFFER_MAX_SECTORS][BUFFER_MAX_SECTOR_SIZE];
extern bool globalSkipAuth;

extern MFRC522::MIFARE_Key globalKey;

extern byte globalBuffer_0[BUFFER_BLOCK_SIZE * 4];

#endif

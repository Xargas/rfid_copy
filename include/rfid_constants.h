
#ifndef __RFID_CONSTANTS_H__
#define __RFID_CONSTANTS_H__

#include <MFRC522.h>

#define BUFFER_SIZE 18u
#define BUFFER_BLOCK_SIZE 16u
#define BUFFER_CRC_SIZE 2u
#define BUFFER_MAX_SECTORS 40u
#define BUFFER_MAX_SECTOR_SIZE 256u

extern const MFRC522::MIFARE_Key keyFactorFF;
extern const MFRC522::MIFARE_Key keyFactor00;
extern const MFRC522::MIFARE_Key keyMifareApplicationDirectory;
extern const MFRC522::MIFARE_Key keyNfcForum;

#endif

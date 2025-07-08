
#include "globals.h"
#include "debug_commands.h"



void user_readRfidToGlobal(void)
{
  MFRC522::PICC_Type piccType;
  uint8_t numSectors = 0;
  byte numRead = 0;
  byte blockAddr = 0;
  byte no_of_blocks = 0;
  byte firstBlock = 0;
  byte localBuffer[BUFFER_SIZE];
#ifdef __DEBUG__
  byte error = 0;
#endif

  globalRfid.PICC_ReadCardSerial();

#ifdef __DEBUG__
  Serial.print(F("\nPICC type: "));
#endif
  piccType = globalRfid.PICC_GetType(globalRfid.uid.sak);
#ifdef __DEBUG__
  Serial.println(globalRfid.PICC_GetTypeName(piccType));
#endif

  if ((piccType != MFRC522::PICC_TYPE_MIFARE_MINI) &&
      (piccType != MFRC522::PICC_TYPE_MIFARE_1K  ) &&
      (piccType != MFRC522::PICC_TYPE_MIFARE_4K  )
     )
  {
    Serial.println(F("ERROR: Not of type MIFARE Classic!"));
    return;
  }

  switch (piccType)
  {
    case MFRC522::PICC_TYPE_MIFARE_1K:
      numSectors = 16u;
      break;
    case MFRC522::PICC_TYPE_MIFARE_4K:
      numSectors = 40u;
      break;
    default:
      Serial.println(F("ERROR: Card type not yet supported!"));
      return;
      break;
  }

#ifdef __DEBUG__
  globalRfid.PICC_DumpMifareClassicSectorToSerial(&globalRfid.uid, &globalKey, 0);
#endif

  MFRC522::StatusCode status;
  memset(globalBuffer, 0, BUFFER_MAX_SECTORS*BUFFER_MAX_SECTOR_SIZE);
  for (int i=0; i<numSectors; i++)
  {
    if (i < 32) { /* Sectors 0..31 have 4 bytes each, so 128 bytes total */
      no_of_blocks = 4;
      firstBlock = i * no_of_blocks;
    }
    else if (i < 40) { /* Sectors 32-39 have 16 bytes each */
      no_of_blocks = 16;
      firstBlock = 128 + (i - 32) * no_of_blocks;
    }
    blockAddr = firstBlock + no_of_blocks - 1;
    status = globalRfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr, &globalKey, &globalRfid.uid);
    if ( (MFRC522::STATUS_OK == status) || globalSkipAuth )
    {
      for (int8_t blockOffset = no_of_blocks - 1; blockOffset >= 0; blockOffset--)
      {
        blockAddr = firstBlock + blockOffset;
        numRead = BUFFER_SIZE;
        byte *bufferAddr = globalBuffer[i] + blockOffset * BUFFER_BLOCK_SIZE;
#ifdef __DEBUG__
        error = 0;
#endif

        if (MFRC522::STATUS_OK == (status = globalRfid.MIFARE_Read(blockAddr, localBuffer, &numRead)))
        {
          memcpy(bufferAddr, localBuffer, BUFFER_BLOCK_SIZE);
        }
        else
        {
#ifdef __DEBUG__
          error = 1;
#endif
          Serial.print(F("ERROR: read: ")); Serial.println((int)status);
        }
      }
    }
    else
    {
#ifdef __DEBUG__
      error = 1;
#endif
      Serial.print(F("ERROR: auth: ")); Serial.println((int)status);
      continue;
    }
#ifdef __DEBUG__
    dumpBuffer(globalBuffer[i], no_of_blocks * BUFFER_BLOCK_SIZE);
#endif
  }
#ifdef __DEBUG__
  Serial.println(error);
#endif

#ifdef __DEBUG__
  Serial.println(F("PICC's UID:"));
  for (byte i = 0; i < globalRfid.uid.size; i++)
  {
    Serial.print(globalRfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(globalRfid.uid.uidByte[i], HEX);
  }
  Serial.println();
#endif

  globalRfid.PICC_HaltA(); /* done reading, set PICC also in this mode */
  globalRfid.PCD_StopCrypto1();
}

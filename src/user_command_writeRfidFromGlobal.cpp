
#include "globals.h"
#include "debug_commands.h"



void user_writeRfidFromGlobal(void)
{
  MFRC522::PICC_Type piccType;
  int numSectors = 0;
  byte numWrite = 0;
  byte blockAddr = 0;
  byte no_of_blocks = 0;
  byte firstBlock = 0;
  byte localBuffer[BUFFER_SIZE];
  byte *currentSector = NULL;
  byte *currentSectorLastByte = NULL;
  byte *currentSectorTrailer = NULL;
  uint64_t currentSectorKeyA = 0;
  uint64_t currentSectorKeyB = 0;
  uint32_t currentSectorAccessBits = 0;
  bool currentSectorValidTrailerAccess = false;

  (void)currentSectorKeyA;
  (void)currentSectorKeyB;
  (void)currentSectorAccessBits;

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
      numSectors = 16;
      break;
    case MFRC522::PICC_TYPE_MIFARE_4K:
      numSectors = 40;
      break;
    default:
      Serial.println(F("ERROR: Card type not yet supported!"));
      return;
      break;
  }

  MFRC522::StatusCode status;
  for (int i=0; i<numSectors; i++)
  {
    currentSector = globalBuffer[i];

    if (i < 32) { /* Sectors 0..31 have 4 bytes each, so 128 bytes total */
      no_of_blocks = 4;
      firstBlock = i * no_of_blocks;
    }
    else if (i < 40) { /* Sectors 32-39 have 16 bytes each */
      no_of_blocks = 16;
      firstBlock = 128 + (i - 32) * no_of_blocks;
    }
    currentSectorLastByte = currentSector + no_of_blocks * 16u - 1u;
    currentSectorTrailer = currentSectorLastByte - (16u - 1u);

    // overwrite Trailer for full access always!
    // TODO note possible improvement: this prevents a 1:1 copy by default, but also prevents breaking a RFID card by writing invalid access bits.
    const byte trailerBlockFix[BUFFER_BLOCK_SIZE] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // KeyA
                                                     0xFF, 0x07, 0x80, 0x69, // AccessBits
                                                     0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // KeyB
    memcpy(currentSectorTrailer, trailerBlockFix, sizeof(trailerBlockFix));

    currentSectorKeyA = ((uint64_t)currentSectorTrailer[5] <<  0u) |
                        ((uint64_t)currentSectorTrailer[4] <<  8u) |
                        ((uint64_t)currentSectorTrailer[3] << 16u) |
                        ((uint64_t)currentSectorTrailer[2] << 24u) |
                        ((uint64_t)currentSectorTrailer[1] << 32u) |
                        ((uint64_t)currentSectorTrailer[0] << 40u);
    currentSectorKeyB = ((uint64_t)currentSectorTrailer[10+5] <<  0u) |
                        ((uint64_t)currentSectorTrailer[10+4] <<  8u) |
                        ((uint64_t)currentSectorTrailer[10+3] << 16u) |
                        ((uint64_t)currentSectorTrailer[10+2] << 24u) |
                        ((uint64_t)currentSectorTrailer[10+1] << 32u) |
                        ((uint64_t)currentSectorTrailer[10+0] << 40u);
    currentSectorAccessBits = ((uint32_t)currentSectorTrailer[6+3] <<  0u) |
                              ((uint32_t)currentSectorTrailer[6+2] <<  8u) |
                              ((uint32_t)currentSectorTrailer[6+1] << 16u) |
                              ((uint32_t)currentSectorTrailer[6+0] << 24u);
    
    /* check if the trailer block is valid with valid authentication data, otherwise the card is dead(=unusable) when written */
    currentSectorValidTrailerAccess = dumpAndCheckAccessBitsDecoded(currentSectorTrailer+6, i, false);

    if (!currentSectorValidTrailerAccess)
    {
      /* this sector is not valid -> skip */
      Serial.print(F("ERROR: Invalid sector found! ("));
      Serial.print(i);
      Serial.println(F(") Skipping ..."));
      continue;
    }

#ifdef __DEBUG__
    Serial.print (F("Writing to sector ")); Serial.println(i);
    dumpBuffer(currentSector, no_of_blocks * BUFFER_BLOCK_SIZE);
#endif

    blockAddr = firstBlock + no_of_blocks - 1;
    status = globalRfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr, &globalKey, &globalRfid.uid);
    if ((MFRC522::STATUS_OK == status) || globalSkipAuth)
    {
      for (int8_t blockOffset = no_of_blocks - 1; blockOffset >= 0; blockOffset--)
      {
        blockAddr = firstBlock + blockOffset;
        numWrite = BUFFER_BLOCK_SIZE;
        byte *bufferAddr = currentSector + blockOffset * BUFFER_BLOCK_SIZE;
        memcpy(localBuffer, bufferAddr, numWrite);
        
        if (MFRC522::STATUS_OK == (status = globalRfid.MIFARE_Write(blockAddr, localBuffer, numWrite)))
        {
          memcpy(bufferAddr, localBuffer, BUFFER_BLOCK_SIZE);
        }
        else
        {
          Serial.print(F("ERROR: write: ")); Serial.println((int)status);
          break;
        }
      }
    }
    else
    {
      Serial.print(F("ERROR: auth: ")); Serial.println((int)status);
    }
  }

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

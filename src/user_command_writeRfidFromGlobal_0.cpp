
#include "globals.h"
#include "debug_commands.h"



void user_writeRfidFromGlobal_0(void)
{
  MFRC522::PICC_Type piccType;
  byte numWrite = 0;
  byte blockAddr = 0;
  byte no_of_blocks = 0;
  byte firstBlock = 0;
  byte localBuffer[BUFFER_SIZE];
#ifdef __DEBUG__
  byte error = 0;
#endif

  globalRfid.PICC_ReadCardSerial();

  Serial.print(F("\nPICC type: "));
  piccType = globalRfid.PICC_GetType(globalRfid.uid.sak);
  Serial.println(globalRfid.PICC_GetTypeName(piccType));

  if ((piccType != MFRC522::PICC_TYPE_MIFARE_MINI) &&
      (piccType != MFRC522::PICC_TYPE_MIFARE_1K  ) &&
      (piccType != MFRC522::PICC_TYPE_MIFARE_4K  )
     )
  {
    Serial.println(F("Not of type MIFARE Classic!"));
    return;
  }

#ifdef __DEBUG__
  dumpBuffer((byte*)globalBuffer_0, sizeof(globalBuffer_0));
#endif

  MFRC522::StatusCode status;
  no_of_blocks = 4;
  firstBlock = 0;
  blockAddr = firstBlock + 3;
  status = globalRfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr, &globalKey, &globalRfid.uid);
  if ((MFRC522::STATUS_OK == status) || globalSkipAuth)
  {
    for (int8_t blockOffset = no_of_blocks - 1; blockOffset >= 0; blockOffset--)
    {
      numWrite = BUFFER_BLOCK_SIZE;
      byte *bufferAddr = ((byte*)globalBuffer_0) + blockOffset * BUFFER_BLOCK_SIZE;
      memcpy(localBuffer, bufferAddr, numWrite);
      
      if (MFRC522::STATUS_OK == (status = globalRfid.MIFARE_Write(blockAddr, localBuffer, numWrite)))
      {
        memcpy(bufferAddr, localBuffer, BUFFER_BLOCK_SIZE);
      }
      else
      {
#ifdef __DEBUG__
        error = 1;
#endif
        Serial.print(F("ERROR write: ")); Serial.println((int)status);
      }
    }
  }
  else
  {
#ifdef __DEBUG__
    error = 1;
#endif
    Serial.print(F("ERROR auth: ")); Serial.println((int)status);
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

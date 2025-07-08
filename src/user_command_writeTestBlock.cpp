
#include "globals.h"
#include "debug_commands.h"



void user_writeTestBlock(void)
{
  MFRC522::PICC_Type piccType;
  byte numWrite = 0;
  byte blockAddr = 0;
  byte localBuffer[BUFFER_BLOCK_SIZE];

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
    Serial.println(F("Not of type MIFARE Classic!"));
    return;
  }

  MFRC522::StatusCode status;
  status = globalRfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr, &globalKey, &globalRfid.uid);
  if (MFRC522::STATUS_OK == status)
  {
    const byte exampleLocalBuffer[] = {1,2,3,4, 0,0,0,0, 5,6,7,8, 0,0,0,0};
    blockAddr = 0x01;
    memcpy(localBuffer, exampleLocalBuffer, 16u);
    numWrite = BUFFER_BLOCK_SIZE;

    if (MFRC522::STATUS_OK == (status = globalRfid.MIFARE_Write(blockAddr, localBuffer, numWrite)))
    {
      Serial.println(F("Write ok"));
    }
    else
    {
      Serial.print(F("ERROR write: ")); Serial.println((int)status);
    }
  }
  else
  {
    Serial.print(F("ERROR auth: ")); Serial.println((int)status);
  }

  globalRfid.PICC_HaltA(); /* done reading, set PICC also in this mode */
  globalRfid.PCD_StopCrypto1();
}


#include "globals.h"
#include "debug_commands.h"



void user_writeUnbrick(void)
{
  MFRC522::PICC_Type piccType;

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

  globalRfid.MIFARE_UnbrickUidSector(true);

  globalRfid.PICC_HaltA(); /* done reading, set PICC also in this mode */
  globalRfid.PCD_StopCrypto1();
}

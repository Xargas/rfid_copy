
#include "globals.h"
#include "debug_commands.h"



void user_setGlobalMemorySector(void)
{
  long sectorNum = 0;
  long blockNum = 0;

  Serial.println(F("Set Mem Sector"));
  
  Serial.print(F("Target Sector (0-"));  Serial.print(BUFFER_MAX_SECTORS); Serial.println(F("): "));
  while (!Serial.available()) ;
  String strSectorNum = Serial.readString();
  sectorNum = strSectorNum.toInt();

  Serial.print(F("Target Block (0-"));  Serial.print(BUFFER_MAX_SECTOR_SIZE / BUFFER_BLOCK_SIZE); Serial.println(F("): "));
  while (!Serial.available()) ;
  String strBlockNum = Serial.readString();
  blockNum = strBlockNum.toInt();

  Serial.print(F("Selected: ("));
  Serial.print(sectorNum);
  Serial.print(" - ");
  Serial.print(blockNum);
  Serial.println(F(")"));
  
  if (
       ( (0 <= sectorNum) && (sectorNum < (long)BUFFER_MAX_SECTORS) )
       &&
       ( (0 <= blockNum) && (blockNum < (long)(BUFFER_MAX_SECTOR_SIZE / BUFFER_BLOCK_SIZE)))
     )
  {
    byte *bufferStart = &globalBuffer[sectorNum][blockNum * BUFFER_BLOCK_SIZE];
    
    for (uint8_t n=0; n<BUFFER_BLOCK_SIZE; n++)
    {
      Serial.print(F("Byte "));
      Serial.print(n);
      Serial.println(F("(hex, without 0x prefix): "));
      while (!Serial.available()) ;
      String strHexVal = Serial.readString();
      int hexVal = strtol(strHexVal.c_str(), 0, 16);
      if ( (0 <= hexVal) && (hexVal <= 0xFF) )
      {
        bufferStart[n] = hexVal;
        Serial.print(hexVal, 16);
      }
    }
  }
}

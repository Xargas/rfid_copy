
#include "globals.h"
#include "debug_commands.h"



void user_printGlobalMemorySector(void)
{
  int sectorNum = 0;
  int blockNum = 0;

  Serial.println(F("Get Mem Sector"));
  
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
    dumpBuffer(bufferStart, BUFFER_BLOCK_SIZE);
  }
}

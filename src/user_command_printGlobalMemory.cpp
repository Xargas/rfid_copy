
#include "globals.h"
#include "debug_commands.h"



void user_printGlobalMemory(void)
{
  Serial.println(F("Get Mem"));
  for (uint8_t sectorNum=0u; sectorNum < (long)BUFFER_MAX_SECTORS; sectorNum++)
  {
    for (uint8_t blockNum=0u; blockNum < (long)(BUFFER_MAX_SECTOR_SIZE / BUFFER_BLOCK_SIZE); blockNum++)
    {
      byte *bufferStart = &globalBuffer[sectorNum][blockNum * BUFFER_BLOCK_SIZE];
      Serial.print(sectorNum); Serial.print(' '); Serial.println(blockNum);
      dumpBuffer(bufferStart, BUFFER_BLOCK_SIZE);
    }
  }
}

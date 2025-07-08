
#include <Arduino.h>

#include "debug_commands.h"



void dumpBuffer(byte *buffer, size_t bufferSize)
{
  for (size_t i=0; i<bufferSize; i++)
  {
    if (i%16 == 0) Serial.print(F("\n"));
    else if (i%4 == 0) Serial.print(F(" "));

    int tmp = (int)(buffer[i]);
    if (tmp < 0x10) Serial.print("0");
    Serial.print(tmp, HEX);
  }
  Serial.println();
}

bool dumpAndCheckAccessBitsDecoded(byte *accessBitsPtr, uint32_t sectorNum, bool dump)
{
  bool validAccessBits = false;
  /* 4 bytes are in accessBits */
  if (dump)
  {
    Serial.print(F("-------------------- Sector ")); Serial.print(sectorNum); Serial.println(F(" --------------------"));
    Serial.println(accessBitsPtr[0], 2u);
    Serial.println(accessBitsPtr[1], 2u);
    Serial.println(accessBitsPtr[2], 2u);
    Serial.println(F("---"));
  }

  byte c123bits[4] = {0u};          /* index is block index */
  byte c123bitsInverted[4] = {0u};  /* index is block index */

  byte c1,  c2,  c3;
  byte c1n, c2n, c3n;

  c1  = ((accessBitsPtr[1] & 0xF0) >> 4u);
  c2  = ((accessBitsPtr[2] & 0x0F) >> 0u);
  c3  = ((accessBitsPtr[2] & 0xF0) >> 4u);
  c1n = ((accessBitsPtr[0] & 0x0F) >> 0u);
  c2n = ((accessBitsPtr[0] & 0xF0) >> 4u);
  c3n = ((accessBitsPtr[1] & 0x0F) >> 0u);

  Serial.print(c1, 2u);  Serial.print(c2, 2u);  Serial.println(c3, 2u);
  Serial.print(c1n, 2u); Serial.print(c2n, 2u); Serial.println(c3n, 2u);
  
  validAccessBits = (c1 == ((~c1n) & 0xF)) &&
                    (c2 == ((~c2n) & 0xF)) &&
                    (c3 == ((~c3n) & 0xF));

  c123bits[0] = (((accessBitsPtr[2] & 0x10) >> 4u) << 2u) | /* c3, block 0 */
                (((accessBitsPtr[2] & 0x01) >> 0u) << 1u) | /* c2, block 0 */
                (((accessBitsPtr[1] & 0x10) >> 4u) << 0u);  /* c1, block 0 */
  c123bits[1] = (((accessBitsPtr[2] & 0x20) >> 5u) << 2u) | /* c3, block 1 */
                (((accessBitsPtr[2] & 0x02) >> 1u) << 1u) | /* c2, block 1 */
                (((accessBitsPtr[1] & 0x20) >> 5u) << 0u);  /* c1, block 1 */
  c123bits[2] = (((accessBitsPtr[2] & 0x40) >> 6u) << 2u) | /* c3, block 2 */
                (((accessBitsPtr[2] & 0x04) >> 2u) << 1u) | /* c2, block 2 */
                (((accessBitsPtr[1] & 0x40) >> 6u) << 0u);  /* c1, block 2 */
  c123bits[3] = (((accessBitsPtr[2] & 0x80) >> 7u) << 2u) | /* c3, block 3 */
                (((accessBitsPtr[2] & 0x08) >> 3u) << 1u) | /* c2, block 3 */
                (((accessBitsPtr[1] & 0x80) >> 7u) << 0u);  /* c1, block 3 */
  c123bitsInverted[0] = (((accessBitsPtr[1] & 0x01) >> 0u) << 2u) | /* c3, block 0 */
                        (((accessBitsPtr[0] & 0x10) >> 4u) << 1u) | /* c2, block 0 */
                        (((accessBitsPtr[0] & 0x01) >> 0u) << 0u);  /* c1, block 0 */
  c123bitsInverted[1] = (((accessBitsPtr[1] & 0x02) >> 1u) << 2u) | /* c3, block 1 */
                        (((accessBitsPtr[0] & 0x20) >> 5u) << 1u) | /* c2, block 1 */
                        (((accessBitsPtr[0] & 0x02) >> 1u) << 0u);  /* c1, block 1 */
  c123bitsInverted[2] = (((accessBitsPtr[1] & 0x04) >> 2u) << 2u) | /* c3, block 2 */
                        (((accessBitsPtr[0] & 0x40) >> 6u) << 1u) | /* c2, block 2 */
                        (((accessBitsPtr[0] & 0x04) >> 2u) << 0u);  /* c1, block 2 */
  c123bitsInverted[3] = (((accessBitsPtr[1] & 0x08) >> 3u) << 2u) | /* c3, block 3 */
                        (((accessBitsPtr[0] & 0x80) >> 7u) << 1u) | /* c2, block 3 */
                        (((accessBitsPtr[0] & 0x08) >> 3u) << 0u);  /* c1, block 3 */

  if (dump && (sectorNum < 32))
  {
    Serial.print(F("Affected Block 0: ")); Serial.print(c123bits[0], 2u); Serial.print(F(" ")); Serial.println(c123bitsInverted[0], 2u);
    Serial.print(F("Affected Block 1: ")); Serial.print(c123bits[1], 2u); Serial.print(F(" ")); Serial.println(c123bitsInverted[1], 2u);
    Serial.print(F("Affected Block 2: ")); Serial.print(c123bits[2], 2u); Serial.print(F(" ")); Serial.println(c123bitsInverted[2], 2u);
    Serial.print(F("Affected Block 3: ")); Serial.print(c123bits[3], 2u); Serial.print(F(" ")); Serial.print(c123bitsInverted[3], 2u); Serial.println(F("(Sector trailer)"));
    validAccessBits ? Serial.println(F("Valid")) : Serial.println(F("Invalid"));
  }
  else if (dump && (sectorNum >= 32))
  {
    Serial.println(F("Affected Block  0- 4: ")); Serial.print(c123bits[0], 2u); Serial.print(F(" ")); Serial.println(c123bitsInverted[0], 2u);
    Serial.println(F("Affected Block  5- 9: ")); Serial.print(c123bits[1], 2u); Serial.print(F(" ")); Serial.println(c123bitsInverted[1], 2u);
    Serial.println(F("Affected Block 10-14: ")); Serial.print(c123bits[2], 2u); Serial.print(F(" ")); Serial.println(c123bitsInverted[2], 2u);
    Serial.println(F("Affected Block    15: ")); Serial.print(c123bits[3], 2u); Serial.print(F(" ")); Serial.print(c123bitsInverted[3], 2u); Serial.println(F("(Sector trailer)"));
    validAccessBits ? Serial.println(F("Valid")) : Serial.println(F("Invalid"));
  }

  return validAccessBits;
}

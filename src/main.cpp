
//#define __DEBUG__

#include "globals.h"

#include "debug_commands.h"
#include "user_commands.h"



bool PICC_IsCardPresent(void);

void setup(void)
{
  Serial.begin(115200);
  while (!Serial) ;
  Serial.println("Starting:");
  SPI.begin();
  globalRfid.PCD_Init();
}

void loop(void)
{
  int cmd = 0;
  while (Serial.available()>0) {Serial.read(); delay(10);} /* remove possible old inputs */

  if (PICC_IsCardPresent())
  {
    Serial.println(F("Commands:\n\
      (1) Read this card to global buffer\n\
      (2) Write global buffer to card\n\
      (3) Write sector 0, block 0-3\n\
      (4) Unbrick card\n\
      (5) Write test block\n\
      (6) Print global buffer block\n\
      (7) Set global buffer block\n\
      (8) Print global buffer\n\
    "));
    while (Serial.available() <= 0) ; /* active wait for input */
    cmd = Serial.read();
    switch (cmd)
    {
      case '1': user_readRfidToGlobal(); break;
      case '2': user_writeRfidFromGlobal(); break;
      case '3': user_writeRfidFromGlobal_0(); break;
      case '4': user_writeUnbrick(); break;
      case '5': user_writeTestBlock(); break;
      case '6': user_printGlobalMemorySector(); break;
      case '7': user_setGlobalMemorySector(); break;
      case '8': user_printGlobalMemory(); break;
      default: break;
    }
  }
  delay(100);
}

/// @brief Checks if new card or previous card still on sensor
/// @return True if a card is present, false otherwise
bool PICC_IsCardPresent(void)
{
  byte buffer[2];
  byte bufferSize = sizeof(buffer);

  /* Reset baud rates */
  globalRfid.PCD_WriteRegister(MFRC522::PCD_Register::TxModeReg, 0x00);
  globalRfid.PCD_WriteRegister(MFRC522::PCD_Register::RxModeReg, 0x00);
  /* Reset ModWidthReg */
  globalRfid.PCD_WriteRegister(MFRC522::PCD_Register::ModWidthReg, 0x26);

  MFRC522::StatusCode result = globalRfid.PICC_WakeupA(buffer, &bufferSize);
  return (result == MFRC522::StatusCode::STATUS_OK || result == MFRC522::StatusCode::STATUS_COLLISION);
}

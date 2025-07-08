
#ifndef __DEBUG_COMMANDS_H__
#define __DEBUG_COMMANDS_H__

void dumpBuffer(byte *buffer, size_t bufferSize);
bool dumpAndCheckAccessBitsDecoded(byte *accessBitsPtr, uint32_t sectorNum, bool dump);

#endif

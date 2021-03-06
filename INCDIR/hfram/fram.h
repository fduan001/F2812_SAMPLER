#ifndef FRAM_H
#define FRAM_H


int FRAMInit(void);
UINT8 FRAMReadByte(UINT16 addr);
UINT8 FRAMReadSR(void);
int FRAMWriteByte(UINT16 addr, UINT8 val);
int FRAMReadBlock(UINT16 addr, UINT8* buffer, UINT16 length);
int FRAMWriteBlock(UINT16 addr, UINT8* buffer, UINT16 length);

#endif

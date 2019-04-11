#ifndef  FPGA_H
#define  FPGA_H

void WriteFpgaRegister( UINT32 regaddr, UINT16 regvalue);
UINT16 ReadFpgaRegister( UINT32 regaddr );
void WriteFpgaRegisterBit(UINT32 regaddr, UINT8 bitpos, UINT8 bitvalue);
UINT8 ReadFpgaRegisterBit(UINT32 regaddr, UINT8 bitpos, UINT8 bitvalue);

#endif
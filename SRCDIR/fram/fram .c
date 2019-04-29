#include "F2812_datatype.h"
#include "fram.h"
#include "host_spi.h"

// FeRAM command defines
#define FRAM_WREN        0x06    // Write enable
#define FRAM_WRITE       0x02    // Write command
#define FRAM_READ        0x03    // Read command
#define FRAM_RDSR        0x05    // Read Status Register

int FRAMInit(void) {
	return 0;
}

UINT8 FRAMReadByte(UINT16 addr) {
	UINT8 val[1];
	UINT8 tx_buf[3];
	
	/* build read buffer for tx */
	tx_buf[0] = FRAM_READ;
	tx_buf[1] = ((addr >> 8) & 0xFF);
	tx_buf[2] = (addr & 0xFF);

	HostSpiXfer(tx_buf, 3, val, 1);
	return val[0];
}

int FRAMWriteByte(UINT16 addr, UINT8 val) {
	UINT8 tx_buf[4];

	/* WREN has to be written prior to each write access to FRAM */
	tx_buf[0] = FRAM_WREN;
	HostSpiXfer(tx_buf, 1, NULL, 0);
	
	/* build read buffer for tx */
	tx_buf[0] = FRAM_READ;
	tx_buf[1] = ((addr >> 8) & 0xFF);
	tx_buf[2] = (addr & 0xFF);
	tx_buf[3] = val;

	HostSpiXfer(tx_buf, 4, NULL, 0);
	return 0;
}

int FRAMReadBlock(UINT16 addr, UINT8* buffer, UINT16 length) {
	/* TBD */
	return 0;
}

int FRAMWriteBlock(UINT16 addr, UINT8* buffer, UINT16 length) {
	/* TBD */
	return 0;
}


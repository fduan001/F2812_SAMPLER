#include "F2812_datatype.h"
#include "fram.h"
#include "host_spi.h"

// FeRAM command defines
#define FRAM_WREN        0x06    // Write enable
#define FRAM_WRITE       0x02    // Write command
#define FRAM_READ        0x03    // Read command
#define FRAM_RDSR        0x05    // Read Status Register

extern void FRAMWriteEnable(UINT8 onoff);

int FRAMInit(void) {
	HostSpiInit();
	return 0;
}

UINT8 FRAMReadByte(UINT16 addr) {
	UINT8 val[1];
	UINT8 tx_buf[3];
	
	/* build read buffer for tx */
	tx_buf[0] = FRAM_READ;
	tx_buf[1] = ((addr >> 8) & 0xFF);
	tx_buf[2] = (addr & 0xFF);

	HostSpiAssertCS();
	HostSpiXfer(tx_buf, 3, val, 1);
	HostSpiDeassertCS();
	return val[0];
}

UINT8 FRAMReadSR(void) {
	UINT8 val[1] = {0x0};
	UINT8 tx_buf[1];
	
	/* build read buffer for tx */
	tx_buf[0] = FRAM_RDSR;

	HostSpiAssertCS();
	HostSpiXfer(tx_buf, 1, val, 1);
	HostSpiDeassertCS();

	return val[0];
}

int FRAMWriteByte(UINT16 addr, UINT8 val) {
	UINT8 tx_buf[4];

	FRAMWriteEnable(1);
	HostSpiAssertCS();
	/* WREN has to be written prior to each write access to FRAM */
	tx_buf[0] = FRAM_WREN;

	HostSpiXfer(tx_buf, 1, NULL, 0);
	HostSpiDeassertCS();

	HostSpiAssertCS();
	
	/* build write buffer for tx */
	tx_buf[0] = FRAM_WRITE;
	tx_buf[1] = ((addr >> 8) & 0xFF);
	tx_buf[2] = (addr & 0xFF);
	tx_buf[3] = val;

	HostSpiXfer(tx_buf, 4, NULL, 0);
	HostSpiDeassertCS();
	FRAMWriteEnable(0);
	return 0;
}

int FRAMReadBlock(UINT16 addr, UINT8* buffer, UINT16 length) {
	/* TBD */
	UINT8 tx_buf[3];
	
	/* build read buffer for tx */
	tx_buf[0] = FRAM_READ;
	tx_buf[1] = ((addr >> 8) & 0xFF);
	tx_buf[2] = (addr & 0xFF);

	HostSpiAssertCS();
	HostSpiXfer(tx_buf, 3, buffer, length);
	HostSpiDeassertCS();
	return 0;
}

int FRAMWriteBlock(UINT16 addr, UINT8* buffer, UINT16 length) {
	UINT8 tx_buf[4];

	FRAMWriteEnable(1);
	HostSpiAssertCS();
	/* WREN has to be written prior to each write access to FRAM */
	tx_buf[0] = FRAM_WREN;
	HostSpiDeassertCS();

	HostSpiXfer(tx_buf, 1, NULL, 0);
	
	HostSpiAssertCS();
	
	/* build read buffer for tx */
	tx_buf[0] = FRAM_WRITE;
	tx_buf[1] = ((addr >> 8) & 0xFF);
	tx_buf[2] = (addr & 0xFF);

	HostSpiXfer(tx_buf, 3, NULL, 0);
	HostSpiXfer(buffer, length, NULL, 0);
	HostSpiDeassertCS();
	FRAMWriteEnable(0);
	return 0;
}


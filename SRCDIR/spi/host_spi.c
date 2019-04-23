#include "host_spi.h"

#include "DSP28_Device.h"
#include "boardcfg.h"
#include "F2812_datatype.h"

void HostSpiInit(void) {
	   UINT16 regval;

    // Initialize SPI-A:
    // Initialize SCI-A:
    EALLOW;
    regval = GpioMuxRegs.GPFMUX.all;
    regval |= SPI_PIN_MUX;
    GpioMuxRegs.GPFMUX.all = regval;
    EDIS;
    /* loopback   8 bit data */
    SpiaRegs.SPICCR.all = SPICCR_CFG;

    SpiaRegs.SPICTL.all = SPICTL_CFG;

    SpiaRegs.SPIBRR = SPIBRR_CFG;//128分频

    SpiaRegs.SPICCR.all = (0x80 | SPICCR_CFG);
	/* initialize the GPIO settings */
	/* set master mode, 8-bits mode */
}

void HostSpiAssertCS(void) {

}

void HostSpiDeassertCS(void) {

}

int HostSpiRead(UINT8* buffer, UINT16 length) {
	HostSpiAssertCS();
	HostSpiDeassertCS();
	return 0;
}

int HostSpiWrite(UINT8* buffer, UINT16 length) {
	HostSpiAssertCS();
	HostSpiDeassertCS();
	return 0;
}

int HostSpiXfer(UINT8* wrtBuf, UINT16 wrtLen, UINT8* rdBuf, UINT16 rdLen) {
	HostSpiAssertCS();
	HostSpiDeassertCS();
	return 0;
}


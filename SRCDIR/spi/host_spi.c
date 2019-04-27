#include "host_spi.h"

#include "DSP28_Device.h"
#include "boardcfg.h"
#include "F2812_datatype.h"

#define HOST_SPI_MAX_XFER_BYTES           16 /* 16 words per fifo */

typedef struct spi_msg_t {
	UINT8 tx_len;
	UINT8 rx_len;
	UINT8 *tx_buf;
	UINT8 *rx_buf;
} spi_msg_t;

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

    SpiaRegs.SPIBRR = SPIBRR_CFG;//

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

void HostSpiReset(void) {
	/* Disable SPI Master */
	SpiaRegs.SPICCR.all = 0x0;
	/* Disable FIFO and reset TX/RX FIFO */
	SpiaRegs.SPIFFTX.all = 0x0;
	SpiaRegs.SPIFFRX.all = 0x0;

	SpiaRegs.SPICCR.all = SPICCR_CFG;
	SpiaRegs.SPICTL.all = SPICTL_CFG;
    SpiaRegs.SPIBRR = SPIBRR_CFG;
    SpiaRegs.SPICCR.all = (0x80 | SPICCR_CFG);

    SpiaRegs.SPIFFRX.all = ( 1 << 13) | 0xF; /* no interrupt */
    SpiaRegs.SPIFFTX.all = ( 1 << 15) | (1 << 13);

	return ;
}

int HostSpiSetupXfer(spi_msg_t *spi_msg) {
	/* reset SPI controller */
	HostSpiReset();
	/* Setup the TX FIFO and RX FIFO */
	if( spi_msg->tx_len < HOST_SPI_MAX_XFER_BYTES ) {
		SpiaRegs.SPIFFTX.all |= spi_msg->tx_len;
	} else {
		SpiaRegs.SPIFFTX.all |= 0xF;
	}

	return 0;
}

int HostSpiXfer(spi_msg_t *spi_msg) {
	HostSpiAssertCS();
	HostSpiDeassertCS();
	return 0;
}



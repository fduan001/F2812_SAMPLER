#include "DSP28_Device.h"
#include "boardcfg.h"
#include "F2812_datatype.h"
#include "host_spi.h"
#include "shellconsole.h"

#define HOST_SPI_MAX_XFER_BYTES           16 /* 16 words per fifo */

extern void GpioSpiAssertCS(void);
extern void GpioSpiDeassertCS(void);
void HostSpiReset(void);

void HostSpiInit(void) {
	UINT16 regval;

    // Initialize SPI-A:
    // Initialize SCI-A:
	EALLOW;
	regval = GpioMuxRegs.GPFMUX.all;
	regval |= SPI_PIN_MUX;
	GpioMuxRegs.GPFMUX.all = regval;
	EDIS;

	HostSpiReset();
	/* initialize the GPIO settings */
	/* set master mode, 8-bits mode */
}

void HostSpiAssertCS(void) {
	GpioSpiAssertCS();
	PlatformDelay(10);

}

void HostSpiDeassertCS(void) {
	GpioSpiDeassertCS();
	PlatformDelay(10);
}

void HostSpiReset(void) {
   SpiaRegs.SPICCR.bit.RESET=0; // Reset SPI
   SpiaRegs.SPICCR.all=0x0007;       //8-bit character, no Loopback mode
   SpiaRegs.SPICTL.all=0x000e;       //Interrupt enabled, Master/Slave XMIT enabled
   SpiaRegs.SPISTS.all=0x0000;
   SpiaRegs.SPIBRR = SPIBRR_CFG;           // Baud rate
   SpiaRegs.SPIPRI.all=0x0010;
   SpiaRegs.SPICCR.bit.RESET=1;  // Enable SPI
   return ;
}

int IsMasterTXIdle(UINT32 timeout) {
	UINT32 count = 0;
	while( SpiaRegs.SPISTS.bit.BUFFULL_FLAG != 0 ) {
		++count;
		if( count > timeout ) {
			PRINTF("Master is busy\n");
			return 0;
		}
		PlatformDelay(1);
	}
	return 1;
}

int IsMasterRXReady(UINT32 timeout) {
	UINT32	count = 0;
	while( SpiaRegs.SPISTS.bit.INT_FLAG == 0 ) {
		++count;
		if( count > timeout ) {
			PRINTF("Master RX Ready timeout\n");
			return 0;
		}
		PlatformDelay(1);
	}
	return 1;
}

int HostSpiXferWrite(spi_msg_t *spi_msg) {
	int  i = 0;
	UINT16 val = 0;

	if( spi_msg->tx_len > 0 ) {
		spi_msg->tx_done = HOST_SPI_MSG_INP;
		for( i = 0; i < spi_msg->tx_len; ++i ) {
			if( IsMasterTXIdle(2000) != 1 ) {
				return -1;
			}
			val = (spi_msg->tx_buf[i] << 8) | 0x00;
			SpiaRegs.SPITXBUF = val;
			if( IsMasterRXReady(2000) != 1 ) {
				return -1;
			}

			val = SpiaRegs.SPIRXBUF;
			//PRINTF("%d: 0x%02x\n", i, spi_msg->tx_buf[i]);
		}
	}

	spi_msg->tx_done = HOST_SPI_MSG_DONE;
	return 0;
}

int HostSpiXferRead(spi_msg_t *spi_msg) {
	int i = 0;
	if( spi_msg->rx_len > 0 ) {
		spi_msg->rx_done = HOST_SPI_MSG_INP;
		for( i = 0; i < spi_msg->rx_len; ++i ) {
			if( IsMasterTXIdle(2000) != 1 ) {
				return -1;
			}
			SpiaRegs.SPITXBUF = 0x0000; /* dummy byte */
			if( IsMasterRXReady(2000) != 1 ) {
				return -1;
			}
			spi_msg->rx_buf[i] = (UINT8)(SpiaRegs.SPIRXBUF & 0xFF);
			//PRINTF("read 0x%02x\n", spi_msg->rx_buf[i]);
		}
	}

	spi_msg->rx_done = HOST_SPI_MSG_DONE;
	return 0;
}

int HostSpiSetupXfer(spi_msg_t *spi_msg) {
	int i = 0;
	int loop = 0;
	int remain = 0;
	spi_msg_t cur_msg;

	if( spi_msg->tx_buf ) {
		loop = spi_msg->tx_len / HOST_SPI_MAX_XFER_BYTES;
		remain = spi_msg->tx_len % HOST_SPI_MAX_XFER_BYTES;
		/* tx 1st then rx */
		for( i = 0; i < loop; ++i ) {
			cur_msg.tx_len = HOST_SPI_MAX_XFER_BYTES;
			cur_msg.tx_done = HOST_SPI_MSG_INIT;
			cur_msg.tx_buf = spi_msg->tx_buf + (i * HOST_SPI_MAX_XFER_BYTES);
			HostSpiXferWrite(&cur_msg);
		}

		if( remain > 0 ) {
			cur_msg.tx_len = remain;
			cur_msg.tx_done = HOST_SPI_MSG_INIT;
			cur_msg.tx_buf = spi_msg->tx_buf + (i * HOST_SPI_MAX_XFER_BYTES);
			HostSpiXferWrite(&cur_msg);
		}
	}
	
	if( spi_msg->rx_buf ) {
		loop = spi_msg->rx_len / HOST_SPI_MAX_XFER_BYTES;
		remain = spi_msg->rx_len % HOST_SPI_MAX_XFER_BYTES;

		/* tx 1st then rx */
		for( i = 0; i < loop; ++i ) {
			cur_msg.rx_len = HOST_SPI_MAX_XFER_BYTES;
			cur_msg.rx_done = HOST_SPI_MSG_INIT;
			cur_msg.rx_buf = spi_msg->rx_buf + (i * HOST_SPI_MAX_XFER_BYTES);
			HostSpiXferRead(&cur_msg);
		}

		if( remain > 0 ) {
			cur_msg.rx_len = remain;
			cur_msg.rx_done = HOST_SPI_MSG_INIT;
			cur_msg.rx_buf = spi_msg->rx_buf + (i * HOST_SPI_MAX_XFER_BYTES);
			HostSpiXferRead(&cur_msg);
		}
	}
	return 0;
}

int HostSpiXferMsg(spi_msg_t *spi_msg) {
	HostSpiSetupXfer(spi_msg);
	return 0;
}

int HostSpiXfer(UINT8* tx_buf, UINT16 tx_len, UINT8* rx_buf, UINT16 rx_len) {
	spi_msg_t cur_msg;
	cur_msg.tx_len = tx_len;
	cur_msg.tx_buf = tx_buf;
	cur_msg.rx_len = rx_len;
	cur_msg.rx_buf = rx_buf;
	HostSpiXferMsg(&cur_msg);
	return 0;
}


int HostSpiRead(UINT8* buffer, UINT16 length) {
	HostSpiXfer(NULL, 0, buffer, length);
	return 0;
}

int HostSpiWrite(UINT8* buffer, UINT16 length) {
	HostSpiXfer(buffer, length, NULL, 0);
	return 0;
}



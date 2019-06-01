#include "F2812_datatype.h"
#include "spi.h"
#include "fpga_spi.h"
#include "fpga.h"
#include "shellconsole.h"

//fpga spi bit
#define FPGA_SPI_BASE  (0x80000 + 0x20)
#define FPGA_SPI1_RESET_BIT    2
#define FPGA_SPI2_RESET_BIT    3

#define BSP_DRV_OK  (1)
#define BSP_DRV_FAIL (0)

#define FPGA_SPI_CTL_WRDIR_BIT15         (15)
//1 for write , 0 for read only used for 3wire
#define FPGA_SPI_CTL_WIREMODE_BIT14  (14)
#define FPGA_SPI_CTL_ASS_BIT13 (13)
#define FPGA_SPI_CTL_IE_BIT12   (12)
#define FPGA_SPI_CTL_LSB_BIT11 (11)
#define FPGA_SPI_CTL_TX_NEG_BIT10   (10)
#define FPGA_SPI_CTL_RX_NEG_BIT9    (9)
#define FPGA_SPI_CTL_GO_BSY_BIT8    (8)

#define FPGA_SPI_TIMEOUT   10000 /* 10 ms */

typedef struct fpga_spi_cfg_t {
    UINT32 trx; /* tx and rx data register */
    UINT32 csr; /* ctrl status register */
    UINT32 divider; /* freq settings */
    UINT32 cs; /* chip select */
} fpga_spi_cfg_t;

#define MAX_FPGA_SPI_CHAN            2

static fpga_spi_cfg_t g_fpga_spi_cfg[2];

UINT8 SYSSPI_RESET_BIT[MAX_FPGA_SPI_CHAN] = {
    FPGA_SPI1_RESET_BIT,
    FPGA_SPI2_RESET_BIT,
};

void InitFpgaSpiData(void) {
    g_fpga_spi_cfg[0].trx = FPGA_SPI_BASE;
    g_fpga_spi_cfg[0].csr = FPGA_SPI_BASE + 1;
    g_fpga_spi_cfg[0].divider = FPGA_SPI_BASE + 2;
    g_fpga_spi_cfg[0].cs = FPGA_SPI_BASE + 3;

    g_fpga_spi_cfg[1].trx = FPGA_SPI_BASE + 0x20;
    g_fpga_spi_cfg[1].csr = FPGA_SPI_BASE + 0x20 + 1;
    g_fpga_spi_cfg[1].divider = FPGA_SPI_BASE + 0x20 + 2;
    g_fpga_spi_cfg[1].cs = FPGA_SPI_BASE + 0x20 + 3;
}

UINT8 Chan2RstBitPos(UINT8 chan) {
    return SYSSPI_RESET_BIT[chan];
}

void FpgaSpiMasterReset(UINT8 chan) {
    UINT8 bit_pos = 0;
    if( chan > MAX_FPGA_SPI_CHAN ) {
        return;
    }

    bit_pos = Chan2RstBitPos(chan);

    FPGA_REG16_W(FPGA_PERIPHERAL_RST_REG, FPGA_REG16_R(FPGA_PERIPHERAL_RST_REG) | (1 << bit_pos)); // MR = HI;
    PlatformDelay(5); 

    FPGA_REG16_W(FPGA_PERIPHERAL_RST_REG, FPGA_REG16_R(FPGA_PERIPHERAL_RST_REG) & (~(1 << bit_pos))); // MR = LO;
    PlatformDelay(5);

    FPGA_REG16_W(FPGA_PERIPHERAL_RST_REG, FPGA_REG16_R(FPGA_PERIPHERAL_RST_REG) | (1 << bit_pos)); // MR = HI;
    PlatformDelay(5); 
}

static inline void __msleep__(UINT32 ms)
{
	PlatformDelay(ms * 1000);
}

INT32 IsMasterIdle(UINT8 chan, UINT32 timeout);

/******************************************************************************
 *
 * Function:		FpgaSpiConfig
 *
 * Description:       This function config the spi controller using the paramter
 *
 * Parameters:	channel:  uart channel number
                             commconfig:  the uart configuration parameter
 *
 *
 * Return Value:

 *
 *****************************************************************************/
INT32 FpgaSpiConfig(UINT8 chan , S_SPI_CFG_TYPE spicfg)
{
    UINT32 ref_clk = 150000;
    UINT16 val = 0;
    UINT16 regdata;

    FpgaSpiMasterReset(chan);
    if( IsMasterIdle(chan, FPGA_SPI_TIMEOUT) == 0 ) {
        return BSP_DRV_FAIL;
    }

    //set the  clock  rate;

    val = (ref_clk / spicfg.spisclk) / 2 - 1;
    FPGA_REG16_W(g_fpga_spi_cfg[chan].divider, val);

    regdata = FPGA_REG16_R(g_fpga_spi_cfg[chan].divider);
    PRINTF("divider=0x%04x  spisclk=%lu 0x%08lx\n", regdata, spicfg.spisclk, g_fpga_spi_cfg[chan].divider);
    //set the spi timing protocol ,default is read mode
    regdata = 0;
    //set the cs generate manually
    regdata = FPGA_SET_BITFIELD(regdata, spicfg.spicsgen, FPGA_SPI_CTL_ASS_BIT13, FPGA_SPI_CTL_ASS_BIT13);
    //set the bit order;
    regdata = FPGA_SET_BITFIELD(regdata, spicfg.bitorder, FPGA_SPI_CTL_LSB_BIT11, FPGA_SPI_CTL_LSB_BIT11);
    //set the tx data timing
    regdata = FPGA_SET_BITFIELD(regdata, spicfg.spitxdatrising, FPGA_SPI_CTL_TX_NEG_BIT10, FPGA_SPI_CTL_TX_NEG_BIT10);
    //set the rx data timing
    regdata = FPGA_SET_BITFIELD(regdata, spicfg.spirxdatrising, FPGA_SPI_CTL_RX_NEG_BIT9, FPGA_SPI_CTL_RX_NEG_BIT9);
    //set the data length
    regdata = FPGA_SET_BITFIELD(regdata, spicfg.spidatalen, 6, 0);
    //set 3wire or 4wire spi mode
    regdata = FPGA_SET_BITFIELD(regdata, spicfg.wiremode, FPGA_SPI_CTL_WIREMODE_BIT14 , FPGA_SPI_CTL_WIREMODE_BIT14);
    FPGA_REG16_W(g_fpga_spi_cfg[chan].csr, regdata);

    regdata = FPGA_REG16_R(g_fpga_spi_cfg[chan].csr);
    PRINTF("Addr: 0x%08lx CSR=0x%04x\n", g_fpga_spi_cfg[chan].csr, regdata);

    __msleep__(1);
    
    return BSP_DRV_OK;
}

INT32 IsMasterIdle(UINT8 chan, UINT32 timeout) {
    UINT16 regdata;
    UINT16 bitvalue;

    while(1) {
    	regdata = FPGA_REG16_R(g_fpga_spi_cfg[chan].csr);
        bitvalue = FPGA_READ_BITFIELD(regdata, FPGA_SPI_CTL_GO_BSY_BIT8, FPGA_SPI_CTL_GO_BSY_BIT8);
        if( SPI_GO_START != bitvalue ) {
        	return 1; /* master is idle */
        }
        PlatformDelay(1);
        timeout--;
        if(0 == timeout) {
        	PRINTF("master is not idle timeout check\n");
            return 0;
        }
    }

    return 1;
}

void FpgaSpiTransChar(UINT8 chan, UINT8 obj) {
    UINT16 regdata;
    FPGA_REG16_W(g_fpga_spi_cfg[chan].trx, obj);
    //PRINTF("DIN: 0x%02x\n", obj);
    regdata = FPGA_REG16_R(g_fpga_spi_cfg[chan].csr);
    regdata = FPGA_SET_BITFIELD(regdata, SPI_GO_START, FPGA_SPI_CTL_GO_BSY_BIT8, FPGA_SPI_CTL_GO_BSY_BIT8);
    FPGA_REG16_W(g_fpga_spi_cfg[chan].csr, regdata);
}

/******************************************************************************
 *
 * Function:		FpgaSpiWrite
 *
 * Description:       This function send all the data to spi
 *
 * Parameters:	channel:  spi channel number
                             sendbuffer:  send buffer
                             sendlen:      send buffer length( byte)
 *
 *
 * Return Value:

 *
 *****************************************************************************/

INT32 FpgaSpiWrite(UINT8 chan , UINT8 *sendbuffer, UINT8 sendlen)
{
    UINT8  indx = 0;
    //write the data byte by byte
    for(indx = 0; indx < sendlen; ++indx ) {
        if( IsMasterIdle(chan, FPGA_SPI_TIMEOUT) == 0 ) {
            return BSP_DRV_FAIL;
        }
        //send one byte
        FpgaSpiTransChar(chan, sendbuffer[indx]);
        //PRINTF("DIN %u: 0x%02x\n", indx, sendbuffer[indx]);
    }
    return BSP_DRV_OK;
}

/******************************************************************************
 *
 * Function:		FpgaSpiWriteRead
 *
 * Description:       This function send all the command to spi and read the
                             data;
 *
 * Parameters:	channel:  spi channel number
                             sendbuffer:  send buffer
                             sendlen:      send buffer length( byte)
                             readbuffer :   read buffer;
                             readlen   :    real length
 *
 *
 * Return Value:

 *
 *****************************************************************************/

INT32 FpgaSpiWriteRead(UINT8 chan , UINT8 *sendbuffer, UINT8 sendlen , UINT8 *readbuffer , UINT8 readlen  )
{
    UINT8  indx;

    if( sendbuffer ) {
        //write the command data byte by byte
        for(indx = 0; indx < sendlen; indx++) {
            //check ready 1st
            if( IsMasterIdle(chan, FPGA_SPI_TIMEOUT) == 0 ) {
                return BSP_DRV_FAIL;
            }
            //send one byte
            FpgaSpiTransChar(chan, sendbuffer[indx]);
        }
    }

    if( IsMasterIdle(chan, FPGA_SPI_TIMEOUT) == 0 ) {
        return BSP_DRV_FAIL;
    }

    if( readbuffer ) {
        // now to send the clock to read buffer;
        for(indx = 0; indx < readlen; indx++) {
            //send one byte
            FpgaSpiTransChar(chan, 0);
            //check ready for next;
            if( IsMasterIdle(chan, FPGA_SPI_TIMEOUT) == 0 ) {
                return BSP_DRV_FAIL;
            }
            readbuffer[indx] = (UINT8)(FPGA_REG16_R(g_fpga_spi_cfg[chan].trx) & 0xFF);
            // PRINTF("DOUT: 0x%02x\n", readbuffer[indx]);
        }
    }
    return BSP_DRV_OK;

}

/******************************************************************************
 *
 * Function:		FpgaSpiCs
 *
 * Description:       This function set the spi chip select
 *
 * Parameters:	channel:  spi channel number
                              chipsel:   chip select 0 or 1
 *
 *
 * Return Value:

 *
 *****************************************************************************/

void FpgaSpiCs(UINT8 chan, UINT8 chipsel)
{
    FPGA_REG16_W(g_fpga_spi_cfg[chan].cs, chipsel);
}


/******************************************************************************
 *
 * Function:		FpgaSpiRead
 *
 * Description:       This function send all the data to spi
 *
 * Parameters:	channel:  spi channel number
                             sendbuffer:  send buffer
                             sendlen:      send buffer length( byte)
 *
 *
 * Return Value:

 *
 *****************************************************************************/

INT32 FpgaSpiRead(UINT8 chan, UINT8 *readbuffer, UINT8 readlen)
{
    UINT8   indx;
    //first check the go_bsy=0?
    if( IsMasterIdle(chan, FPGA_SPI_TIMEOUT) == 0 ) {
        return BSP_DRV_FAIL;
    }
    
    //write the data byte by byte
    for(indx = 0; indx < readlen; indx++) {
        //send one byte
        FpgaSpiTransChar(chan, 0);
        //check ready for next;
        if( IsMasterIdle(chan, FPGA_SPI_TIMEOUT) == 0 ) {
            return BSP_DRV_FAIL;
        }
        readbuffer[indx] = (UINT8)(FPGA_REG16_R(g_fpga_spi_cfg[chan].trx) & 0xFF);
        PRINTF("%u: 0x%02x\n", indx, readbuffer[indx]);
    }
    return BSP_DRV_OK;
}

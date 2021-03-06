#include "F2812_datatype.h"
#include "DSP28_Device.h"
#include "F2812_gpio.h"

#define GPIO_MATE_CARD_IN0       (10)
#define GPIO_MATE_CARD_IN1       (11)
#define GPIO_MATE_CARD_IN2       (12)

#define GPIO_MATE_CARD_OUT0       (0)
#define GPIO_MATE_CARD_OUT1       (1)
#define GPIO_MATE_CARD_OUT2       (2)

void GpioInit(void) {
	/* GPIOF10 input, role */
	/* GPIOF11 input, role */
	/* GPIOF12 input, role */
	/* GPIOB0 output, role */
	/* GPIOB1 output, role */
	/* GPIOB2 output, role */

	EALLOW;
	GpioMuxRegs.GPFMUX.bit.MFSXA_GPIOF10 = 0;
	GpioMuxRegs.GPFMUX.bit.MFSRA_GPIOF11 = 0;
	GpioMuxRegs.GPFMUX.bit.MDXA_GPIOF12 = 0;
	GpioMuxRegs.GPFDIR.bit.GPIOF10 = GPIO_DIR_INPUT;
	GpioMuxRegs.GPFDIR.bit.GPIOF10 = GPIO_DIR_INPUT;
	GpioMuxRegs.GPFDIR.bit.GPIOF10 = GPIO_DIR_INPUT;

	GpioMuxRegs.GPBMUX.bit.PWM7_GPIOB0 = 0;
	GpioMuxRegs.GPBMUX.bit.PWM8_GPIOB1 = 0;
	GpioMuxRegs.GPBMUX.bit.PWM9_GPIOB2 = 0;
	GpioMuxRegs.GPBDIR.bit.GPIOB0 = GPIO_DIR_OUTPUT;
	GpioMuxRegs.GPBDIR.bit.GPIOB1 = GPIO_DIR_OUTPUT;
	GpioMuxRegs.GPBDIR.bit.GPIOB2 = GPIO_DIR_OUTPUT;

	GpioDataRegs.GPBCLEAR.bit.GPIOB0 = 1;
	GpioDataRegs.GPBCLEAR.bit.GPIOB1 = 1;
	GpioDataRegs.GPBCLEAR.bit.GPIOB2 = 1;

	GpioMuxRegs.GPAMUX.bit.C1TRIP_GPIOA13 = 0;
	GpioMuxRegs.GPAMUX.bit.C2TRIP_GPIOA14 = 0;
	GpioMuxRegs.GPAMUX.bit.C3TRIP_GPIOA15 = 0;

	GpioMuxRegs.GPADIR.bit.GPIOA13 = GPIO_DIR_OUTPUT;
	GpioMuxRegs.GPADIR.bit.GPIOA14 = GPIO_DIR_OUTPUT;
	GpioMuxRegs.GPADIR.bit.GPIOA15 = GPIO_DIR_INPUT;

	GpioMuxRegs.GPFMUX.bit.SPISTEA_GPIOF3 = 0;
	GpioMuxRegs.GPFDIR.bit.GPIOF3 = GPIO_DIR_OUTPUT;

	GpioDataRegs.GPFSET.bit.GPIOF3 = 1;

	GpioDataRegs.GPASET.bit.GPIOA14 = 1;

	EDIS;
}

void FRAMWriteEnable(UINT8 onoff) {
	if( onoff ) {
		GpioDataRegs.GPASET.bit.GPIOA13 = 1;
	} else {
		GpioDataRegs.GPACLEAR.bit.GPIOA13 = 1;
	}
}

void GpioSpiAssertCS(void) {
	GpioDataRegs.GPFCLEAR.bit.GPIOF3 = 1;
}

void GpioSpiDeassertCS(void) {
	GpioDataRegs.GPFSET.bit.GPIOF3 = 1;
}

void GpioMateCardSet(UINT8 pin, UINT8 ops) {
	switch(pin) {
		case 0:
		if( ops ) {
			GpioDataRegs.GPFSET.bit.GPIOF10 = 1;
		} else {
			GpioDataRegs.GPFCLEAR.bit.GPIOF10 = 1;
		}
		break;
		case 1:
		if( ops ) {
			GpioDataRegs.GPFSET.bit.GPIOF11 = 1;
		} else {
			GpioDataRegs.GPFCLEAR.bit.GPIOF11 = 1;
		}
		break;
		case 2:
		if( ops ) {
			GpioDataRegs.GPFSET.bit.GPIOF12 = 1;
		} else {
			GpioDataRegs.GPFCLEAR.bit.GPIOF12 = 1;
		}
		break;
	}
	return ;
}

UINT8 GpioMateCardGet(UINT8 pin) {
	switch(pin) {
		case 0:
			return GpioDataRegs.GPBDAT.bit.GPIOB0;
		case 1:
			return GpioDataRegs.GPBDAT.bit.GPIOB1;
		case 2:
			return GpioDataRegs.GPBDAT.bit.GPIOB2;
	}

	return 0;
}

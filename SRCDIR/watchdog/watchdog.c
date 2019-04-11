#include "DSP28_Device.h"
#include "F2812_datatype.h"
#include "shellconsole.h"
#include "F2812_gpio.h"
#include "watchdog.h"
#include "boardcfg.h"
#include "util.h"

#pragma DATA_SECTION   (g_wdog_counter,  "shell_lib");
static UINT16 g_wdog_counter = 0;

void WatchdogInit(void)
{
	EALLOW;
	GpioMuxRegs.GPAMUX.bit.CAP1Q1_GPIOA8 = 0;
	GpioMuxRegs.GPAMUX.bit.CAP2Q2_GPIOA9 = 0;
	GpioMuxRegs.GPADIR.bit.GPIOA8 = GPIO_DIR_OUTPUT;
	GpioMuxRegs.GPADIR.bit.GPIOA9 = GPIO_DIR_OUTPUT;
	EDIS;
}

void WatchdogEnable(void)
{
	GpioDataRegs.GPACLEAR.bit.GPIOA9 = 1;
}

void WatchdogDisable(void)
{
	GpioDataRegs.GPASET.bit.GPIOA9 = 1;
}

void WatchdogKick(void)
{
	++g_wdog_counter;
	if( (g_wdog_counter % 2) == 0 ) {
		GpioDataRegs.GPACLEAR.bit.GPIOA8 = 1;
	} else {
		GpioDataRegs.GPASET.bit.GPIOA8 = 1;
	}
}

void WatchdogShow(void)
{
	shellprintf("counter=0x%x\n", g_wdog_counter);
}

void WatchdogTask(void)
{
	// WatchdogInit();
	while(1) {
		WatchdogKick();
		Osal_TaskSleep(300);
	}
}

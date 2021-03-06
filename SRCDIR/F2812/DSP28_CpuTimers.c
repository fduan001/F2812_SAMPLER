//
//      TMDX ALPHA RELEASE
//      Intended for product evaluation purposes
//
//###########################################################################
//
// FILE:	DSP28_CpuTimers.c
//
// TITLE:	DSP28 CPU 32-bit Timers Initialization & Support Functions.
//
//###########################################################################
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  0.55| 06 May 2002 | L.H. | EzDSP Alpha Release
//  0.56| 20 May 2002 | L.H. | No change
//  0.57| 27 May 2002 | L.H. | No change
//###########################################################################

#include "F2812_datatype.h"
#include "DSP28_Device.h"
#include "DSP28_CpuTimers.h"
#include "boardcfg.h"

#pragma DATA_SECTION   (CpuTimer0,"shell_lib");
#pragma DATA_SECTION   (CpuTimer1,"shell_lib");
#pragma DATA_SECTION   (CpuTimer2,"shell_lib");

struct CPUTIMER_VARS CpuTimer0;
struct CPUTIMER_VARS CpuTimer1;
struct CPUTIMER_VARS CpuTimer2;

//---------------------------------------------------------------------------
// InitCpuTimers:
//---------------------------------------------------------------------------
// This function initializes all three CPU timers to a known state.
//
void InitCpuTimers(void)
{
    // Initialize address pointers to respective timer registers:
    CpuTimer0.RegsAddr = &CpuTimer0Regs;
    CpuTimer1.RegsAddr = &CpuTimer1Regs;
    CpuTimer2.RegsAddr = &CpuTimer2Regs;

    // Initialize timer period to maximum:
    CpuTimer0Regs.PRD.all  = 0xFFFFFFFF;
    CpuTimer1Regs.PRD.all  = 0xFFFFFFFF;
    CpuTimer2Regs.PRD.all  = 0xFFFFFFFF;

    // Initialize pre-scale counter to divide by 1 (SYSCLKOUT):
    CpuTimer0Regs.TPR.all  = 0;
    CpuTimer0Regs.TPRH.all = 0;

    // Make sure timers are stopped:
    CpuTimer0Regs.TCR.bit.TSS = 1;
    CpuTimer1Regs.TCR.bit.TSS = 1;
    CpuTimer2Regs.TCR.bit.TSS = 1;

    // Reload all counter registers with period values:
    CpuTimer0Regs.TCR.bit.TRB = 1;
    CpuTimer1Regs.TCR.bit.TRB = 1;
    CpuTimer2Regs.TCR.bit.TRB = 1;

    // Reset interrupt counters:
    CpuTimer0.InterruptCount = 0;
    CpuTimer1.InterruptCount = 0;
    CpuTimer2.InterruptCount = 0;
}

void InitCpuTimersBase(void)
{
    // Initialize address pointers to respective timer registers:
    CpuTimer0.RegsAddr = &CpuTimer0Regs;
    CpuTimer1.RegsAddr = &CpuTimer1Regs;
    CpuTimer2.RegsAddr = &CpuTimer2Regs;
}

//---------------------------------------------------------------------------
// ConfigCpuTimer:
//---------------------------------------------------------------------------
// This function initializes the selected timer to the period specified
// by the "Freq" and "Period" parameters. The "Freq" is entered as "MHz"
// and the period in "uSeconds". The timer is held in the stopped state
// after configuration.
//
void ConfigCpuTimer(struct CPUTIMER_VARS *Timer, float Freq, float Period)
{
    UINT32 	temp;

    // Initialize timer period:
    Timer->CPUFreqInMHz = Freq;
    Timer->PeriodInUSec = Period;
    temp = (UINT32) (Freq * Period);
    Timer->RegsAddr->PRD.all = temp;

    // Set pre-scale counter to divide by 1 (SYSCLKOUT):
    Timer->RegsAddr->TPR.all  = 0;
    Timer->RegsAddr->TPRH.all  = 0;

    // Initialize timer control register:
    Timer->RegsAddr->TCR.bit.POL = 0;      // 0 = Pulse Low
    Timer->RegsAddr->TCR.bit.TOG =	0;     // 0 = No Toggle, POL bit defines action
    Timer->RegsAddr->TCR.bit.TSS = 1;      // 1 = Stop timer, 0 = Start/Restart Timer
    Timer->RegsAddr->TCR.bit.TRB = 1;      // 1 = reload timer
    Timer->RegsAddr->TCR.bit.FRCEN = 0;    // Force output enable (not used)
    Timer->RegsAddr->TCR.bit.PWIDTH = 7;   // 7+1 = 8 SYSCLKOUT cycle pulse width
    Timer->RegsAddr->TCR.bit.SOFT = 1;
    Timer->RegsAddr->TCR.bit.FREE = 1;     // Timer Free Run
    Timer->RegsAddr->TCR.bit.TIE = 1;      // 0 = Disable/ 1 = Enable Timer Interrupt

    // Reset interrupt counter:
    Timer->InterruptCount = 0;
}

// yexin udelay counter ,1us base
void ConfigCpuTimerCntBase(struct CPUTIMER_VARS *Timer, float Freq, float Period )
{
    UINT32 	temp;

    // Initialize timer period:
    Timer->CPUFreqInMHz = Freq;
    Timer->PeriodInUSec = Period;
    //temp = (long) (Freq * Period);
    //Freq unit is MHZ,150MHZ*1US=150
    temp = (Freq/1000000);
    Timer->RegsAddr->PRD.all = Period;

    // Set pre-scale counter to divide by  (SYSCLKOUT/1us):
    Timer->RegsAddr->TPR.all  = temp&0xffff;
    Timer->RegsAddr->TPRH.all  = (temp>>16)&0xffff;

    // Initialize timer control register:
    Timer->RegsAddr->TCR.bit.POL = 0;      // 0 = Pulse Low
    Timer->RegsAddr->TCR.bit.TOG =	0;     // 0 = No Toggle, POL bit defines action
    Timer->RegsAddr->TCR.bit.TSS = 1;      // 1 = Stop timer, 0 = Start/Restart Timer
    Timer->RegsAddr->TCR.bit.TRB = 1;      // 1 = reload timer
    Timer->RegsAddr->TCR.bit.FRCEN = 0;    // Force output enable (not used)
    Timer->RegsAddr->TCR.bit.PWIDTH = 7;   // 7+1 = 8 SYSCLKOUT cycle pulse width
    Timer->RegsAddr->TCR.bit.SOFT = 1;
    Timer->RegsAddr->TCR.bit.FREE = 1;     // Timer Free Run
    //disable interrupt ,only for time base count
	Timer->RegsAddr->TCR.bit.TIE = 0;      // 0 = Disable/ 1 = Enable Timer Interrupt

    // Reset interrupt counter:
    Timer->InterruptCount = 0;
}


UINT32 GetCpuTimer2Cnt ()
{
    return  (CpuTimer2.InterruptCount);
}


UINT32 GetCpuTimerUsCnt(struct CPUTIMER_VARS *Timer)
{
	return (Timer->RegsAddr->TIM.all);
}

//timer 2 is used for cpu us delay counter
void SetCpuUsTimer2()
{
 	InitCpuTimersBase();
   	ConfigCpuTimerCntBase(&CpuTimer2,CPU_FREQ,CPU_US_TIMER_MAXCNT);
    	StartCpuTimer2();	
}

//delay unit is us
void UDelay(UINT16 delay)
{
	UINT32 timercnt1;
	UINT32 diffvalue=0;
    timercnt1=0;
	//if time is 0 ,skip it,wait the TIM reload
	while(0==timercnt1)
		timercnt1=GetCpuTimerUsCnt(&CpuTimer2);
	
	while(diffvalue<delay)
	{
		diffvalue=timercnt1-GetCpuTimerUsCnt(&CpuTimer2);
	}
}


void PlatformDelay (UINT16 usec)
{

	UINT32 timercnt1;
	UINT32 diffvalue=0;
	timercnt1=0;
	//if time is 0 ,skip it,wait the TIM reload
	while(0==timercnt1)
		timercnt1=GetCpuTimerUsCnt(&CpuTimer2);

	while(diffvalue<usec)
	{
		diffvalue=timercnt1-GetCpuTimerUsCnt(&CpuTimer2);
	}

}



UINT32 GetTimer2Cnt ()
{
    return  (CpuTimer2.InterruptCount);
}


//===========================================================================
// No more.
//===========================================================================

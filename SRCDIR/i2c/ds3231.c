/********************************************************************************

 **** Copyright (C), 2018, xx xx xx xx info&tech Co., Ltd.                ****

 ********************************************************************************
 * File Name     : ds3231.c
 * Author        : yexin
 * Date          : 2018-06-27
 * Description   : ds3231 driver
 * Version       : 1.0
 * Function List :
 *
 * Record        :
 * 1.Date        : 2018-06-27
 *   Author      : yexin
 *   Modification: Created file

*************************************************************************************************************/

#include "F2812_datatype.h"
#include "shellconsole.h"
#include "boarddrv.h"
#include "soft_i2c.h"
#include "rtc.h"
#include "boardcfg.h"

#define DEBUG_RTC  shellprintf

/*
 * RTC register addresses
 */
#define RTC_SEC_REG_ADDR	0x0
#define RTC_MIN_REG_ADDR	0x1
#define RTC_HR_REG_ADDR		0x2
#define RTC_DAY_REG_ADDR	0x3
#define RTC_DATE_REG_ADDR	0x4
#define RTC_MON_REG_ADDR	0x5
#define RTC_YR_REG_ADDR		0x6
#define RTC_CTL_REG_ADDR	0x0e
#define RTC_STAT_REG_ADDR	0x0f


/*
 * RTC control register bits
 */
#define RTC_CTL_BIT_A1IE	0x1	/* Alarm 1 interrupt enable     */
#define RTC_CTL_BIT_A2IE	0x2	/* Alarm 2 interrupt enable     */
#define RTC_CTL_BIT_INTCN	0x4	/* Interrupt control            */
#define RTC_CTL_BIT_RS1		0x8	/* Rate select 1                */
#define RTC_CTL_BIT_RS2		0x10	/* Rate select 2                */
#define RTC_CTL_BIT_DOSC	0x80	/* Disable Oscillator           */

/*
 * RTC status register bits
 */
#define RTC_STAT_BIT_A1F	0x1	/* Alarm 1 flag                 */
#define RTC_STAT_BIT_A2F	0x2	/* Alarm 2 flag                 */
#define RTC_STAT_BIT_OSF	0x80	/* Oscillator stop flag         */





static  UINT32 Bcd2Bin(UINT8 val)
{
	return ((val) & 0x0f) + ((val) >> 4) * 10;
}


/*
 * Get the current time from the RTC
 */
INT32 RtcGet (struct rtc_time *tmp)
{
	INT32 rel = 0;
	UINT8 sec, min, hour, mday, wday, mon_cent, year, control, status;

	control = RtcRead (RTC_CTL_REG_ADDR);
	status = RtcRead (RTC_STAT_REG_ADDR);
	sec = RtcRead (RTC_SEC_REG_ADDR);
	min = RtcRead (RTC_MIN_REG_ADDR);
	hour = RtcRead (RTC_HR_REG_ADDR);
	wday = RtcRead (RTC_DAY_REG_ADDR);
	mday = RtcRead (RTC_DATE_REG_ADDR);
	mon_cent = RtcRead (RTC_MON_REG_ADDR);
	year = RtcRead (RTC_YR_REG_ADDR);

	DEBUG_RTC ("Get RTC year: %02x mon/cent: %02x mday: %02x wday: %02x "
		"hr: %02x min: %02x sec: %02x control: %02x status: %02x\n",
		year, mon_cent, mday, wday, hour, min, sec, control, status);

	if (status & RTC_STAT_BIT_OSF) {
		DEBUG_RTC ("### Warning: RTC oscillator has stopped\n");
		/* clear the OSF flag */
		RtcWrite (RTC_STAT_REG_ADDR,
			   RtcRead (RTC_STAT_REG_ADDR) & ~RTC_STAT_BIT_OSF);
		rel = -1;
	}

	tmp->tm_sec  = Bcd2Bin (sec & 0x7F);
	tmp->tm_min  = Bcd2Bin (min & 0x7F);
	tmp->tm_hour = Bcd2Bin (hour & 0x3F);
	tmp->tm_mday = Bcd2Bin (mday & 0x3F);
	tmp->tm_mon  = Bcd2Bin (mon_cent & 0x1F);
	tmp->tm_year = Bcd2Bin (year) + ((mon_cent & 0x80) ? 2000 : 1900);
	tmp->tm_wday = Bcd2Bin ((wday - 1) & 0x07);
	tmp->tm_yday = 0;
	tmp->tm_isdst= 0;

	DEBUG_RTC ("Get DATE: %4d-%02d-%02d (wday=%d)  TIME: %2d:%02d:%02d\n",
		tmp->tm_year, tmp->tm_mon, tmp->tm_mday, tmp->tm_wday,
		tmp->tm_hour, tmp->tm_min, tmp->tm_sec);

	return rel;
}


/*
 * Set the RTC
 */
INT32 RtcSet (struct rtc_time *tmp)
{
	UINT8 century;

	DEBUG_RTC ("Set DATE: %4d-%02d-%02d (wday=%d)  TIME: %2d:%02d:%02d\n",
		tmp->tm_year, tmp->tm_mon, tmp->tm_mday, tmp->tm_wday,
		tmp->tm_hour, tmp->tm_min, tmp->tm_sec);

	RtcWrite (RTC_YR_REG_ADDR, Bcd2Bin (tmp->tm_year % 100));

	century = (tmp->tm_year >= 2000) ? 0x80 : 0;
	RtcWrite (RTC_MON_REG_ADDR, Bcd2Bin (tmp->tm_mon) | century);

	RtcWrite (RTC_DAY_REG_ADDR, Bcd2Bin (tmp->tm_wday + 1));
	RtcWrite (RTC_DATE_REG_ADDR, Bcd2Bin (tmp->tm_mday));
	RtcWrite (RTC_HR_REG_ADDR, Bcd2Bin (tmp->tm_hour));
	RtcWrite (RTC_MIN_REG_ADDR, Bcd2Bin (tmp->tm_min));
	RtcWrite (RTC_SEC_REG_ADDR, Bcd2Bin (tmp->tm_sec));

	return 0;
}

/*
 * Reset the RTC.  We also enable the oscillator output on the
 * SQW/INTB* pin and program it for 32,768 Hz output. Note that
 * according to the datasheet, turning on the square wave output
 * increases the current drain on the backup battery from about
 * 600 nA to 2uA.
 */
void RtcReset (void)
{
	RtcWrite (RTC_CTL_REG_ADDR, RTC_CTL_BIT_RS1 | RTC_CTL_BIT_RS2);
}


/*
 * Helper functions
 */

 UINT8 RtcRead (UINT8 reg)
{
    UINT8 readbuf = 0x0;
	INT16 result = 0;
	result=GpioI2cReadBytes(CONFIG_SYS_I2C_RTC_ADDR,reg,1,&readbuf,1);
	if(0!=result) {
		shellprintf("read rtc failure \n");
	}
	return readbuf;
}


 void RtcWrite (UINT8 reg, UINT8 val)
{
	INT16 result;
	result=GpioI2cWriteBytes(CONFIG_SYS_I2C_RTC_ADDR,reg,1,&val,1);
	if(0!=result) {
		shellprintf("write rtc failure \n");
	}
}



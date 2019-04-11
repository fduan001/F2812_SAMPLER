/********************************************************************************

 **** Copyright (C), 2017, xx xx xx xx info&tech Co., Ltd.                ****

 ********************************************************************************
 * File Name     : rtc.h
 * Author        : yexin technologic
 * Date          : 2018-6-27
 * Description   : rtc
 * Version       : 1.0
 * Function List :
 *
 * Record        :
 * 1.Date        : 2018-6-27
 *   Author      : yexin technologic
 *   Modification: Created file

*************************************************************************************************************/
#ifndef RTC_H
#define RTC_H
#include "F2812_datatype.h"

/*
 * The struct used to pass data from the generic interface code to
 * the hardware dependend low-level code ande vice versa. Identical
 * to struct rtc_time used by the Linux kernel.
 *
 * Note that there are small but significant differences to the
 * common "struct time":
 *
 *		struct time:		struct rtc_time:
 * tm_mon	0 ... 11		1 ... 12
 * tm_year	years since 1900	years since 0
 */

struct rtc_time {
	INT32 tm_sec;
	INT32 tm_min;
	INT32 tm_hour;
	INT32 tm_mday;
	INT32 tm_mon;
	INT32 tm_year;
	INT32 tm_wday;
	INT32 tm_yday;
	INT32 tm_isdst;
};

extern INT32 RtcSet (struct rtc_time *tmp);
extern INT32 RtcGet (struct rtc_time *tmp);

#endif




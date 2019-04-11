/********************************************************************************

 **** Copyright (C), 2018, xx xx xx xx info&tech Co., Ltd.                ****

 ********************************************************************************
 * File Name     : i2c_command.c
 * Author        : yexin
 * Date          : 2018-07-27
 * Description   : i2c command
 * Version       : 1.0
 * Function List :
 *
 * Record        :
 * 1.Date        : 2018-07-27
 *   Author      : yexin
 *   Modification: Created file

*************************************************************************************************************/
#include "shellconsole.h"
#include "boarddrv.h"
#include "command.h"
#include "util.h"
#include "rtc.h"
#include "soft_i2c.h"

s32 do_rtcread ( cmd_tbl_t *cmdtp, s32 flag, s32 argc, s8 *const argv[])
{

    u16  repeat,index;
    u8   readdata;
    u8   regaddr;
    char *ops;
    /*check input arg number*/
    if (argc < 2)
    {
        shellprintf ("Usage:\n%s\n", cmdtp->usage);
        return 1;
    }
    regaddr = simple_strtoul(argv[1], NULL, 16);
    readdata=RtcRead(regaddr);
    shellprintf("read rtc reg=0x%x read data=0x%x \n",regaddr,readdata);
    return 0;

}

s32 do_rtcdump ( cmd_tbl_t *cmdtp, s32 flag, s32 argc, s8 *const argv[])
{
	u8  regaddr = 0x0;
	u8  max_addr = 0x12;
	for(regaddr; regaddr <= max_addr; ++regaddr ) {
		shellprintf("0x%x: 0x%x\n", regaddr, RtcRead(regaddr));
	}
	
	return 0;
}

s32 do_rtcwrite ( cmd_tbl_t *cmdtp, s32 flag, s32 argc, s8 *const argv[])
{
    u16  repeat,index;
    u8   writedata;
    u8   regaddr;
    /*check input arg number*/
    if (argc < 3)
    {
        shellprintf ("Usage:\n%s\n", cmdtp->usage);
        return 1;
    }
    regaddr = simple_strtoul(argv[1], NULL, 16);
	writedata = simple_strtoul(argv[2], NULL, 16);
    RtcWrite(regaddr,writedata);
    shellprintf("write rtc reg=0x%x write data=0x%x \n",regaddr,writedata);
    return 0;
}

s32 do_rtcset ( cmd_tbl_t *cmdtp, s32 flag, s32 argc, s8 *const argv[])
{
	int rc = 0;
	struct rtc_time ts;
	char *date = NULL;
	char *timestamp = NULL;

	memset(&ts, 0, sizeof(ts));

	if( argc == 2 ) {
		date = argv[1];
	} 

	if( argc == 3 ) {
		date = argv[1];
		timestamp = argv[2];
	}

	if( date != NULL ) {
		sscanf(date, "%04d-%02d-%02d", &ts.tm_year, &ts.tm_mon, &ts.tm_mday);
	}

	if( timestamp != NULL ) {
		sscanf(timestamp, "%2d:%02d:%02d", &ts.tm_hour, &ts.tm_min, &ts.tm_sec);
	}

	rc = RtcSet(&ts);
	if( rc != 0 ) {
		shellprintf("RtcSet failed, rc=%d\n", rc);
		return 1;
	}

	/*
	INT32 tm_sec;
	INT32 tm_min;
	INT32 tm_hour;
	INT32 tm_mday;
	INT32 tm_mon;
	INT32 tm_year;
	INT32 tm_wday;
	INT32 tm_yday;
	INT32 tm_isdst;
	*/
	return 0;
}

s32 do_rtcget ( cmd_tbl_t *cmdtp, s32 flag, s32 argc, s8 *const argv[])
{
	int rc = 0;
	struct rtc_time ts;
	rc = RtcGet(&ts);
	if( rc != 0 ) {
		shellprintf("Failed to rtcget timestamp, rc=%d\n", rc);
		return 1;
	}
	shellprintf("%d-%d-%d %d:%d:%d\n", ts.tm_year, ts.tm_mon, ts.tm_mday, 
		ts.tm_hour, ts.tm_min, ts.tm_sec);
	return 0;
}

#pragma DATA_SECTION   (i2ccmd,"shell_lib");
far cmd_tbl_t i2ccmd[] =
{
	{
		"rtcread", CONFIG_SYS_MAXARGS, 1,	do_rtcread,
		"rtc read reg",
		"regaddr   \n"
		"regaddr :	rtc register addr\n"
		"example:  rtcread  0x0"

	},

	{
		"rtcwrite", CONFIG_SYS_MAXARGS, 1,	do_rtcwrite,
		"rtcwrite reg",
		"regaddr regvalue   \n"
		"regaddr :	rtc register addr\n"
		"regavalue:  write reg value \n"
		"example:  rtcwrite  0x0 0x32"
		
	},

	{
		"rtcdump", CONFIG_SYS_MAXARGS, 1,	do_rtcdump,
		"rtcdump",
		"regaddr   \n"
		"regaddr :	rtc register addr\n"
		"example:  rtcdump"

	},

	{
		"rtcset", CONFIG_SYS_MAXARGS, 1,	do_rtcset,
		"rtcget",
		"year-mon-date:hr:min:sec   \n"
		"example:  rtcset 2018-09-10 9:06:05"
	},

	{
		"rtcget", CONFIG_SYS_MAXARGS, 1,	do_rtcget,
		"rtcget",
		"display the date and timestamp   \n"
		"example:  rtcget"
	},

};

void I2cCmdInitialzie()
{
    s8 index;
    for (index = 0; index < sizeof(i2ccmd) / sizeof(cmd_tbl_t); index++)
        RegisterCommand(i2ccmd[index]);
}






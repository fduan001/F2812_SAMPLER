#include "shellconsole.h"
#include "boarddrv.h"
#include "command.h"
#include "util.h"
#include "temp_measure.h"

INT32 do_temp( cmd_tbl_t *cmdtp, INT32 flag, INT32 argc, INT8 *const argv[])
{
	char *ops = NULL;
	UINT8 chan = 0;

	if( argc < 2 ) {
		shellprintf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	ops = argv[1];

	if( strcmp(ops, "init") == 0 ) {
		TempMeasInit();
		return 0;
	}

	if( strcmp(ops, "start") == 0 ) {
		chan = simple_strtoul(argv[2], NULL, 10);
		TempMeasStart(chan);
		return 0;
	}

	if( strcmp(ops, "reset") == 0 ) {
		return 0;
	}

	if( strcmp(ops, "dump") == 0 ) {
		TempMeasDump();
		return 0;
	}

	if( strcmp(ops, "test") == 0 ) {
		TempMeasSelfTest();
		return 0;
	}
	return 1;
}


#pragma DATA_SECTION   (temp_cmd,"shell_lib");
far cmd_tbl_t temp_cmd[] =
{

	{
		"temp", CONFIG_SYS_MAXARGS, 1,	do_temp,
		"temp init/start/reset/dump/test",
		"init - initialize the temp measurement system\n"
		"start chan - start specified channel temp measurement\n"
		"reset -  reset the temp measurement system\n"
		"dump - dump  the temp measurment result\n"
		"test - temp measurement self test\n"
		"example:  temp start 0 : start channel 0 temp measurement\n"
	},
};

void TempCmdInitialzie()
{
    INT8 index;
    for (index = 0; index < sizeof(temp_cmd) / sizeof(cmd_tbl_t); index++)
        RegisterCommand(temp_cmd[index]);
}

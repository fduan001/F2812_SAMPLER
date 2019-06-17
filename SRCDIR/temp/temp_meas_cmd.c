#include "shellconsole.h"
#include "boarddrv.h"
#include "command.h"
#include "util.h"
#include "temp_measure.h"

INT32 do_temp( cmd_tbl_t *cmdtp, INT32 flag, INT32 argc, INT8 *const argv[])
{
	char *ops = NULL;
	UINT8 chan = 0;
	UINT32 diff = 0;

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
		TempMeasStart();
		return 0;
	}

	if( strcmp(ops, "stop") == 0 ) {
		TempMeasStop();
		return 0;
	}

	if( strcmp(ops, "cal") == 0 ) {
		diff = TempMeasCalibration();
		PRINTF("diff=%08lu\n", diff);
		return 0;
	}

	return 1;
}


#pragma DATA_SECTION   (temp_cmd,"shell_lib");
cmd_tbl_t temp_cmd[] =
{
	{
		"temp", CONFIG_SYS_MAXARGS, 1,	do_temp,
		"temp init/start/stop/cal",
		"init - initialize the temp measurement system\n"
		"start - start temp measurement\n"
		"stop - stop temp measurement\n"
		"cal -  calibration\n"
	},
};

void TempCmdInitialzie()
{
    INT8 index;
    for (index = 0; index < sizeof(temp_cmd) / sizeof(cmd_tbl_t); index++)
        RegisterCommand(temp_cmd[index]);
}

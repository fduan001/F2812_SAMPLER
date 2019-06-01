#include "F2812_datatype.h"
#include "shellconsole.h"
#include "boarddrv.h"
#include "command.h"
#include "util.h"
#include "role.h"

#ifndef PRINTF
#define PRINTF   console_printf
#endif

INT32 do_role_cmd(cmd_tbl_t * cmdtp, INT32 flag, INT32 argc, char * const argv[])
{
	char *ops;
	int curr_role = 0;

	if( argc < 2 ) {
		return CMD_RET_USAGE;
	}

	ops = argv[1];
	if( strcmp(ops, "switch") == 0 ) {
		SetMyRole(I_AM_MASTER); // force myself to be master
		if( GetMyRole() == I_AM_MASTER ) {
			PRINTF("I AM MASTER NOW\n");
		} else {
			PRINTF("ROLE SWITCH OOPS\n");
			PRINTF("I AM STILL SLAVE\n");
		}
	}

	if( strcmp(ops, "dump") == 0 ) {
		curr_role = GetMyRole();
		if( curr_role == I_AM_MASTER ) {
			PRINTF("I AM MASTER\n");
		} else {
			PRINTF("I AM SLAVE\n");
		}

		if( IsMatePresent() ) {
			PRINTF("MATE PRESENT\n");
			curr_role = GetMateRole();
			if( curr_role == I_AM_MASTER ) {
				PRINTF("Mate is MASTER\n");
			} else {
				PRINTF("Mate is SLAVE\n");
			}
		} else {
			PRINTF("MATE ABSENT\n");
		}
	}

	if( strcmp(ops, "reset") == 0 ) {
		ResetMateCard();
	}

	return 0;
}

#pragma DATA_SECTION   (role_cmd, "shell_lib");
far cmd_tbl_t role_cmd[] =
{
	{
		"role", CONFIG_SYS_MAXARGS, 1,	do_role_cmd,
		"role switch/dump/reset",
		"role switch: force myself to be master\n"
		"role dump: dump current role\n"
	},
};

int RoleCmdInit(void)
{
	INT8 index;

	for (index = 0; index < sizeof(role_cmd) / sizeof(cmd_tbl_t); index++)
		RegisterCommand(role_cmd[index]);

	PutMeOnline();
	return 0;
}



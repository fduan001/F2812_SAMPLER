#include "shellconsole.h"
#include "boarddrv.h"
#include "command.h"
#include "util.h"
#include "platform_os.h"
#include "host_spi.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)  (sizeof(x)/sizeof(x[0]))
#endif

#ifndef NULL
#define NULL   (void*)0
#endif

#ifndef DISP_LINE_LEN
#define DISP_LINE_LEN	16
#endif

INT32 do_host_spi_cs(cmd_tbl_t *cmdtp, INT32 flag, INT32 argc, char * const argv[])
{
	UINT32 option;

	if (argc != 2)
	{
		PRINTF("argc=%d\n", argc);
		return cmd_usage(cmdtp);
	}

	option = simple_strtoul(argv[1], NULL, 10);

	option = (option == 0) ? 0 : 1;

	if( option ) {
		HostSpiAssertCS();
	} else {
		HostSpiDeassertCS();
	}

	return CMD_RET_SUCCESS;
}

static cmd_tbl_t cmd_host_spi_sub[] = {
	{"cs", 6, 1, do_host_spi_cs, "", ""},
};

INT32 do_host_spi(cmd_tbl_t * cmdtp, INT32 flag, INT32 argc, char * const argv[])
{
	cmd_tbl_t *c;

	if (argc < 2) {

		return CMD_RET_USAGE;
	}

	/* Strip off leading 'i2c' command argument */
	argc--;
	argv++;

	c = find_cmd_tbl(argv[0], &cmd_host_spi_sub[0], ARRAY_SIZE(cmd_host_spi_sub));

	if (c)
		return c->cmd(cmdtp, flag, argc, argv);
	else
		return CMD_RET_USAGE;
}

#pragma DATA_SECTION   (host_spi_cmd, "shell_lib");
far cmd_tbl_t host_spi_cmd[] =
{
	{
		"host_spi", CONFIG_SYS_MAXARGS, 1,	do_host_spi,
		"host_spi ops  args",
		"host_spi cs channel option\n"
		"host_spi read channel length\n"
		"host_spi write channel source length\n"
	},
};

int HostSpiCmdInit(void)
{
	INT8 index;

	for (index = 0; index < sizeof(host_spi_cmd) / sizeof(cmd_tbl_t); index++)
		RegisterCommand(host_spi_cmd[index]);
	return 0;
}

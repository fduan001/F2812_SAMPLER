#include "F2812_datatype.h"
#include "shellconsole.h"
#include "boarddrv.h"
#include "command.h"
#include "util.h"
#include "platform_os.h"
#include "F2812_gpio.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)  (sizeof(x)/sizeof(x[0]))
#endif

#ifndef NULL
#define NULL   (void*)0
#endif

#ifndef DISP_LINE_LEN
#define DISP_LINE_LEN	16
#endif

INT32 do_gpio(cmd_tbl_t * cmdtp, INT32 flag, INT32 argc, char * const argv[])
{
	char *ops = NULL;
	UINT8 val = 0;
	UINT8 cmd = 0;

	if (argc < 2) {
		return CMD_RET_USAGE;
	}
	ops = argv[1];
	if( strcmp(ops, "dump") == 0 ) {
		val = GpioMateCardGet(0);
		if( val ) {
			PRINTF("0: H\n");
		} else {
			PRINTF("0: L\n");
		}
		val = GpioMateCardGet(1);
		if( val ) {
			PRINTF("1: H\n");
		} else {
			PRINTF("1: L\n");
		}
		val = GpioMateCardGet(2);
		if( val ) {
			PRINTF("2: H\n");
		} else {
			PRINTF("2: L\n");
		}
		return 0;
	}

	if( strcmp(ops, "set") == 0 ) {
		val = simple_strtoul(argv[2], NULL, 10);
		cmd = simple_strtoul(argv[3], NULL, 10);
		if( val > 3 ) {
			PRINTF("val %u out of range\n", val);
			return CMD_RET_FAILURE;
		}

		GpioMateCardSet(val, cmd);
		return 0;
	}

	return CMD_RET_USAGE;
}

#pragma DATA_SECTION   (gpio_cmd, "shell_lib");
far cmd_tbl_t gpio_cmd[] =
{
	{
		"gpio", CONFIG_SYS_MAXARGS, 1,	do_gpio,
		"gpio ops  args",
		"gpio cs channel option\n"
		"gpio read channel length\n"
		"gpio write channel source length\n"
	},
};

int GpioCmdInit(void)
{
	INT8 index;

	for (index = 0; index < sizeof(gpio_cmd) / sizeof(cmd_tbl_t); index++)
		RegisterCommand(gpio_cmd[index]);
	return 0;
}

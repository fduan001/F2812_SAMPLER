#include "F2812_datatype.h"
#include "shellconsole.h"
#include "command.h"
#include "util.h"
#include "fram.h"

#ifndef NULL
#define NULL   (void*)0
#endif

INT32 do_fram ( cmd_tbl_t *cmdtp, INT32 flag, INT32 argc, INT8 *const argv[])
{
	char *ops;
	UINT16 addr = 0;
	UINT8 val = 0;
	UINT16 len = 1;
	UINT16 i = 0;
	UINT8  buffer[100] = {0, 0};

    /*check input arg number*/
	if (argc < 2)
	{
		shellprintf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	ops = argv[1];

	if( strcmp(ops, "init") == 0) {
		FRAMInit();
		return 0;
	}

	if( strcmp(ops, "read") == 0 ) {
		addr = simple_strtoul(argv[2], NULL, 16);
		if( argc >= 4 ) {
			len = simple_strtoul(argv[3], NULL, 16);
		}

		if( len == 1 ) {
			val = FRAMReadByte(addr);
			PRINTF("%x: 0x%02x\n", addr, val);
		} else {
			len = len > 100 ? 100 : len;
			FRAMReadBlock(addr, buffer, len);
			for( i = 0 ; i < len; ++i ) {
				PRINTF("0x%02x ", buffer[i]);
				if( (i+1) % 7 == 0 ) {
					PRINTF("\n");
				}
			}
			PRINTF("\n");
		}
		return 0;
	}

	if( strcmp(ops, "write") == 0 ) {
		addr = simple_strtoul(argv[2], NULL, 16);
		val = simple_strtoul(argv[3], NULL, 16);
		if( argc < 4 ) {
			return 1;
		}
		len = argc - 3;
		len  = len > 100 ? 100 : len;
		for( i = 0; i < len; ++i) {
			buffer[i] = simple_strtoul(argv[3 + i], NULL, 16);
		}

		if( len == 1 ) {
			FRAMWriteByte(addr, val);
		} else {
			FRAMWriteBlock(addr, buffer, len);
		}
		return 0;
	}

	if( strcmp(ops, "rsr") == 0 ) {
		val = FRAMReadSR();
		PRINTF("SR: 0x%02x\n", val);
		return 0;
	}

	return 1;
}

#pragma DATA_SECTION   (fram_cmd,"shell_lib");
far cmd_tbl_t fram_cmd[] =
{
	{
		"fram", CONFIG_SYS_MAXARGS, 1,	do_fram,
		"fram init/read/write",
		"example:  fram read 0x10\n"
		"          fram write 0x10 0x5a\n"
	},
};


void FRAMCmdInitialize()
{
	INT8 index;

	for (index = 0; index < sizeof(fram_cmd) / sizeof(cmd_tbl_t); index++)
		RegisterCommand(fram_cmd[index]);
}

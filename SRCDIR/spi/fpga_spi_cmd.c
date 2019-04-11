#include "fpga.h"
#include "fpga_spi.h"
#include "cmd.h"
#include "cmd_console.h"
#include "strto.h"
#include "display.h"


#ifndef NULL
#define NULL   (void*)0
#endif

#ifndef DISP_LINE_LEN
#define DISP_LINE_LEN	16
#endif

static int do_fpga_spi_cs(cmd_tbl_t *cmdtp, int argc, char * const argv[])
{
	int rc = 0;
	int channel;
	unsigned int option;

	if (argc != 3)
	{
		PRINTF("argc=%d\n", argc);
		return cmd_usage(cmdtp);
	}

	channel = simple_strtoul(argv[1], NULL, 10);
	option = simple_strtoul(argv[2], NULL, 10);

	option = (option == 0) ? 0 : 1;

	FpgaSpiCs(channel, option);

	return CMD_RET_SUCCESS;
}

static int do_fpga_spi_read(cmd_tbl_t *cmdtp, int argc, char * const argv[])
{
	int rc = 0;
	int channel;
	unsigned int length;
	unsigned char  *buffer;

	if (argc != 3)
	{
		PRINTF("argc=%d\n", argc);
		return cmd_usage(cmdtp);
	}

	channel = simple_strtoul(argv[1], NULL, 10);
	length = simple_strtoul(argv[2], NULL, 16);

	buffer = (unsigned char*)smm_alloc(length);
	if( !buffer ) {
		PRINTF("memory is not ENOUGH, OOPS(%u)!\n", length);
		return CMD_RET_FAILURE;
	}

	rc = FpgaSpiRead(channel, buffer, length);
	if( rc != 0 ) {
		smm_free(buffer);
		PRINTF("FpgaSpiRead failed, rc=%d\n", rc);
		return CMD_RET_FAILURE;
	} else {
		printf_buffer(0, buffer, 1, length, DISP_LINE_LEN);
	}

	smm_free(buffer);

	return CMD_RET_SUCCESS;
}

static int do_fpga_spi_write(cmd_tbl_t *cmdtp, int argc, char * const argv[])
{
	int rc = 0;
	int channel;
	unsigned int source;
	unsigned int length;

	if (argc != 4)
	{
		PRINTF("argc=%d\n", argc);
		return cmd_usage(cmdtp);
	}

	channel = simple_strtoul(argv[1], NULL, 10);
	source = simple_strtoul(argv[2], NULL, 16);
	length = simple_strtoul(argv[3], NULL, 16);

	rc = FpgaSpiWrite(channel, (unsigned char*)source, length);
	if( rc != 0 ) {
		PRINTF("FpgaSpiWrite failed, rc=%d\n", rc);
		return CMD_RET_FAILURE;
	}

	PRINTF("FPGA SPI WRITE COMPLETE\n");
	return CMD_RET_SUCCESS;
}

static cmd_tbl_t cmd_fpga_spi_sub[] = {
	{"cs", 6, do_fpga_spi_cs, "", ""},
	{"read", 6, do_fpga_spi_read, "", ""},
	{"write", 6, do_fpga_spi_write, "", ""},
};

static int do_fpga_spi(cmd_tbl_t * cmdtp, int argc, char * const argv[])
{
	cmd_tbl_t *c;

	if (argc < 2) {

		return CMD_RET_USAGE;
	}

	/* Strip off leading 'i2c' command argument */
	argc--;
	argv++;

	c = find_cmd_tbl(argv[0], &cmd_fpga_spi_sub[0], ARRAY_SIZE(cmd_fpga_spi_sub));

	if (c)
		return c->cmd(cmdtp, argc, argv);
	else
		return CMD_RET_USAGE;
}

static char fpga_spi_help_text[] = \
	"fpga_spi cs channel option\n" \
	"fpga_spi read channel length\n" \
	"fpga_spi write channel source length\n";

int fpga_spi_cmd_init(void)
{
	ADD_CMD(fpga_spi, 6, do_fpga_spi,
		"FPGA SPI sub-system",
		fpga_spi_help_text);
	return 0;
}

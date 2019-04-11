#include "altera_rs422.h"
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

static int do_rs422_init(cmd_tbl_t *cmdtp, int argc, char * const argv[])
{
	int  rc = 0;
	unsigned char chip;
	char party;
	unsigned char stop;
	unsigned char data_bit;
	unsigned int baud;

	if (argc != 6)
	{
		PRINTF("argc=%d\n", argc);
		return cmd_usage(cmdtp);
	}

	chip = simple_strtoul(argv[1], NULL, 10);
	party = simple_strtoul(argv[2], NULL, 10);
	stop = simple_strtoul(argv[3], NULL, 10);
	data_bit = simple_strtoul(argv[4], NULL, 10);
	baud = simple_strtoul(argv[5], NULL, 10);

	rc = altera_uart_open(chip, party, stop, data_bit, baud);
	if( rc != 0 ) {
		PRINTF("altera_uart_open failed, rc=%d\n", rc);
		return CMD_RET_FAILURE;
	}

	return CMD_RET_SUCCESS;
}

static int do_rs422_show(cmd_tbl_t *cmdtp, int argc, char * const argv[])
{
	unsigned char chip;

	if (argc != 2)
	{
		PRINTF("argc=%d\n", argc);
		return cmd_usage(cmdtp);
	}

	chip = simple_strtoul(argv[1], NULL, 10);
	DebugUartRegInfo(chip);

	return CMD_RET_SUCCESS;
}

static int do_rs422_read(cmd_tbl_t *cmdtp, int argc, char * const argv[])
{
	int rc = 0;
	int chip;
	unsigned int bytes;
	char *buff = NULL;

	if (argc != 3)
	{
		PRINTF("argc=%d\n", argc);
		return cmd_usage(cmdtp);
	}

	chip = simple_strtoul(argv[1], NULL, 10);
	bytes = simple_strtoul(argv[2], NULL, 10);

	buff = smm_alloc(bytes);
	if( buff == NULL ) {
		PRINTF("memory is not ENOUGH, OOPS(%u)!\n", bytes);
		return CMD_RET_FAILURE;
	}

	rc = altera_uart_read(chip, buff, bytes);
	if( rc != 0 ) {
		smm_free(buff);
		PRINTF("altera_uart_read failed\n");
		return CMD_RET_FAILURE;
	}

	printf_buffer(0, buff, 1, bytes, DISP_LINE_LEN);

	smm_free(buff);
	return CMD_RET_SUCCESS;
}

static int do_rs422_write(cmd_tbl_t *cmdtp, int argc, char * const argv[])
{
	int rc = 0;
	int chip;
	unsigned int bytes;
	unsigned int source;

	if (argc != 4)
	{
		PRINTF("argc=%d\n", argc);
		return cmd_usage(cmdtp);
	}

	chip = simple_strtoul(argv[1], NULL, 10);
	source = simple_strtoul(argv[2], NULL, 16);
	bytes = simple_strtoul(argv[3], NULL, 10);

	rc = altera_uart_write(chip, source, bytes);
	if( rc != 0 ) {
		PRINTF("altera_uart_write failed\n");
		return CMD_RET_FAILURE;
	}

	PRINTF("rs422 WRITE COMPLETE\n");
	return CMD_RET_SUCCESS;
}

#define  UT_LEN   16

static int do_rs422_test(cmd_tbl_t *cmdtp, int argc, char * const argv[])
{
	int  rc = 0;
	unsigned char cbuf[64];
	int  i = 0;
	int index = 0;
	int  activeNo = 0;

	if (argc != 1)
	{
		PRINTF("argc=%d\n", argc);
		return cmd_usage(cmdtp);
	}

	rc = altera_uart_open(0, 'e', 1, 8, 115200);
	if( rc != 0 ) {
		PRINTF("altera_uart_open failed, rc=%d\n", rc);
		return CMD_RET_FAILURE;
	}

	rc = altera_uart_open(1, 'e', 1, 8, 115200);
	if( rc != 0 ) {
		PRINTF("altera_uart_open failed, rc=%d\n", rc);
		return CMD_RET_FAILURE;
	}

	memset(cbuf, 0, sizeof(cbuf));
	     
	for(i = 0;i < UT_LEN;i++)
	{
	    cbuf[i] = i+0x30;
	    	 //cbuf[i] = (i % 2 == 0) ? 0x5a : 0xa5;
	}
	sysMsDelay(100);
	     
	PRINTF("----Send RS422 Frame on chip1-----\r\n");
	    // for(index = 0;index <= 0x100;index++)
	     {
	     PRINTF("send index = %d\r\n",index); 	 
	     altera_uart_write(0,cbuf,UT_LEN);
	     sysMsDelay(1000);
	     } 
	     activeNo = 1;
	     PRINTF("send ok......\r\n"); 	 
	     
	     for(index = 0;index <= 0x100;index++)
	     { 
	    	 memset(cbuf,0,sizeof(cbuf));
	    	 if(activeNo == 1)
	    	 {
		    	 if(altera_uart_read(1,cbuf,UT_LEN) == UT_LEN)
		    	 {
		    		 PRINTF("----Chip2 Recv RS422 Frame -----\r\n");

		    	     for(i = 0;i < UT_LEN;i++)
		    	     {
		    	    	 PRINTF("data[%02d] = %2x\r\n",i,cbuf[i]);
		    	    	 cbuf[i]++;
		    	     }
		    	     
		    	     altera_uart_write(1,cbuf,UT_LEN);
		    	     PRINTF("----Send RS422 Frame on chip2-----\r\n");
		    	     activeNo = 0;
		     		 
		    	 }
	    	 }
	    	 else
	    	 {
	    		 if(altera_uart_read(0,cbuf,UT_LEN) == UT_LEN)
		    	 {
		    		 PRINTF("----Chip1 Recv RS422 Frame -----\r\n");
	    		 
		    	     for(i = 0;i < UT_LEN;i++)
		    	     {
		    	    	 PRINTF("data[%02d] = %2x\r\n",i,cbuf[i]);
		    	    	 cbuf[i]++;
		    	     }
		    	     
		    	     altera_uart_write(0,cbuf,UT_LEN);
		    	     PRINTF("----Send RS422 Frame on chip1-----\r\n");
		    	     activeNo = 1;
		     		 
		    	 } 		 
	    	 }

	    	 
	    	 sysMsDelay(200);
	    	 
	    	 
	     }

	return CMD_RET_SUCCESS;
}

static cmd_tbl_t cmd_rs422_sub[] = {
	{"init", 8, do_rs422_init, "", ""},
	{"show", 2, do_rs422_show, "", ""},
	{"read", 6, do_rs422_read, "", ""},
	{"write", 6, do_rs422_write, "", ""},
	{"test", 1, do_rs422_test, "", ""},
};

static int do_rs422(cmd_tbl_t * cmdtp, int argc, char * const argv[])
{
	cmd_tbl_t *c;

	if (argc < 2) {

		return CMD_RET_USAGE;
	}

	/* Strip off leading 'i2c' command argument */
	argc--;
	argv++;

	c = find_cmd_tbl(argv[0], &cmd_rs422_sub[0], ARRAY_SIZE(cmd_rs422_sub));

	if (c)
		return c->cmd(cmdtp, argc, argv);
	else
		return CMD_RET_USAGE;
}

static char rs422_help_text[] = \
	"rs422 init chip \n" \
	"rs422 show chip \n" \
	"rs422 test \n" \
	"rs422 read chip bytes\n" \
	"rs422 write chip source bytes\n";

int rs422_cmd_init(void)
{
	ADD_CMD(rs422, 8, do_rs422,
		"rs422 sub-system",
		rs422_help_text);
	return 0;
}

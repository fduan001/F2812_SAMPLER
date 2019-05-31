#include "F2812_datatype.h"
#include "shellconsole.h"
#include "fpga.h"
#include "temp_measure.h"

typedef struct TEMP_MEAS_DATA_T
{
	UINT8 valid;
	UINT16 datah;
	UINT16 datal;
} temp_meas_data_t;

#define MAX_TEMP_CHANLLE       4
static temp_meas_data_t  g_temp_meas_data[MAX_TEMP_CHANLLE];

#define TEMP_INTR_BIT_POS         5  

void TempIntrHandler(UINT8 bit_pos);

int TempMeasInit(void) {
	int i = 0;
	for( i = 0; i < MAX_TEMP_CHANLLE; ++i ) {
		g_temp_meas_data[i].valid = 0;
		g_temp_meas_data[i].datah = 0;
		g_temp_meas_data[i].datal = 0;
	}

	return 0;
}

int TempMeasStart(UINT8 chan) {
	UINT16 val = 0;
	if( chan > MAX_TEMP_CHANLLE ) {
		return 1;
	}


	return 0;
}

int TempGetMeasData(UINT8 chan, UINT16 *data) {
	if( chan > MAX_TEMP_CHANLLE ) {
		return 1;
	}

	if( g_temp_meas_data[chan].valid == 0 ) {
		return 1;
	}

	data[0] = g_temp_meas_data[chan].datah;
	data[1] = g_temp_meas_data[chan].datal;

	/* clear the valid flag */
	g_temp_meas_data[chan].valid = 0;
	return 0;
}

void TempIntrHandler(UINT8 bit_pos) {

	return ;
}

int TempMeasSelfTest(void) {
	return 0;
}

void TempMeasDump(void) {
	UINT8 i = 0;
	for( i = 0; i < MAX_TEMP_CHANLLE; ++i ) {
		PRINTF("valid=%u datah=%u datal=%u\n", 
			g_temp_meas_data[i].valid,
			g_temp_meas_data[i].datah,
			g_temp_meas_data[i].datal);
	}
}

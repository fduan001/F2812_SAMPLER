#include "F2812_datatype.h"
#include "shellconsole.h"
#include "fpga.h"
#include "fuel_measure.h"

#define FUEL_USR_INTR_MODE 1
typedef struct FUEL_MEAS_DATA_T
{
	UINT8 valid;
	UINT16 datah;
	UINT16 datal;
} fuel_meas_data_t;

#define MAX_FUEL_CHANLLE       4
static fuel_meas_data_t  g_fuel_meas_data[MAX_FUEL_CHANLLE];

#define FUEL_INTR_BIT_POS         5  

void FuelIntrHandler(UINT8 bit_pos);

int FuelMeasInit(void) {
	int i = 0;
	for( i = 0; i < MAX_FUEL_CHANLLE; ++i ) {
		g_fuel_meas_data[i].valid = 0;
		g_fuel_meas_data[i].datah = 0;
		g_fuel_meas_data[i].datal = 0;
	}

	FPGA_REG16_W(FPGA_FUEL_SWITCH_REG, 0);
	FPGA_REG16_W(FPGA_FUEL_MEAS_REG, 0);
	FPGA_REG16_W(FPGA_FUEL_STATUS_REG, 0xFFFF);
#ifdef FUEL_USR_INTR_MODE
	RegisterIsr(FUEL_INTR_BIT_POS, FuelIntrHandler);
#endif
	return 0;
}

int FuelMeasStart(UINT8 chan) {
	UINT16 val = 0;
	if( chan > MAX_FUEL_CHANLLE ) {
		return 1;
	}

	/* rising edge to trigger one-shot measurement */
	val = FPGA_REG16_R(FPGA_FUEL_MEAS_REG);
	val &= ~(1 << chan);
	FPGA_REG16_W(FPGA_FUEL_MEAS_REG, val);
	val |= (1 << chan);
	FPGA_REG16_W(FPGA_FUEL_MEAS_REG, val);

	return 0;
}

#ifndef FUEL_USR_INTR_MODE
int IsFuelMeasReady(UINT8 chan) {
	UINT32 i = 0;
	UINT16 period = FPGA_REG16_R(FPGA_FUEL_MEAS_PERIOD_REG);
	UINT32 limit = period * 1000;
	UINT16 sts = 0;
	if( chan > MAX_FUEL_CHANLLE ) {
		return 0;
	}

	if( limit == 0 ) {
		PRINTF("period register is not well set\n");
		return 0;
	}
	while(1) {
		sts = FPGA_REG16_R(FPGA_FUEL_STATUS_REG);
		if( sts & (1 << chan) ) {
			FPGA_REG16_W(FPGA_FUEL_STATUS_REG, (1 << chan)); /* w1c */
			return 1;
		}
		++i;
		if( i >= limit ) {
			PRINTF("timeout for this measurement, chan=%u\n", chan);
			return 0;
		}
		PlatformDelay(1000); /* delay 1 ms */
	}
	return 0;
}
#endif

int FuelGetMeasData(UINT8 chan, UINT16 *data) {
	if( chan > MAX_FUEL_CHANLLE ) {
		return 1;
	}
#ifdef FUEL_USR_INTR_MODE
	if( g_fuel_meas_data[chan].valid == 0 ) {
		return 1;
	}
#else
	if( IsFuelMeasReady(chan) != 1 ) {
		PRINTF("measurement for chan %u not ready\n", chan);
		return 1;
	}
#endif

	data[0] = g_fuel_meas_data[chan].datah;
	data[1] = g_fuel_meas_data[chan].datal;

#ifdef FUEL_USR_INTR_MODE
	/* clear the valid flag */
	g_fuel_meas_data[chan].valid = 0;
#endif
	return 0;
}

#ifdef FUEL_USR_INTR_MODE
void FuelIntrHandler(UINT8 bit_pos) {
	UINT16 sts = FPGA_REG16_R(FPGA_FUEL_STATUS_REG);

	if( sts & 0x1 ) {
		g_fuel_meas_data[0].datah = FPGA_REG16_R(FPGA_FUEL_TUNNEL1_RESULT_H_REG);
		g_fuel_meas_data[0].datal = FPGA_REG16_R(FPGA_FUEL_TUNNEL1_RESULT_L_REG);
		g_fuel_meas_data[0].valid = 1;
	}

	if( sts & 0x2 ) {
		g_fuel_meas_data[1].datah = FPGA_REG16_R(FPGA_FUEL_TUNNEL2_RESULT_H_REG);
		g_fuel_meas_data[1].datal = FPGA_REG16_R(FPGA_FUEL_TUNNEL2_RESULT_L_REG);
		g_fuel_meas_data[1].valid = 1;
	}

	if( sts & 0x4 ) {
		g_fuel_meas_data[2].datah = FPGA_REG16_R(FPGA_FUEL_TUNNEL3_RESULT_H_REG);
		g_fuel_meas_data[2].datal = FPGA_REG16_R(FPGA_FUEL_TUNNEL3_RESULT_L_REG);
		g_fuel_meas_data[2].valid = 1;
	}

	if( sts & 0x8 ) {
		g_fuel_meas_data[3].datah = FPGA_REG16_R(FPGA_FUEL_TUNNEL3_RESULT_H_REG);
		g_fuel_meas_data[3].datal = FPGA_REG16_R(FPGA_FUEL_TUNNEL3_RESULT_L_REG);
		g_fuel_meas_data[3].valid = 1;
	}

	/* write b'1 to clear the flag */
	FPGA_REG16_W(FPGA_FUEL_STATUS_REG, sts);
	return ;
}
#endif

int FuelMeasSelfTest(void) {
	UINT8 i = 0;
	UINT16 val = 0;
	UINT16 data[2];
	UINT16 count = 0;

	FuelMeasInit();
	/* switch to internal self test signal */
	val = ((1 << 3) | ( 1 << 2));
	FPGA_REG16_W(FPGA_FUEL_SWITCH_REG, val);
	
	for( i = 0; i < MAX_FUEL_CHANLLE; ++i ) {
		/* choose chan */
		val &= ~0x3;
		val |= i;
		FPGA_REG16_W(FPGA_FUEL_SWITCH_REG, val);
		FuelMeasStart(i);

		/* wait for measurement complete */
		while( count < 1000 ) {
			if( FuelGetMeasData(i, data) == 0 ) {
				count = 0;
				PRINTF("chan %u measure complete %u %u\n", i, data[0], data[1]);
				break;
			}
			count++;
			Osal_TaskSleep(1);
		}
	}

	/* recover the fuel registers */
	FPGA_REG16_W(FPGA_FUEL_SWITCH_REG, 0);
	FPGA_REG16_W(FPGA_FUEL_MEAS_REG, 0);
	FPGA_REG16_W(FPGA_FUEL_STATUS_REG, 0xFFFF);

	return 0;
}

void FuelMeasDump(void) {
	UINT8 i = 0;
	for( i = 0; i < MAX_FUEL_CHANLLE; ++i ) {
		PRINTF("valid=%u datah=%u datal=%u\n", 
			g_fuel_meas_data[i].valid,
			g_fuel_meas_data[i].datah,
			g_fuel_meas_data[i].datal);
	}
}

#include "F2812_datatype.h"
#include "shellconsole.h"
#include "fpga.h"
#include "flow_measure.h"

static u8 g_data_valid = 0;

typedef struct FLOW_MEAS_DATA_T
{
	u8 valid;
	UINT16 datah;
	UINT16 datal;
} flow_meas_data_t;

#define MAX_FLOW_CHANLLE       4
static flow_meas_data_t  g_flow_meas_data[MAX_FLOW_CHANLLE];

#define FLOW_INTR_BIT_POS         6

void FlowIntrHandler(UINT8 bit_pos);

int FlowMeasInit(void) {
	int i = 0;
	for( i = 0; i < MAX_FLOW_CHANLLE; ++i ) {
		g_flow_meas_data[i].valid = 0;
		g_flow_meas_data[i].datah = 0;
		g_flow_meas_data[i].datal = 0;
	}

	FPGA_REG16_W(FPGA_FLOW_SWITCH_REG, 0);
	FPGA_REG16_W(FPGA_FLOW_MEAS_REG, 0);
	FPGA_REG16_W(FPGA_FLOW_STATUS_REG, 0xFFFF);
	RegisterIsr(FLOW_INTR_BIT_POS, FlowIntrHandler);
	return 0;
}

int FlowMeasStart(UINT8 chan) {
	UINT16 val = 0;
	if( chan > MAX_FLOW_CHANLLE ) {
		return 1;
	}

	/* rising edge to trigger one-shot measurement */
	val = FPGA_REG16_R(FPGA_FLOW_MEAS_REG);
	val &= ~(1 << chan);
	FPGA_REG16_W(FPGA_FLOW_MEAS_REG, val);
	val |= (1 << chan);
	FPGA_REG16_W(FPGA_FLOW_MEAS_REG, val);

	return 0;
}

int FlowGetMeasData(UINT8 chan, UINT16 *data) {
	if( chan > MAX_FLOW_CHANLLE ) {
		return 1;
	}

	if( g_flow_meas_data[chan].valid == 0 ) {
		return 1;
	}

	data[0] = g_flow_meas_data[chan].datah;
	data[1] = g_flow_meas_data[chan].datal;

	/* clear the valid flag */
	g_flow_meas_data[chan].valid = 0;
	return 0;
}

void FlowIntrHandler(UINT8 bit_pos) {
	UINT16 sts = FPGA_REG16_R(FPGA_FLOW_STATUS_REG);

	if( sts & 0x1 ) {
		g_flow_meas_data[0].datah = FPGA_REG16_R(FPGA_FLOW_TUNNEL1_RESULT_H_REG);
		g_flow_meas_data[0].datal = FPGA_REG16_R(FPGA_FLOW_TUNNEL1_RESULT_L_REG);
		g_flow_meas_data[0].valid = 1;
	}

	if( sts & 0x2 ) {
		g_flow_meas_data[1].datah = FPGA_REG16_R(FPGA_FLOW_TUNNEL2_RESULT_H_REG);
		g_flow_meas_data[1].datal = FPGA_REG16_R(FPGA_FLOW_TUNNEL2_RESULT_L_REG);
		g_flow_meas_data[1].valid = 1;
	}

	if( sts & 0x4 ) {
		g_flow_meas_data[2].datah = FPGA_REG16_R(FPGA_FLOW_TUNNEL3_RESULT_H_REG);
		g_flow_meas_data[2].datal = FPGA_REG16_R(FPGA_FLOW_TUNNEL3_RESULT_L_REG);
		g_flow_meas_data[2].valid = 1;
	}

	if( sts & 0x8 ) {
		g_flow_meas_data[3].datah = FPGA_REG16_R(FPGA_FLOW_TUNNEL3_RESULT_H_REG);
		g_flow_meas_data[3].datal = FPGA_REG16_R(FPGA_FLOW_TUNNEL3_RESULT_L_REG);
		g_flow_meas_data[3].valid = 1;
	}

	/* write b'1 to clear the flag */
	FPGA_REG16_W(FPGA_FLOW_STATUS_REG, sts);
	return 0;
}

int FlowMeasSelfTest(void) {
	u8 i = 0;
	UINT16 val = 0;
	UINT16 data[2];
	UINT16 count = 0;

	FlowMeasInit();
	/* switch to internal self test signal */
	val = ((1 << 3) | ( 1 << 2));
	FPGA_REG16_W(FPGA_FLOW_SWITCH_REG, val);
	
	for( i = 0; i < MAX_FLOW_CHANLLE; ++i ) {
		/* choose chan */
		val &= ~0x3;
		val |= i;
		FPGA_REG16_W(FPGA_FLOW_SWITCH_REG, val);
		FlowMeasStart(i);

		/* wait for measurement complete */
		while( count < 1000 ) {
			if( FlowGetMeasData(i, data) == 0 ) {
				count = 0;
				PRINTF("chan %u measure complete %u %u\n", i, data[0], data[1]);
				break;
			}
			count++;
			Osal_TaskSleep(1);
		}
	}

	/* recover the flow registers */
	FPGA_REG16_W(FPGA_FLOW_SWITCH_REG, 0);
	FPGA_REG16_W(FPGA_FLOW_MEAS_REG, 0);
	FPGA_REG16_W(FPGA_FLOW_STATUS_REG, 0xFFFF);

	return 0;
}

void FlowMeasDump(void) {
	UINT8 i = 0;
	for( i = 0; i < MAX_FLOW_CHANLLE; ++i ) {
		PRINTF("valid=%u datah=%u datal=%u\n", 
			g_flow_meas_data[i].valid,
			g_flow_meas_data[i].datah,
			g_flow_meas_data[i].datal);
	}
}

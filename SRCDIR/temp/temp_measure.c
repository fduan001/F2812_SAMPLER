#include "F2812_datatype.h"
#include "shellconsole.h"
#include "fpga.h"
#include "temp_measure.h"
#include "ADS124S08.h"

int TempMeasInit(void) {

	ADS124S08_Init();
	TempMeasStart();
	PlatformDelay(20);

	ADS1248_SetInputChan(ADS_P_AIN0, ADS_N_AIN1);
	ADS1248_SetReference(0x20, 0x10);
	PlatformDelay(10);
	ADS1248_SetIDAC(ADS_IDAC1_A0, ADS_IDAC2_A1, ADS_IDACMAG_1000);
	ADS1248_SetPGAGainAndDataRate(ADS1248_GAIN_4, ADS1248_DR_320);

	TempMeasStop();
	PlatformDelay(5000);
	return 0;
}

int TempMeasStart(void) {
	// ADS124S08_SendCmd(START_OPCODE_MASK); /* send the START cmd to start converting in continuous conversion mode */ 
	ADS124S08_AssertStart();
	return 0;
}

int TempMeasStop(void) {
	// ADS124S08_SendCmd(STOP_OPCODE_MASK); /* send the START cmd to start converting in continuous conversion mode */ 
	ADS124S08_DeassertStart();
	return 0;
}

int IsTempMeasReady(void) {
	UINT32 i = 0;
	UINT32 limit = 500;
	UINT16 ready = 0;

	while(1) {
		ready = FPGA_REG16_R(FPGA_TEMP_MEAS_STATUS_REG);
		if( ready ) {
			PRINTF("ready to get data i=%ld\n", i);
			break;
		}
		++i;
		if( i >= limit ) {
			PRINTF("Timeout to wait for measurment ready\n");
			return 0;
		}
		PlatformDelay(1000);
	}
	return 1;
}

UINT32 TempGetMeasData(void) {
	UINT8 data[3] = {0, 0, 0};
	UINT8 status = 0;
	UINT8 crc = 0;
	UINT32 result = 0;
	INT32 tmp = 0;
	TempMeasStart();
	PlatformDelay(5000);
	TempMeasStop();
	if( IsTempMeasReady() != 1 ) {
		return 0;
	}

	ADS124S08_ReadDate(&status, data, &crc);
	PRINTF("%02x %02x %02x\n", data[0], data[1], data[2]);
	result = (((UINT32)data[0]) << 16) | (((UINT32)data[1]) << 8) | ((UINT32)data[2]);

	tmp = ((INT32)result) << 8;
	tmp >>= 8;
	PRINTF("tmp=%ld\n",tmp);
	return result;
}

UINT32 TempMeasCalibration(void) {
	UINT32 first = 0;
	TempMeasInit();
	first = TempGetMeasData();
	PRINTF("%ld\n", first);
	return first;
}

void TempMeasDump(void) {

}

#include "F2812_datatype.h"
#include "shellconsole.h"
#include "fpga.h"
#include "temp_measure.h"
#include "ADS124S08.h"

int TempMeasInit(void) {
	TempMeasStop();
	ADS124S08_Init();
	TempMeasStart();
	PlatformDelay(20);

	ADS1248_SetInputChan(ADS_P_AIN0, ADS_N_AIN1);
	ADS1248_SetReference(0x20, 0x00);
	PlatformDelay(10);
	ADS1248_SetIDAC(ADS_IDAC1_A0, ADS_IDAC2_A1, ADS_IDACMAG_1000);
	ADS1248_SetPGAGainAndDataRate(ADS1248_GAIN_8, ADS1248_DR_20);

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
			PRINTF("try %ld loops, ready=%04x\n", i, ready);
			break;
		}
		++i;
		if( i >= limit ) {
			PRINTF("Timeout to wait for measurment ready\n");
			return 0;
		}
		PlatformDelay(1000);
	}

	if( i == 0 ) {
		while(1) {
			ready = FPGA_REG16_R(FPGA_TEMP_MEAS_STATUS_REG);
			if( ready == 0 ) {
				break;
			}
			++i;
			if( i >= limit ) {
				PRINTF("Timeout to wait for measurment ready\n");
				return 0;
			}
			PlatformDelay(1000);
		}
	}
	return 1;
}

UINT32 TempGetMeasData(void) {
	UINT8 data[3] = {0, 0, 0};
	UINT8 status = 0;
	UINT8 crc = 0;
	UINT32 result = 0;

	if( IsTempMeasReady() != 1 ) {
		return 0;
	}

	ADS124S08_ReadDate(&status, data, &crc);
	PRINTF("%02x %02x %02x\n", data[0], data[1], data[2]);
	result = (((UINT32)data[0]) << 16) | (((UINT32)data[1]) << 8) | ((UINT32)data[2]);

	return result;
}

UINT32 TempMeasCalibration(void) {
	UINT32 first = 0, second = 0;
	TempMeasInit();
	// TempMeasStart();
	// ADS124S08_WriteReg(0x2, 0x12); /* select AINP = AIN1 and AINN = AIN2 */
	first = TempGetMeasData();
	// ADS124S08_WriteReg(0x2, 0x23); /* select AINP = AIN2 and AINN = AIN3 */
	second = TempGetMeasData();

	// TempMeasStop();
	PRINTF("%ld %ld\n", first, second);
	return (second - first);
}

void TempMeasDump(void) {

}

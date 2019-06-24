#include "F2812_datatype.h"
#include "shellconsole.h"
#include "fpga.h"
#include "temp_measure.h"
#include "ADS124S08.h"

UINT8 TempSelfCalibration(void) {
	UINT8 i = 0;
	UINT8 maxTry = 3;

	TempMeasStop();
	TempMeasStart();
	ADS1248_SetMuxCal(ADS_NORMAL_OP);
	TempMeasStop();
	PlatformDelay(50000);
	
	while(i++ < maxTry) {
		TempMeasStart();
		PlatformDelay(5000);
		TempMeasStop();
		ADS124S08_SendCmd(ADS_SYSOCAL_CMD);
		PlatformDelay(50000);
		if( IsTempMeasReady() != 1 ) {
			
			continue;
		} else {
			break;
		}
	}

	if( i >= maxTry ) {
		PRINTF("Do SYSOCAL timeout\n");
		return 1;
	}

	TempMeasStart();
	ADS1248_SetMuxCal(ADS_OFFSET_CAL);
	TempMeasStop();
	PlatformDelay(50000);

	i = 0;
	while(i++ < maxTry) {
		TempMeasStart();
		PlatformDelay(5000);
		TempMeasStop();
		ADS124S08_SendCmd(ADS_SYSGCAL_CMD);
		PlatformDelay(50000);
		if( IsTempMeasReady() != 1 ) {
			
			continue;
		}
	}

	if( i >= maxTry ) {
		PRINTF("Do SYSGCAL timeout\n");
		return 1;
	}

	TempMeasStart();
	ADS1248_SetMuxCal(ADS_GAIN_CAL);
	TempMeasStop();
	PlatformDelay(50000);
	
	i = 0;
	while(i++ < maxTry) {
		TempMeasStart();
		PlatformDelay(5000);
		TempMeasStop();
		ADS124S08_SendCmd(ADS_SELFOCAL_CMD);
		PlatformDelay(50000);
		if( IsTempMeasReady() != 1 ) {			
			continue;
		}
	}

	if( i >= maxTry ) {
		PRINTF("Do SELFOCAL timeout\n");
		return 1;
	}

	TempMeasStart();
	ADS1248_SetMuxCal(ADS_NORMAL_OP);
	TempMeasStop();
	PlatformDelay(50000);

	return 0;
}

int TempMeasInit(void) {

	ADS124S08_Init();
	TempMeasStart();
	ADS1248_SetPGAGainAndDataRate(ADS1248_GAIN_4, ADS1248_DR_320);
	TempSelfCalibration();

	TempMeasStart();
	PlatformDelay(20);
	ADS1248_SetInputChan(ADS_P_AIN1, ADS_N_AIN2);
	ADS1248_SetReference(0x20, 0x10);
	PlatformDelay(10);
	ADS1248_SetIDAC(ADS_IDAC1_A0, ADS_IDAC2_OFF, ADS_IDACMAG_1000);
	ADS1248_SetPGAGainAndDataRate(ADS1248_GAIN_4, ADS1248_DR_320);
	ADS1248_SetMuxCal(ADS_NORMAL_OP);

	TempMeasStop();
	PlatformDelay(5000);
	return 0;
}

int TempMeasStart(void) {
	// ADS124S08_SendCmd(START_OPCODE_MASK); /* send the START cmd to start converting in continuous conversion mode */ 
	ADS124S08_AssertStart();
	FPGA_REG16_W(FPGA_TEMP_MEAS_STATUS_REG, 0);
	return 0;
}

int TempMeasStop(void) {
	// ADS124S08_SendCmd(STOP_OPCODE_MASK); /* send the START cmd to start converting in continuous conversion mode */ 
	ADS124S08_DeassertStart();
	FPGA_REG16_W(FPGA_TEMP_MEAS_STATUS_REG, 0);
	return 0;
}

int IsTempMeasReady(void) {
	UINT32 i = 0;
	UINT32 limit = 800;
	UINT16 ready = 0;

	while(1) {
		ready = FPGA_REG16_R(FPGA_TEMP_MEAS_STATUS_REG);
		if( ready ) {
			PRINTF("ready to get data i=%ld\n", i);
			FPGA_REG16_W(FPGA_TEMP_MEAS_STATUS_REG, 0);
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
	UINT8 maxTry = 3;
	UINT8 i = 0;

	while(i < maxTry ) {
		FPGA_REG16_W(FPGA_TEMP_MEAS_STATUS_REG, 0);
		TempMeasStart();
		PlatformDelay(5000);
		TempMeasStop();
		PlatformDelay(5000);
		if( IsTempMeasReady() != 1 ) {
			++i;
			continue;
		}

		ADS124S08_ReadDate(&status, data, &crc);
		PRINTF("%02x %02x %02x\n", data[0], data[1], data[2]);
		result = (((UINT32)data[0]) << 16) | (((UINT32)data[1]) << 8) | ((UINT32)data[2]);
		if( result != 0 ) {
			break;
		}
	}

	if( result == 0 ) {
		return 0;
	}

	tmp = data[0];
	tmp = tmp * 256 + data[1];
	tmp = tmp * 256 + data[2];
	tmp = tmp * 256;
	tmp = tmp / 256;
	PRINTF("tmp=%ld\n",tmp);
	return tmp;
}

float TempMeasCalibration(void) {
	UINT32 dacVRTD;
	UINT32 dacVREF1st;
	UINT32 dacVRLead;
	UINT32 dacVREF2nd;
	float rRTD = 0;
	float rLead3 = 0;
	float rREF = 1620.0; /* 1.62K */

	TempMeasInit();
	
	TempMeasStart();
	ADS1248_SetMuxCal(ADS_NORMAL_OP);
	TempMeasStop();
	PlatformDelay(50000);

	dacVRTD = TempGetMeasData();

	/* test vref */
	TempMeasStart();
	ADS1248_SetMuxCal(ADS_REF0_MON);
	TempMeasStop();
	PlatformDelay(50000);
	dacVREF1st = TempGetMeasData();
	PRINTF("%ld %ld\n", dacVRTD, dacVREF1st);

	TempMeasStart();
	ADS1248_SetInputChan(ADS_P_AIN2, ADS_N_AIN3);
	ADS1248_SetMuxCal(ADS_NORMAL_OP);
	TempMeasStop();
	PlatformDelay(50000);
	dacVRLead = TempGetMeasData();

	/* test vref again */
	TempMeasStart();
	ADS1248_SetMuxCal(ADS_REF0_MON);
	TempMeasStop();
	PlatformDelay(50000);
	dacVREF2nd = TempGetMeasData();
	PRINTF("%ld %ld\n", dacVRLead, dacVREF2nd);

	rRTD = (float)rREF / ((float)dacVREF1st/(float)dacVRTD * 16);
	rLead3 = (float)rREF / ((float)dacVREF2nd/(float)dacVRLead * 16);
	PRINTF("Rrtd=%f Rlead3=%f\n", rRTD, rLead3);
	return (rRTD - rLead3);
}

void TempMeasDump(void) {

}

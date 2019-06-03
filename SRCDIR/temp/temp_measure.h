#ifndef TEMP_MEASURE_H
#define TEMP_MEASURE_H

int TempMeasInit(void);
int TempMeasStart(void);
int TempMeasStop(void);
UINT32 TempMeasCalibration(void);
void TempMeasDump(void);

#endif

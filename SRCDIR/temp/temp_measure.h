#ifndef TEMP_MEASURE_H
#define TEMP_MEASURE_H

int TempMeasInit(void);
int TempMeasStart(void);
int TempMeasStop(void);
float TempMeasCalibration(void);
void TempMeasDump(void);

#endif

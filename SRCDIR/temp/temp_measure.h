#ifndef TEMP_MEASURE_H
#define TEMP_MEASURE_H

int TempMeasInit(void);
int TempMeasStart(UINT8 chan);
int TempGetMeasData(UINT8 chan, UINT16 *data);
void TempIntrHandler(UINT8 bit_pos);
int TempMeasSelfTest(void);
void TempMeasDump(void);

#endif

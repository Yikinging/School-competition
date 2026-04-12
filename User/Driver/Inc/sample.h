#ifndef __SMAPLE_H
#define __SMAPLE_H

#include <stdint.h>

#define SAMPLE_ADCMainHANDLE (&hadc2)
#define SAMPLE_ADCCalibrationHANDLE (&hadc1)

#define SAMPLE_VoltageReference 1.2f

#define SAMPLE_BufferSize 5
extern uint16_t SAMPLE_Buffer[SAMPLE_BufferSize];

typedef enum {
    Sample_VacIndex = 0,
    Sample_IacIndex,
    Sample_VdcIndex,
    Sample_IdcIndex,
    Sample_VbusIndex
} Sample_IndexTypeDef;

void Sample_Init(void);
void Sample_DeInit(void);

uint16_t Sample_GetRaw(Sample_IndexTypeDef index);
uint16_t Sample_GetVrefRaw(void);
float Sample_GetPinVoltage(Sample_IndexTypeDef index);
float Sample_GetVdda(void);

float Sample_GetVacTheoretical(void);
float Sample_GetIacTheoretical(void);
float Sample_GetVdcTheoretical(void);
float Sample_GetIdcTheoretical(void);
float Sample_GetVbusTheoretical(void);

float Sample_CorrectVac(float theoryValue);
float Sample_CorrectIac(float theoryValue);
float Sample_CorrectVdc(float theoryValue);
float Sample_CorrectIdc(float theoryValue);
float Sample_CorrectVbus(float theoryValue);

float Sample_GetVacActural(void);
float Sample_GetIacActural(void);
float Sample_GetVdcActural(void);
float Sample_GetIdcActural(void);
float Sample_GetVbusActural(void);

#endif /* __SMAPLE_H */

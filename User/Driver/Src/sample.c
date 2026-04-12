#include "sample.h"
#include "main.h"
#include "adc.h"

// SAMPLE_GetStandardVoltageRaw() is defined as a macro in sample.h
uint16_t SAMPLE_Buffer[SAMPLE_BufferSize];

void Sample_Init(void) {
    // // ADC calibration
    // if (HAL_ADC_Calibration_Start(SAMPLE_ADCMainHANDLE) != HAL_OK) {
    //     Error_Handler();
    // }
    // if (HAL_ADC_Calibration_Start(SAMPLE_ADCCalibrationHANDLE) != HAL_OK) {
    //     Error_Handler();
    // }

    // ADC start
    HAL_ADC_Start_DMA(SAMPLE_ADCMainHANDLE, (uint32_t*)SAMPLE_Buffer, SAMPLE_BufferSize);
    HAL_ADC_Start(SAMPLE_ADCCalibrationHANDLE);

    //verify vref  be prepare
    Sample_GetVrefRaw();
}

void Sample_DeInit(void) {
    // ADC stop
    HAL_ADC_Stop_DMA(SAMPLE_ADCMainHANDLE);
    HAL_ADC_Stop(SAMPLE_ADCCalibrationHANDLE);
}

uint16_t Sample_GetRaw(Sample_IndexTypeDef index) {
    if (index >= SAMPLE_BufferSize) {
        return 0;
    }
    return SAMPLE_Buffer[index];
}

uint16_t Sample_GetVrefRaw(void){
    uint16_t value = HAL_ADC_GetValue(SAMPLE_ADCCalibrationHANDLE);
    
    // Safety policy: fail-stop until ADC1 Vrefint becomes valid; system must not run with invalid reference.
    while( value == 0) {
        value = HAL_ADC_GetValue(SAMPLE_ADCCalibrationHANDLE);
    }
    return value;
}

float Sample_GetPinVoltage(Sample_IndexTypeDef index) {
    uint16_t raw = Sample_GetRaw(index);

    //Correction
    float voltage = (float)raw * SAMPLE_VoltageReference / (float)Sample_GetVrefRaw();

    return voltage;
}

float Sample_GetVdda(void) {
    uint16_t vrefRaw = Sample_GetVrefRaw();
    return SAMPLE_VoltageReference * 4096.0f / (float)vrefRaw;
}

// Need  to be defined by hardware design
float Sample_GetVacTheoretical(void){return Sample_GetPinVoltage(Sample_VacIndex) ;}
float Sample_GetIacTheoretical(void){return Sample_GetPinVoltage(Sample_IacIndex) ;}
float Sample_GetVdcTheoretical(void){return Sample_GetPinVoltage(Sample_VdcIndex) ;}
float Sample_GetIdcTheoretical(void){return Sample_GetPinVoltage(Sample_IdcIndex) ;}
float Sample_GetVbusTheoretical(void){return Sample_GetPinVoltage(Sample_VbusIndex) ;}

__weak float Sample_CorrectVac(float theoryValue){return theoryValue;}
__weak float Sample_CorrectIac(float theoryValue){return theoryValue;}
__weak float Sample_CorrectVdc(float theoryValue){return theoryValue;}
__weak float Sample_CorrectIdc(float theoryValue){return theoryValue;}
__weak float Sample_CorrectVbus(float theoryValue){return theoryValue;}

float Sample_GetVacActural(void){return Sample_CorrectVac(Sample_GetVacTheoretical());}
float Sample_GetIacActural(void){return Sample_CorrectIac(Sample_GetIacTheoretical());}
float Sample_GetVdcActural(void){return Sample_CorrectVdc(Sample_GetVdcTheoretical());}
float Sample_GetIdcActural(void){return Sample_CorrectIdc(Sample_GetIdcTheoretical());}
float Sample_GetVbusActural(void){return Sample_CorrectVbus(Sample_GetVbusTheoretical());}


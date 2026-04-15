#ifndef __POWER_SAMPLE_H
#define __POWER_SAMPLE_H

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==============================================================================
 * 1. 硬件句柄映射 (Handle Mapping)
 * ============================================================================== */
#define PWR_ADC_VREF_HDL    (&hadc1)
#define PWR_ADC_DATA_HDL    (&hadc2)
#define PWR_DMA_BUF_SIZE    5

/* ==============================================================================
 * 2. 物理参数配置 (Physical Configuration)
 * ============================================================================== */
#define ADC_MAX_VALUE       4095.0f
#define PWR_VREFINT_CAL_ADDR    ((uint16_t*) (0x1FFF7A2A))

/* 电压分压比例 (V_in = V_pin * RATIO) */
#define RATIO_VDC           4.9f      
#define RATIO_VAC           (1000.0f / 27.0f) 
#define RATIO_VBUS          21.0f  

/* 电流传感器灵敏度 (I = V_diff / SENSE) */
#define SENSE_IDC           (50.0f * 0.05f) // = 2.50 V/A
#define SENSE_IAC           (20.0f * 0.02f) // = 0.40 V/A

/* ==============================================================================
 * 3. API 接口声明 (API Declarations)
 * ============================================================================== */
void Power_Sample_Init(void);
void Power_Sample_Update(void); // 在 while(1) 中调用

/* 实际物理值 (单位: V 或 A) */
float Power_Get_VDDA_Actual(void);
float Power_Get_VBUS_Actual(void);
float Power_Get_VDC_Actual(void);
float Power_Get_VAC_Actual(void);
float Power_Get_IDC_Actual(void);
float Power_Get_IAC_Actual(void);

/* 原始数据获取 (调试用) */
uint16_t Power_Get_Raw_VDC(void);

/* 软件修正接口 (Weak) */
float Power_Correct_VBUS(float theory_val);
float Power_Correct_VDC(float theory_val);
float Power_Correct_VAC(float theory_val);
float Power_Correct_IDC(float theory_val);
float Power_Correct_IAC(float theory_val);

#ifdef __cplusplus
}
#endif
#endif

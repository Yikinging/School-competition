#ifndef __ADC_Vref_H
#define __ADC_Vref_H

#include "main.h"

/* ========================================================
 * [1] 硬件参数宏定义
 * ======================================================== */
// STM32F4 出厂 Vrefint 校准值地址 (Condition: Vrefint:1.21V(typical))
#ifndef VREFINT_CAL_ADDR
#define VREFINT_CAL_ADDR ((uint16_t*)0x1FFF7A2A)
#endif
// --- 电压分压比例 (实际电压 = ADC引脚电压 * 比例) ---
// 母线 (VBUS): R7(200k) + R8(10k) 
#define RATIO_VBUS    21.0f    
// 输出 (VDC): R9(39k) + R10(10k)
#define RATIO_VDC     4.9f     
// 交流 (VAC): 差分运放放大倍数: 0.027 (R13/R14)的倒数 (1/0.027)
#define RATIO_VAC     37.037f  

// --- 电流采样灵敏度 (V/A) (ADC引脚电压 = 实际电流 * 灵敏度) ---
// 输出电流 (IDC): INA240A2 增益 50V/V, 检流电阻 50mΩ -> 50 * 0.05
#define SENSE_IDC     2.5f     
// 交流电流 (IAC): INA240A1 增益 20V/V, 检流电阻 50mΩ -> 20 * 0.05
#define SENSE_IAC     1.0f

/* ========================================================
 * [2] 核心数据结构体
 * ======================================================== */
typedef struct {
    float Sample_V_Bus;  // 48V 母线直流电压 (V)
    float Sample_V_DC;   // 3~12V 输出直流电压 (V)
    float Sample_I_DC;   // 输出直流电流 (A)
    float Sample_V_AC;   // 28V 交流输入电压 (V)
    float Sample_I_AC;   // 交流输入电流 (A)
    
    float Vdda;   // 实时反推的单片机模拟供电电压 (V)，用于监控温漂。 实验参考电压(Vdda) = 3.3V * 厂家校准值/实验采样值
} PowerSystemData_t;
// 声明外部变量
extern PowerSystemData_t sysData;

/* ========================================================
 * [3] 外部函数接口
 * ======================================================== */
// 启动 ADC 采样 (DMA 及外设)
void ADC_Vref_Start(void);

// 在主循环中调用此函数，刷新并计算所有的电压电流值
void ADC_Read_And_Calculate(void);

#endif /* __ADC_Vref_H */

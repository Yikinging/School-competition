#include "ADC_Vref.h"

// 引入 CubeMX 生成的外设句柄
extern ADC_HandleTypeDef hadc1; // 负责 Vrefint (规则组)
extern ADC_HandleTypeDef hadc2; // 负责 外部5个通道 (规则组 + DMA)

/* ========================================================
 * [1] 内部私有变量
 * ======================================================== */
// ADC2 规则组 DMA 接收缓冲区 (大小为5)
// 【极其重要】以下代码默认 CubeMX 中的 Rank 顺序为：
// Rank1=VAC, Rank2=IAC, Rank3=VDC, Rank4=IDC, Rank5=VBUS
volatile uint16_t adc2_dma_buf[5]; 

// 实例化对外暴露的数据结构
PowerSystemData_t sysData;

/* ========================================================
 * [2] 接口函数实现
 * ======================================================== */

/**
 * @brief  启动电源 ADC 采样 
 * @note   在 main 函数的 while(1) 之前调用一次即可
 */
void ADC_Vref_Start(void)
{
    // 启动 ADC2 规则组 DMA，并捕捉它的返回值
    HAL_StatusTypeDef status = HAL_ADC_Start_DMA(&hadc2, (uint32_t*)adc2_dma_buf, 5);
    
    // 如果它返回的不是 HAL_OK (0)，说明底层配置有严重 Bug！
    if (status != HAL_OK)
    {
        char err_buf[32];
        sprintf(err_buf, "DMA ERR: %d", status);
        OLED_ShowString(0, 0, err_buf); // 把错误码贴在屏幕最上面
        OLED_GFX_Refresh();
        
        while(1); // 故意死机在这里，让你看清楚屏幕上的报错！
    }
}

/**
 * @brief  核心物理量读取与换算 (包含动态温漂校准)
 * @note   请在 main.c 的 while(1) 循环中定时调用此函数
 */
void ADC_Read_And_Calculate(void)
{
    uint16_t raw_Vrefint = 0;
    
    // ----------------------------------------------------
    // 1. 软件触发读取 ADC1 的 Vrefint (用于校准基准电压)
    // ----------------------------------------------------
    HAL_ADC_Start(&hadc1); 
    // 等待转换完成，超时时间 10ms
    if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) 
    {
        raw_Vrefint = HAL_ADC_GetValue(&hadc1);
    }
    HAL_ADC_Stop(&hadc1);

    // ----------------------------------------------------
    // 2. 从 ADC2 的 DMA 数组中极速提取 5 个外部通道数据
    // ----------------------------------------------------
    uint16_t raw_VAC  = adc2_dma_buf[0]; // 对应 Rank 1
    uint16_t raw_IAC  = adc2_dma_buf[1]; // 对应 Rank 2
    uint16_t raw_VDC  = adc2_dma_buf[2]; // 对应 Rank 3
    uint16_t raw_IDC  = adc2_dma_buf[3]; // 对应 Rank 4
    uint16_t raw_VBUS = adc2_dma_buf[4]; // 对应 Rank 5

    // ----------------------------------------------------
    // 3. 开始高精度浮点数学运算
    // ----------------------------------------------------
    // 步骤 A: 动态反推此刻真实的 VDDA (消除 LDO 发热带来的温漂)
    if(raw_Vrefint > 0) {
        sysData.Vdda = (3.3f * (*VREFINT_CAL_ADDR)) / raw_Vrefint;
    } else {
        sysData.Vdda = 3.3f; // 防呆保护，防止除以 0
    }

    // 预计算乘法因子，提升运算效率
    float adc_to_volt = sysData.Vdda / 4095.0f;
    float v_bias = sysData.Vdda / 2.0f; // 动态跟随的 1.65V 零点偏置

    // 步骤 B: 直流量计算 (纯比例映射)
    sysData.Sample_V_DC  = (raw_VDC  * adc_to_volt) * RATIO_VDC;
    sysData.Sample_V_Bus = (raw_VBUS * adc_to_volt) * RATIO_VBUS;
    sysData.Sample_I_DC  = (raw_IDC  * adc_to_volt) / SENSE_IDC;

    // 步骤 C: 交流量计算 (必须扣除动态零点偏置，还原正负交变波形)
    sysData.Sample_I_AC = (raw_IAC * adc_to_volt - v_bias) / SENSE_IAC;
    sysData.Sample_V_AC = (raw_VAC * adc_to_volt - v_bias) * RATIO_VAC;
}

#include "Power_Sample.h"
#include "adc.h"

/* 内部私有变量 */
static volatile uint16_t g_pwr_raw_buf[PWR_DMA_BUF_SIZE];
static float g_vdda = 3.3f; //  3.3V

/* 弱函数默认实现 */
__weak float Power_Correct_VDC(float val) { return val; }
__weak float Power_Correct_VAC(float val) { return val; }
__weak float Power_Correct_IDC(float val) { return val; }
__weak float Power_Correct_IAC(float val) { return val; }
__weak float Power_Correct_VBUS(float val) { return val; }

/* ==============================================================================
 * 1. 初始化 (启动硬件级自动化)
 * ============================================================================== */
void Power_Sample_Init(void)
{
    // 1. 启动 ADC2 的 DMA 循环搬运 (处理 5 个外部通道)
    // 即使 ADC2 也是定时器触发，DMA 会自动在每次触发后搬运数据
    HAL_ADC_Start_DMA(PWR_ADC_DATA_HDL, (uint32_t*)g_pwr_raw_buf, PWR_DMA_BUF_SIZE);
    
    // 2. 启动 ADC1 的 中断模式 (处理 Vrefint 校准)
    // 只要硬件定时器一发信号，ADC1 就会采样，采完自动进中断
    HAL_ADC_Start_IT(PWR_ADC_VREF_HDL); 
}

/* ==============================================================================
 * 2. 中断回调函数 (这是硬件触发的核心枢纽！)
 * 当定时器触发 ADC1 采样完成时，单片机会全自动跳进这个函数
 * ============================================================================== */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    // 判断是不是 ADC1 (Vrefint) 转换完成了
    if (hadc->Instance == ADC1)
    {
        uint32_t vref_raw = HAL_ADC_GetValue(PWR_ADC_VREF_HDL);
        if (vref_raw > 0)
        {
          
            // 公式: 真实VDDA = 3.3V * (出厂标定ADC值 / 当前读到的ADC值)
            float vdda_inst = 3.3f * (float)(*PWR_VREFINT_CAL_ADDR) / (float)vref_raw;
            
            g_vdda = g_vdda + (vdda_inst - g_vdda) * 0.1f; // 低通滤波
        }
    }
}

// 因为现在是全自动中断更新，所以 Update 函数可以直接空置
void Power_Sample_Update(void) 
{ 
    // 由于是全自动中断更新，此函数无需实现
}

/* ==============================================================================
 * 3. 理论计算层 & 外部 API (保持完全不变)
 * ============================================================================== */
static float Get_VDC_Theoretical(void)
{
    float v_pin = (g_pwr_raw_buf[2] / ADC_MAX_VALUE) * g_vdda;
    return v_pin * RATIO_VDC;
}

static float Get_VAC_Theoretical(void)
{
    float v_pin = (g_pwr_raw_buf[0] / ADC_MAX_VALUE) * g_vdda;
    float v_bias = g_vdda / 2.0f; 
    return (v_pin - v_bias) * RATIO_VAC;
}

static float Get_IDC_Theoretical(void)
{
    float v_pin = (g_pwr_raw_buf[3] / ADC_MAX_VALUE) * g_vdda;
    return v_pin / SENSE_IDC;
}

static float Get_IAC_Theoretical(void)
{
    float v_pin = (g_pwr_raw_buf[1] / ADC_MAX_VALUE) * g_vdda;
    float v_bias = g_vdda / 2.0f;
    return (v_pin - v_bias) / SENSE_IAC;
}
static float Get_VBUS_Theoretical(void)
{
    float v_pin = (g_pwr_raw_buf[4] / ADC_MAX_VALUE) * g_vdda;
    return v_pin * RATIO_VBUS;
}
float Power_Get_VDC_Actual(void) { return Power_Correct_VDC(Get_VDC_Theoretical()); }
float Power_Get_VAC_Actual(void) { return Power_Correct_VAC(Get_VAC_Theoretical()); }
float Power_Get_IDC_Actual(void) { return Power_Correct_IDC(Get_IDC_Theoretical()); }
float Power_Get_IAC_Actual(void) { return Power_Correct_IAC(Get_IAC_Theoretical()); }
float Power_Get_VDDA_Actual(void) { return g_vdda; }
float Power_Get_VBUS_Actual(void) { return Power_Correct_VBUS(Get_VBUS_Theoretical()); }

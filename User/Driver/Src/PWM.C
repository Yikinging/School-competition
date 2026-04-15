#include "PWM.h"
#include "main.h"

#define PWM_ADCTriggerChannel TIM_CHANNEL_2

void PWM_Init(void){
    __HAL_TIM_SET_COMPARE(PWM_TIMHandle, PWM_PFCChannel, 0);
    __HAL_TIM_SET_COMPARE(PWM_TIMHandle, PWM_BuckChannel, 0);
    __HAL_TIM_SET_COMPARE(PWM_TIMHandle, PWM_ADCTriggerChannel, PWM_ARRPeriod/2); 
    
    // HAL_TIM_PWM_Start(PWM_TIMHandle, PWM_PFCChannel);
    // HAL_TIMEx_PWMN_Start(PWM_TIMHandle, PWM_PFCChannel);
    // HAL_TIM_PWM_Start(PWM_TIMHandle, PWM_BuckChannel);
    // HAL_TIMEx_PWMN_Start(PWM_TIMHandle, PWM_BuckChannel);
    
    __HAL_TIM_MOE_DISABLE(PWM_TIMHandle);
    HAL_TIM_OC_Start(PWM_TIMHandle, PWM_ADCTriggerChannel);

    // __HAL_TIM_MOE_ENABLE(PWM_TIMHandle);
}

void PWM_DeInit(void){
    HAL_TIM_PWM_Stop(PWM_TIMHandle, PWM_PFCChannel);
    HAL_TIMEx_PWMN_Stop(PWM_TIMHandle, PWM_PFCChannel);
    HAL_TIM_PWM_Stop(PWM_TIMHandle, PWM_BuckChannel);
    HAL_TIMEx_PWMN_Stop(PWM_TIMHandle, PWM_BuckChannel);
    HAL_TIM_OC_Stop(PWM_TIMHandle, PWM_ADCTriggerChannel);

    __HAL_TIM_MOE_DISABLE(PWM_TIMHandle);
}

void PWM_Start(PWM_ChannelTypeDef channel){
    __HAL_TIM_MOE_ENABLE(PWM_TIMHandle);
    if(channel == PWM_PFCChannel || channel == PWM_BuckChannel){
        HAL_TIM_PWM_Start(PWM_TIMHandle, channel);
        HAL_TIMEx_PWMN_Start(PWM_TIMHandle, channel);
    }
}

void PWM_Stop(PWM_ChannelTypeDef channel){
    if(channel == PWM_PFCChannel || channel == PWM_BuckChannel){
        HAL_TIM_PWM_Stop(PWM_TIMHandle, channel);
        HAL_TIMEx_PWMN_Stop(PWM_TIMHandle, channel);
    }
}

void PWM_UrgencyStop(void){
    __HAL_TIM_MOE_DISABLE(PWM_TIMHandle);
    HAL_TIM_PWM_Stop(PWM_TIMHandle, PWM_PFCChannel);
    HAL_TIMEx_PWMN_Stop(PWM_TIMHandle, PWM_PFCChannel);
    HAL_TIM_PWM_Stop(PWM_TIMHandle, PWM_BuckChannel);
    HAL_TIMEx_PWMN_Stop(PWM_TIMHandle, PWM_BuckChannel);
}

void PWM_SetCCR(PWM_ChannelTypeDef channel, uint16_t ccr){
    __HAL_TIM_SET_COMPARE(PWM_TIMHandle, channel, ccr);
}

void PWM_SetDuty(PWM_ChannelTypeDef channel, float duty){
    __HAL_TIM_SET_COMPARE(PWM_TIMHandle, channel, (uint32_t)(duty * PWM_ARRPeriod));
}

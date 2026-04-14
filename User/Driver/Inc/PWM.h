#ifndef __PWM_H
#define __PWM_H

#include "tim.h"

#define PWM_TIMHandle   (&htim1)

#define PWM_ARRPeriod   (__HAL_TIM_GET_AUTORELOAD(PWM_TIMHandle))

typedef enum {
    PWM_PFCChannel = TIM_CHANNEL_1,
    PWM_BuckChannel = TIM_CHANNEL_3
}PWM_ChannelTypeDef;

void PWM_Init(void);
void PWM_DeInit(void);
void PWM_Start(PWM_ChannelTypeDef channel);
void PWM_Stop(PWM_ChannelTypeDef channel);
void PWM_UrgencyStop(void);
void PWM_SetCCR(PWM_ChannelTypeDef channel, uint16_t ccr);
void PWM_SetDuty(PWM_ChannelTypeDef channel, float duty);

#endif /* __PWM_H */

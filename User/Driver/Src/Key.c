#include "Key.h"
#include "main.h"

 void Key_Init(void){
 }

void Key_DeInit(void){
}

uint8_t Key_EncoderDIsPressed(void){
    return (HAL_GPIO_ReadPin(EncoderD_GPIO_Port, EncoderD_Pin) == GPIO_PIN_RESET) ? 1U : 0U;
}

uint8_t Key_SW1IsPressed(void){
    return (HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin) == GPIO_PIN_RESET) ? 1U : 0U;
}

uint8_t Key_SW2IsPressed(void){
    return (HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin) == GPIO_PIN_RESET) ? 1U : 0U;
}

#include "joypad.h"

uint8_t getJoypadVals(uint8_t *joy_vals){
  HAL_Delay(500);
  HAL_ADC_Start(&hadc1);
  HAL_ADC_PollForConversion(&hadc1,1);
  joy_vals[0] = (uint8_t) (HAL_ADC_GetValue(&hadc1)>>4);

  HAL_ADC_Start(&hadc1);
  HAL_ADC_PollForConversion(&hadc1,1);
  joy_vals[1] = (uint8_t) (HAL_ADC_GetValue(&hadc1)>>4);

  return RETURN_OK;
}
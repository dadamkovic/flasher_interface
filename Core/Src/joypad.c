/**
 * @file joypad.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-21
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "joypad.h"


/**
 * @brief Runs ADC connected to joypad lines
 * 
 * @param joy_vals Array of 2 uint8_t values [x,y]
 * @return uint8_t RETURN_OK
 */
uint8_t joyGetVals(uint8_t *joy_vals){
  HAL_ADC_Start(&hadc1);
  HAL_ADC_PollForConversion(&hadc1,1);
  joy_vals[0] = (uint8_t) (HAL_ADC_GetValue(&hadc1)>>4);

  HAL_ADC_Start(&hadc1);
  HAL_ADC_PollForConversion(&hadc1,1);
  joy_vals[1] = (uint8_t) (HAL_ADC_GetValue(&hadc1)>>4);

  return RETURN_OK;
}
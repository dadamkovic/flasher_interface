/**
 * @file utils.c
 * @author Daniel Adamkovic
 * @brief 
 * @version 0.1
 * @date 2023-01-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "utils.h"

/**
 * @brief Converts float to string with precison of 2 decimal places. Can only convert
 *        numbers that are up to 12 unit points long
 * 
 * @param x float to be converted
 * @param p pointer to array where the converted float is to be strored
 */
char *floatToChar(float x, char *p) {
    char temp_storage[16] = {0}; // go to end of buffer
    char *s = (char*) (temp_storage + sizeof(temp_storage) - 1);
    uint16_t decimals;  // variable to store the decimals
    uint16_t units;  // variable to store the units (part to left of decimal place)
    if (x < 0) { // take care of negative numbers
        decimals = (uint32_t)(x * -100) % 100; // make 1000 for 3 decimals etc.
        units = (uint16_t)(-1 * x);
    } else { // positive numbers
        decimals = (uint32_t)(x * 100) % 100;
        units = (uint16_t)x;
    }

    *--s = (decimals % 10) + '0';
    decimals /= 10; // repeat for as many decimal places as you need
    *--s = (decimals % 10) + '0';
    *--s = '.';

    do  {
        *--s = (units % 10) + '0';
        units /= 10;
    } while(units > 0);
    if (x < 0) *--s = '-'; // unary minus sign for negative numbers
    copyString(p, s);
    return p;
}


/**
 * @brief Simple wrapper around the strncpy to copy according to the string size
 * 
 * @param dest Pointer to destiantion
 * @param cont Pointer to content
 * @return uint8_t RETURN_OK
 */
uint8_t copyString(char *dest, const char *cont){
    uint8_t cont_size = strlen(cont)+1;
    strncpy(dest,cont,cont_size);
    
    return RETURN_OK;
}


/**
 * @brief Wrapper that checks how much data is in buffer, sends it and clears afterwards
 * 
 * @param uart_h UART_HandleTypeDef handle
 * @param data Pointer to char array with data to send
 * @return uint8_t RETURN_FAIL | RETURN_OK
 */
uint8_t uartSendData(UART_HandleTypeDef *uart_h, char *data){

  uint16_t data_len = strlen(data) + 1; //adding one for \0

  if(HAL_OK != HAL_UART_Transmit(uart_h, data, data_len, 100)){
    return RETURN_FAIL;
  };

  //clear the buffer for the next use
  memset(data, 0, data_len);

  return RETURN_OK;
}


/**
 * @brief Fetches data as long as it keeps comming or runs our of space
 * 
 * @param uart_h 
 * @param data 
 * @return uint8_t 
 */
uint8_t uartGetData(UART_HandleTypeDef *uart_h, char *data){

  uint16_t max_len = sizeof(data);
  uint16_t curr_byte = 0;
  char tmp = '\0';
  
  //clear the buffer before reception
  memset(data, 0, max_len);

  while(curr_byte < (max_len-1)){
    if(HAL_OK != HAL_UART_Receive(uart_h, &tmp, 1, 100)){
        return RETURN_FAIL;
    }

    if(tmp == '\n'){
      *(data+curr_byte) = '\0';
      break;
    }
    else{
        *(data+curr_byte) = tmp; 
    }
  }
    return RETURN_OK;
}

  

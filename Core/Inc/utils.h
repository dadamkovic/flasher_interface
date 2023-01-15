#ifndef __UTILS_H__
#define __UTILS_H__


#include "main.h"

char *floatToChar(float x, char *p);
uint8_t copyString(char *, const char *);

uint8_t uartSendData(UART_HandleTypeDef *, char *);
uint8_t uartGetData(UART_HandleTypeDef *, char *);

#endif
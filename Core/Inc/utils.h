#ifndef __UTILS_H__
#define __UTILS_H__


#include "main.h"
#include "joypad.h"
#include "menu.h"
#include "st7735.h"
#include "logger.h"
#include "usart.h"
#include <string.h>

char *floatToChar(float x, char *p);
uint8_t copyString(char *, const char *);

uint8_t uartSendData(UART_HandleTypeDef *, char *);
uint8_t uartGetData(UART_HandleTypeDef *, char *);

JOY_ReturnTypeDef userJoyGetVals(int16_t *);
MENU_HandleTypeDef utilsMenuInit();

MENU_ButtonTypeDef utilsMenuGetSelect();
MENU_ButtonTypeDef utilsMenuGetCancel();
MENU_ReturnTypeDef utilsDrawScreen(MENU_HandleTypeDef);
MENU_ReturnTypeDef utilsDisplayClear();
LOG_StatusTypeDef utilsWriteLog(char *);


#endif
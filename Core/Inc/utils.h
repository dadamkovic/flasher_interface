#ifndef __UTILS_H__
#define __UTILS_H__


#include "main.h"
#include "joypad.h"
#include "menu.h"
#include "st7735.h"

#include "usart.h"
#include <string.h>
#include "plotter.h"

#ifdef LOG_DEBUG
#include "logger.h"
#endif 

#define UTILS_COMM_TIMEOUT 500 

uint8_t copyString(char *, const char *);

uint8_t uartSendData(UART_HandleTypeDef *, char *);
uint8_t uartGetData(UART_HandleTypeDef *, char *);

JOY_ReturnTypeDef userJoyGetVals(int16_t *);
MENU_HandleTypeDef utilsMenuInit();

MENU_ReturnTypeDef utilsDrawScreen(MENU_HandleTypeDef);
MENU_ReturnTypeDef utilsDisplayClear();


PLOT_ReturnTypeDef utilsPlotWrite(char *);
PLOT_ReturnTypeDef utilsPlotRead(char *);
PLOT_ReturnTypeDef utilsPlotOpen(PLOT_HandletypeDef);

#ifdef LOG_DEBUG
LOG_StatusTypeDef utilsWriteLog(char *);
#endif

#endif
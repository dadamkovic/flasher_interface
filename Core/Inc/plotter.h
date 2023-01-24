/**
 * @file plotter.h
 * @author Daniel Adamkovic (dadamkovic@protonmail.ch)
 * @brief 
 * @version 0.1
 * @date 24-01-2023
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __PLOTTER_H__
#define __PLOTTER_H__

#ifdef LOG_DEBUG
#include "logger.h"
#endif

#include "stm32f1xx_hal.h"
#include <string.h>
//#include <stdint.h>
#include <stdio.h>


#define PLOT_MAX_PLOTTERS 8
#define PLOT_ERROR_HANDLE -1


typedef enum{
    PLOT_OK,
    PLOT_FAIL
} PLOT_ReturnTypeDef;

typedef enum{
    PLOT_H_FREE,
    PLOT_H_USED
} PLOT_InitTypeDef;

typedef enum{
    PLOT_FASTMOVE_COM,
    PLOT_LIN_MOVE_COM,
    PLOT_TOGGLE_Z,
    PLOT_RAISE_Z,
    PLOT_DROP_Z
} PLOT_CommandTypeDef;

typedef enum{
    PLOT_NO_WAIT_FIN,
    PLOT_WAIT_FIN
} PLOT_WaitTypeDef;

typedef enum{
    PLOT_Z_UNDEF,
    PLOT_Z_UP,
    PLOT_Z_DOWN
} PLOT_ZStatusTypeDef;

typedef enum{
    PLOT_ABS_MODE,
    PLOT_REL_MODE
} PLOT_MoveModeTypeDef;

typedef enum{
    PLOT_NO_FREE_HANDLE_ERR,
    PLOT_NULL_FUNC_ERR,
    PLOT_DATA_SIZE_ERR
} PLOT_ErrorTypeDef;

typedef struct{
    float x;
    float y;
    PLOT_ZStatusTypeDef z;
}PLOT_PositionTypeDef;


typedef int8_t PLOT_HandletypeDef ;
typedef PLOT_ReturnTypeDef (PLOT_IOFuncTypeDef) (char *);
typedef PLOT_ReturnTypeDef (PLOT_UserOpenFuncTypeDef) (PLOT_HandletypeDef);


typedef struct{
    PLOT_IOFuncTypeDef *writeFunc;
    PLOT_IOFuncTypeDef *readFunc;
    PLOT_WaitTypeDef wait_m;
    PLOT_PositionTypeDef position;
    PLOT_MoveModeTypeDef move_m;
    PLOT_InitTypeDef handle_stat;
    float max_feed;
} _PLOT_StatusTypeDef;




PLOT_HandletypeDef plotterInit(PLOT_IOFuncTypeDef *, PLOT_IOFuncTypeDef *, PLOT_WaitTypeDef, float);
PLOT_ReturnTypeDef plotterOpen(PLOT_HandletypeDef, PLOT_UserOpenFuncTypeDef *);
PLOT_ReturnTypeDef plotterWrite(PLOT_HandletypeDef, PLOT_CommandTypeDef , void *);
PLOT_ReturnTypeDef plotterRead(PLOT_HandletypeDef, char *);

PLOT_ReturnTypeDef _plotterWriteRaw(PLOT_HandletypeDef, char *);

PLOT_ReturnTypeDef plotterSetAbsMode(PLOT_HandletypeDef);
PLOT_ReturnTypeDef plotterSetRelMode(PLOT_HandletypeDef);

PLOT_ReturnTypeDef plotterFastMove(PLOT_HandletypeDef, float, float);
PLOT_ReturnTypeDef plotterLinMove(PLOT_HandletypeDef, float, float, float);
PLOT_ReturnTypeDef plotterRaiseZ(PLOT_HandletypeDef);
PLOT_ReturnTypeDef plotterDropZ(PLOT_HandletypeDef);
PLOT_ReturnTypeDef plotterToggleZ(PLOT_HandletypeDef);

PLOT_ReturnTypeDef plotterUpdatePosition(PLOT_HandletypeDef);


void plotterErrorHandler(PLOT_ErrorTypeDef);

#endif






/**
 * @file state_machine.h
 * @author Daniel Adamkovic (dadamkovic@protonmail.ch)
 * @brief
 * @version 0.1
 * @date 05-02-2023
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __STATE_MACHINE_H__
#define __STATE_MACHINE_H__

#include "stm32f1xx_hal.h"

#ifdef LOG_DEBUG
#include "logger.h"
#endif

#define SM_MAX_TRANS 8
#define SM_MAX_SM 32
#define SM_ERROR_HANDLE -1

///@brief When defined all transitions will be evaluated to check conflicts
#define SM_EVAL_ALL_TRANS

typedef enum
{
  SM_TRUE = 0,
  SM_FALSE = 1
} SM_BoolTypeDef;

typedef enum
{
  SM_OK = 0,
  SM_FAIL = 1
} SM_ReturnTypeDef;

typedef int8_t SM_HandleTypeDef;
typedef SM_BoolTypeDef(SM_TransitionFunc)(void);
typedef SM_ReturnTypeDef(SM_StateFunc)(void);
typedef SM_HandleTypeDef(SM_InitFunc)(void);

typedef enum
{
  SM_NO_FREE_HANDLE_ERR,
  SM_EXCEEDED_MAX_TRANS,
  SM_MULITPLE_TRANSITIONS_ERR
} SM_ErrorTypeDef;

typedef struct
{
  SM_HandleTypeDef id;
  SM_TransitionFunc *(trans_fns)[SM_MAX_TRANS];
  SM_HandleTypeDef trans_sts[SM_MAX_TRANS];
  SM_StateFunc *state_func;
  uint8_t num_trans;
  SM_BoolTypeDef available;
} SM_StatusTypeDef;

void smErrorHandler(SM_ErrorTypeDef error_t);
SM_HandleTypeDef smAddState(SM_StateFunc *state_func);

SM_ReturnTypeDef smAddTransition(SM_HandleTypeDef root_id,
                                 SM_HandleTypeDef dest_id,
                                 SM_TransitionFunc *trans_c);
SM_HandleTypeDef smNextState(SM_HandleTypeDef sm_id);
SM_ReturnTypeDef smRunState(SM_HandleTypeDef sm_id);
SM_HandleTypeDef smInit(SM_InitFunc *init_f);

#endif
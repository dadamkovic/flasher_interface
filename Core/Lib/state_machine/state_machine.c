/**
 * @file state_machine.c
 * @author Daniel Adamkovic (dadamkovic@protonmail.ch)
 * @brief
 * @version 0.1
 * @date 05-02-2023
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "state_machine.h"

static SM_StatusTypeDef _state_machines[SM_MAX_SM];
static SM_HandleTypeDef _smGetFreeHandle();

/**
 * @brief Return free handle of state machine node if available
 *
 * @return SM_HandleTypeDef
 */
static SM_HandleTypeDef _smGetFreeHandle()
{
  for (uint8_t i = 0; i < SM_MAX_SM; i++)
  {
    if (_state_machines[i].available == SM_TRUE)
    {
      _state_machines[i].available = SM_FALSE;
      return i;
    }
  }
  smErrorHandler(SM_NO_FREE_HANDLE_ERR);
  return SM_ERROR_HANDLE;
}

/**
 * @brief Adds a new state into the state machine diagram
 *
 * @param state_func Function to be ran inside the new state node (can be NULL)
 * @return SM_ReturnTypeDef Handle of the newly created node or SM_ERROR_HANDLE
 */
SM_HandleTypeDef smAddState(SM_StateFunc *state_func)
{
  SM_HandleTypeDef sm_id = _smGetFreeHandle();
  if (SM_ERROR_HANDLE == sm_id)
  {
    return SM_FAIL;
  }

  _state_machines[sm_id].state_func = state_func;
  return sm_id;
}


/**
 * @brief Wrapper around user initialization function
 * 
 * @param init_f User initialization function pointer
 * @return SM_HandleTypeDef 
 */
SM_HandleTypeDef smInit(SM_InitFunc *init_f){
  return init_f();
}


/**
 * @brief Adds state transition between the root state and dest state
 *
 * @param root_id Handle of the root state
 * @param dest_id Handle of the destination state
 * @param trans_c Function pointer to eval. transition condition
 * @return SM_ReturnTypeDef SM_FAIL or SM_OK
 */
SM_ReturnTypeDef smAddTransition(SM_HandleTypeDef root_id,
                                 SM_HandleTypeDef dest_id,
                                 SM_TransitionFunc *trans_c)
{

  // first fetch the last transition index
  uint8_t tail_idx = _state_machines[root_id].num_trans;
  if (tail_idx >= SM_MAX_TRANS)
  {
    smErrorHandler(SM_EXCEEDED_MAX_TRANS);
    return SM_FAIL;
  }

  // increase the index of last unocupied transition in the state
  _state_machines[root_id].num_trans++;

  _state_machines[root_id].trans_sts[tail_idx] = dest_id;
  _state_machines[root_id].trans_fns[tail_idx] = trans_c;

  return SM_OK;
}

/**
 * @brief Evaluates the transition conditions and returns handle to next state
 *
 * @param sm_id Current state handle
 * @return SM_HandleTypeDef Next state handle
 */
SM_HandleTypeDef smNextState(SM_HandleTypeDef sm_id)
{
  uint8_t tail_idx = _state_machines[sm_id].num_trans;
  SM_ReturnTypeDef ret = SM_FALSE;
  SM_HandleTypeDef ret_handle = SM_ERROR_HANDLE;

  // if state has no tranistions we stay in it
  if (tail_idx == 0)
  {
    return sm_id;
  }

  // looping over all the transition conditions
  while (--tail_idx)
  {
    ret = _state_machines[sm_id].trans_fns[tail_idx]();

    // check if transition condition has been fulfilled first time
    //if not evaluating all state we immediately return
    if ((ret == SM_TRUE) && (ret_handle == SM_ERROR_HANDLE))
    {
      ret_handle = _state_machines[sm_id].trans_sts[tail_idx];
      #ifndef SM_EVAL_ALL_TRANS
      return ret_handle;
      #endif
    }
    
    // if another transition condition is true we raise error
    #ifdef SM_EVAL_ALL_TRANS
    else if ((ret == SM_TRUE) && (ret_handle != SM_ERROR_HANDLE))
    {
      smErrorHandler(SM_MULITPLE_TRANSITIONS_ERR);
      return SM_ERROR_HANDLE;
    }
    #endif
  }

  //we only found one possible transition
  if(ret_handle != SM_ERROR_HANDLE)
  {
    return ret_handle;
  }

  // no transtion condition fulfilled, so we stay in the same state
  else
  {
    return sm_id;
  }
  
}

/**
 * @brief Runs the state function in the given state
 *
 * @param sm_id State handle
 * @return SM_ReturnTypeDef
 */
SM_ReturnTypeDef smRunState(SM_HandleTypeDef sm_id)
{
  if (_state_machines[sm_id].state_func != NULL)
  {
    return _state_machines[sm_id].state_func();
  }
  return SM_OK;
}

__attribute__((weak)) void smErrorHandler(SM_ErrorTypeDef error_t)
{
  switch (error_t)
  {
  case SM_NO_FREE_HANDLE_ERR:
    #ifdef LOG_DEBUG
      logWrite(__FILE__, "No free state handles!", LOG_MEDIUM_PRIO);
    #endif
    break;
  default:
    break;
  }
}

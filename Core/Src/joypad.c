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
#include <string.h>


static JOY_PositionTypeDef _joyGetThreshold(JOY_HandleTypeDef);
static JOY_HandleTypeDef _joyGetFreeJoypad();

static __JOY_StatusTypeDef _joy_status[JOY_MAX_CONNECTED];



/**
 * @brief Runs user defined init function and returns joypad handle
 * 
 * @param init_f User defined init function
 * @return JOG_HandleTypeDef Handle to the joypad
 */
JOY_HandleTypeDef joyInit(JOY_InitFuncTypeDef *init_f, int16_t *thresholds){
  if(thresholds == NULL){
    joyErrorHandler(JOY_THRESHOLD_NULL_ERR);
    return JOY_HANDLE_ERROR;
  }

  JOY_HandleTypeDef joy_id = _joyGetFreeJoypad();
  if(joy_id == JOY_HANDLE_ERROR){
    joyErrorHandler(JOY_HANDLES_EXHAUSED_ERR);
    return JOY_HANDLE_ERROR;
  }

  if(init_f != NULL){
    init_f();
    _joy_status[joy_id].initFunc = init_f;
  }

  for(uint8_t idx=0; idx<4; idx++){
    _joy_status[joy_id].threshold[idx] = thresholds[idx];
  }
  
  return joy_id;
}


/**
 * @brief Internal function checks if there is some unitialized joypad handle locks it if there is one and provides it
 * 
 * @return JOY_HandleTypeDef Free (and locked) joypad or JOY_HANDLE_ERROR if none is free
 */
static JOY_HandleTypeDef _joyGetFreeJoypad(){
  for(uint8_t id=0; id<JOY_MAX_CONNECTED; id++){
    if(_joy_status[id].joy_stat == JOY_FREE){
      _joy_status[id].joy_stat = JOY_USED;
      _joy_status[id].id = id;
      return id;
    }
  }

  return JOY_HANDLE_ERROR;
}


/**
 * @brief Opens joypad for use with supplied read function
 * 
 * @param joy_id Handle of the joypad in use
 * @param read_f Pointer to function of typedef JOY_ReadFuncTypeDef 
 * @return JOY_HandleTypeDef Handle of the joypad in use or JOY_HANDLE_ERROR
 */
JOY_HandleTypeDef joyOpen(JOY_HandleTypeDef joy_id, JOY_ReadFuncTypeDef *read_f){
  if(read_f == NULL){
    joyErrorHandler(JOY_READ_FUNC_NULL_ERR);
    return JOY_HANDLE_ERROR;
  }

  _joy_status[joy_id].readFunc = read_f;

  return joy_id;
}


/**
 * @brief Reads the joypad values into user supplied buffer or provides data from previous measurement
 * 
 * @param joy_id Joypad handle
 * @param output Buffer of type JOY_OutTypeDef
 * @param out_t Flag of type JOY_FeedbackTypeDef to select between reading fresh and just supplying old data
 * @return JOY_OutTypeDef* Pointer to the buffer the user supplied
 */
JOY_OutTypeDef *joyRead(JOY_HandleTypeDef joy_id, JOY_OutTypeDef *output, JOY_FeedbackTypeDef out_t){
  int16_t data[2];
  
  switch(out_t){
    case JOY_MEASURE:
      _joy_status[joy_id].readFunc(data);
      _joy_status[joy_id].x = data[0];
      _joy_status[joy_id].y = data[1];
      break;

    case JOY_RETRIEVE_OLD:
      data[0] = _joy_status[joy_id].x;
      data[1] = _joy_status[joy_id].y;
      break;

    default:
      break;
  }

  output->x = data[0];
  output->y = data[1];
  output->pos = _joyGetThreshold(joy_id);

  return output;
}


/**
 * @brief To be redefined by the end user if needed
 * @todo Create enum with error flags that will be supplied here
 * 
 */
__attribute__((weak)) void joyErrorHandler(JOY_ErrorTypeDef err_origin){
  
  while(1){

  }
}


/**
 * @brief Internal function that outputs JOY_PositionTypeDef status
 * 
 * @param joy_id Joypad handle
 * @return JOY_PositionTypeDef One letter chanracter depending on where the joypad is pointing
 */
static JOY_PositionTypeDef _joyGetThreshold(JOY_HandleTypeDef joy_id){
  int16_t *thresholds = _joy_status[joy_id].threshold; 
  int16_t x = _joy_status[joy_id].x; 
  int16_t y = _joy_status[joy_id].y;

  if(x > thresholds[0])return JOY_RIGHT;
  else if(y > thresholds[1])return JOY_UP;
  else if(x < thresholds[2])return JOY_LEFT;
  else if(y < thresholds[3])return JOY_DOWN;
  else return JOY_CENTER;
}


/**
 * @brief Function used for modifying the read function of the joypad
 * 
 * @param joy_id Joypad handle
 * @param read_f New function for reading values
 * @return JOY_HandleTypeDef Joypad handle
 * 
 * @todo Consider if more atttributes shouldn't be added here
 */
JOY_HandleTypeDef joyIoctl(JOY_HandleTypeDef joy_id, JOY_ReadFuncTypeDef *read_f){
  _joy_status[joy_id].readFunc = read_f;

  return joy_id;
}


/**
 * @brief Frees the selected joypad handle for future use
 * 
 * @param joy_id Joypad handle to be freed
 * @return JOY_ReturnTypeDef JOY_OK
 */
JOY_ReturnTypeDef joyClose(JOY_HandleTypeDef joy_id){
  _joy_status[joy_id].x = 0;
  _joy_status[joy_id].y = 0;
  _joy_status[joy_id].readFunc = NULL;
  _joy_status[joy_id].initFunc = NULL;
  memset(_joy_status[joy_id].threshold, 0, 4*sizeof(_joy_status[joy_id].threshold[0]));
  _joy_status[joy_id].pos = JOY_CENTER;
  _joy_status[joy_id].joy_stat = JOY_FREE;

  return JOY_OK;
} 



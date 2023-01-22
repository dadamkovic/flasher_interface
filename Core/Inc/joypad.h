#ifndef __joypad_h__
#define __joypad_h__


#include "adc.h"


#define JOY_MAX_CONNECTED 8
#define JOY_HANDLE_ERROR -1


typedef void (JOY_InitFuncTypeDef)(void);

typedef uint8_t JOY_HandleTypeDef;

typedef enum JOY_InitTypeDef{
    JOY_FREE,
    JOY_USED
} JOY_InitTypeDef;

typedef enum JOY_ReturnTypeDef{
    JOY_OK,
    JOY_FAIL,
}JOY_ReturnTypeDef;

typedef JOY_ReturnTypeDef (JOY_ReadFuncTypeDef)(int16_t *);

typedef enum {
    JOY_LEFT = 'L',
    JOY_RIGHT = 'R',
    JOY_UP = 'U',
    JOY_DOWN = 'D',
    JOY_CENTER = 'C'
}JOY_PositionTypeDef;

typedef enum JOY_FeedbackTypeDef{
    JOY_MEASURE,
    JOY_RETRIEVE_OLD
} JOY_FeedbackTypeDef;



typedef struct {
    JOY_ReadFuncTypeDef *readFunc;
    int16_t x, y;
    JOY_PositionTypeDef pos;
    int16_t threshold[4];
    JOY_HandleTypeDef id;
    JOY_InitFuncTypeDef *initFunc;
    JOY_InitTypeDef joy_stat;
}__JOY_StatusTypeDef;

typedef struct{
    int16_t x, y;
    JOY_PositionTypeDef pos;
}JOY_OutTypeDef;

typedef enum{
    JOY_THRESHOLD_NULL_ERR,
    JOY_HANDLES_EXHAUSED_ERR,
    JOY_READ_FUNC_NULL_ERR,
}JOY_ErrorTypeDef;

JOY_HandleTypeDef joyInit(JOY_InitFuncTypeDef *, int16_t *);
JOY_HandleTypeDef joyOpen(JOY_HandleTypeDef , JOY_ReadFuncTypeDef *);
JOY_OutTypeDef *joyRead(JOY_HandleTypeDef , JOY_OutTypeDef *, JOY_FeedbackTypeDef );
JOY_HandleTypeDef joyIoctl(JOY_HandleTypeDef, JOY_ReadFuncTypeDef *);
JOY_ReturnTypeDef joyClose(JOY_HandleTypeDef);


void joyErrorHandler(JOY_ErrorTypeDef);



#endif
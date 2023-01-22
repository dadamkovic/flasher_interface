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

typedef enum JOY_PositionEnum{
    JOY_LEFT = 'L',
    JOY_RIGHT = 'R',
    JOY_UP = 'U',
    JOY_DOWN = 'D',
    JOY_CENTER = 'C'
}JOY_PositionEnum;

typedef enum JOY_FeedbackTypeDef{
    JOY_MEASURE,
    JOY_RETRIEVE_OLD
} JOY_FeedbackTypeDef;



typedef struct __JOY_StatusTypeDef{
    JOY_ReadFuncTypeDef *readFunc;
    int16_t x, y;
    JOY_PositionEnum pos;
    int16_t threshold[4];
    JOY_HandleTypeDef id;
    JOY_InitFuncTypeDef *initFunc;
    JOY_InitTypeDef joy_stat;
}__JOY_StatusTypeDef;

typedef struct{
    int16_t x, y;
    JOY_PositionEnum pos;
}JOY_OutTypeDef;



JOY_HandleTypeDef joyInit(JOY_InitFuncTypeDef *, int16_t *);
JOY_HandleTypeDef joyOpen(JOY_HandleTypeDef , JOY_ReadFuncTypeDef *);
JOY_OutTypeDef *joyRead(JOY_HandleTypeDef , JOY_OutTypeDef *, JOY_FeedbackTypeDef );
JOY_HandleTypeDef joyIoctl(JOY_HandleTypeDef, JOY_ReadFuncTypeDef *);
JOY_ReturnTypeDef joyClose(JOY_HandleTypeDef);


void joyErrorHandler();



#endif
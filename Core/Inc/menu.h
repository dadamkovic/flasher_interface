#ifndef __menu_h__
#define __menu_h__



#include <string.h>
#include <stdint.h>
#include "stm32f1xx_hal.h"

#ifdef LOG_DEBUG
#include "logger.h"
#endif

//comment out if you don't want to display initial screen
#define MENU_HANDLE_ERROR -1
#define MENU_MAX_MENUS 16
#define LOG_SOURCE __FILE__

#define LOG_SRC_MENU "menu.c"

#define MAX_CHAR_CNT 255
#define TRUE (uint8_t) 1
#define FALSE (uint8_t) 0

typedef enum{
    MENU_FREE,
    MENU_USED
} MENU_InitTypeDef;

typedef enum{
    MENU_NOT_PRESSED,
    MENU_PRESSED
}MENU_ButtonTypeDef;

typedef enum{
    MENU_OK,
    MENU_FAIL
} MENU_ReturnTypeDef;

typedef enum{
    MENU_LEFT = 'L',
    MENU_RIGHT = 'R',
    MENU_UP = 'U',
    MENU_DOWN = 'D',
    MENU_CENTER = 'C'
}MENU_PositionTypeDef;

typedef MENU_ReturnTypeDef(MENU_ExecFuncTypeDef)(void *);
typedef MENU_ButtonTypeDef (MENU_ButtonFuncTypeDef)(void);
typedef MENU_PositionTypeDef (MENU_PositionTypeFunc)(void);
typedef uint8_t MENU_HandleTypeDef;
typedef MENU_ReturnTypeDef (MENU_DrawScreenTypeFunc)(MENU_HandleTypeDef);


typedef enum{
    MENU_NO_EXEC,
    MENU_EXEC
}MENU_ExecEnum;

typedef MENU_HandleTypeDef (MENU_InitFuncTypeDef)(void);

typedef struct _MENU_StatusTypeDef{
    char title[MAX_CHAR_CNT];
    char content[MAX_CHAR_CNT];
    MENU_HandleTypeDef left_m;
    MENU_HandleTypeDef right_m;
    MENU_HandleTypeDef exec_m;
    MENU_ExecEnum exec;
    MENU_ExecFuncTypeDef *exec_f;
    MENU_HandleTypeDef id;
    MENU_InitTypeDef menu_stat;
}_MENU_StatusTypeDef;

typedef struct {
    MENU_ButtonFuncTypeDef *getSelect;
    MENU_ButtonFuncTypeDef *getCancel;
    MENU_PositionTypeFunc *getPosition;
    MENU_DrawScreenTypeFunc *drawScreen;
}_MENU_ControlTypeDef;


typedef enum {
    MENU_NULL_INIT_FUNC_ERR,
    MENU_HANDLE_ERR,
    MENU_TEXT_LEN_ERR,
    MENU_NULL_EXEC_FUNC_ERR,

}MENU_ErrorTypeDef;


char *menuReadTitle(MENU_HandleTypeDef);
char *menuReadCont(MENU_HandleTypeDef);

MENU_HandleTypeDef menuInit(MENU_InitFuncTypeDef *, 
                            MENU_ButtonFuncTypeDef *, 
                            MENU_ButtonFuncTypeDef *, 
                            MENU_PositionTypeFunc *, 
                            MENU_DrawScreenTypeFunc *);

MENU_ReturnTypeDef menuLinkExec(MENU_HandleTypeDef, MENU_HandleTypeDef, MENU_ExecFuncTypeDef *);
MENU_ReturnTypeDef menuLink(MENU_HandleTypeDef, MENU_HandleTypeDef);
MENU_ReturnTypeDef menuRunState(MENU_HandleTypeDef );
MENU_HandleTypeDef menuNextState(MENU_HandleTypeDef);

MENU_HandleTypeDef menuCreate(char *, char *);


void menuErrorHandler(MENU_ErrorTypeDef);


#endif
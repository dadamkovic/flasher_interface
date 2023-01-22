#ifndef __menu_h__
#define __menu_h__



#include "st7735.h"
#include <string.h>

//comment out if you don't want to display initial screen
#define MENU_HANDLE_ERROR -1
#define MENU_MAX_MENUS 16


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
}MENU_ButtonEnum;

typedef enum{
    MENU_OK,
    MENU_FAIL
} MENU_ReturnTypeDef;

typedef enum Menu_PositionEnum{
    MENU_LEFT = 'L',
    MENU_RIGHT = 'R',
    MENU_UP = 'U',
    MENU_DOWN = 'D',
    MENU_CENTER = 'C'
}MENU_PositionEnum;

typedef MENU_ReturnTypeDef(MENU_ExecFuncTypeDef)(void *);
typedef MENU_ButtonEnum (MENU_ButtonFuncTypeDef)(void);
typedef MENU_PositionEnum (MENU_PositionTypeFunc)(void);
typedef uint8_t MENU_HandleTypeDef;
typedef MENU_ReturnTypeDef (MENU_DrawScreenTypeFunc)(MENU_HandleTypeDef);


typedef enum MENU_ExecEnum{
    MENU_NO_EXEC,
    MENU_EXEC
}MENU_ExecEnum;

typedef MENU_HandleTypeDef (MENU_InitFuncTypeDef)(void);

typedef struct Menu Menu;

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


__attribute__((weak)) void menuErrorHandler();


#endif
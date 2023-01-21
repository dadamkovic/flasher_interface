#ifndef __menu_h__
#define __menu_h__

#include "main.h"
#include "execution.h"
#include "st7735.h"

//comment out if you don't want to display initial screen
#define SHOW_HOME_SCR

#define MAX_CHAR_CNT 64
#define TRUE (uint8_t) 1
#define FALSE (uint8_t) 0

typedef uint8_t(*t_exec_f)(uint8_t *, SetupData *, CommData *);

typedef struct Menu Menu;

typedef struct Menu{
    char title[MAX_CHAR_CNT];
    char content[MAX_CHAR_CNT];
    Menu *left_m;
    Menu *right_m;
    Menu *exec_m;
    uint8_t exec;
    t_exec_f *exec_f;
    uint8_t id;
}Menu;


//external function declarations
Menu * menuInit();
uint8_t menuResetMenu(Menu **);

uint8_t menuGenerate(Menu *, const char *, const char *);
uint8_t menuLink(Menu *, Menu *);
uint8_t menuDisplayClear();
uint8_t menuDrawScreen(Menu *);
uint8_t menuLinkExec(Menu *, Menu *);

uint8_t menuServeMenu(Menu **, uint8_t *);
uint8_t menuServeFunc(Menu *, uint8_t *, struct SetupData *, struct CommData *);

uint8_t _menuHomeScreen(Menu *);

inline uint8_t menuSetExec(Menu *);
uint8_t menuAttachExecFunc(Menu *, t_exec_f *);


#endif
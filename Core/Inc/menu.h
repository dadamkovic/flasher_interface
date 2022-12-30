#ifndef __menu_h__
#define __menu_h__

#include "main.h"
#include "flash_execution.h"
#include "st7735.h"

#define TITLE_IDX 0
#define CONT_IDX 1


enum screens{
  None,
  Home_screen, 
  Manual_move,
  Zero_pos,
  Grid_setup_auto,
  Grid_setup_manual,
  Grid_setup_zero,
  Grid_setup_move,
  Success_screen,
  Fail_screen,
  Next_batch,
  MENUS_NUM};





struct menu_status{
  enum screens curr_screen;
  uint8_t update_needed;
  uint8_t alarm_count;
};




//external function declarations
uint8_t initMenu(struct menu_status *menu_h);
uint8_t serveMenuScreen(struct menu_status *, uint8_t *);
uint8_t serveMenuFunc(struct menu_status *, uint8_t *, struct setup_data *);


uint8_t drawMenuScreen(enum screens);
uint8_t setMenuTexts(char *, const char *);


extern char menu_titles[MENUS_NUM][100];
extern char menu_cont[MENUS_NUM][100];






#endif
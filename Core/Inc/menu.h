#ifndef __menu_h__
#define __menu_h__

#include "main.h"
#include "execution.h"
#include "st7735.h"


enum screens{
  MENU_START,
  Home_screen, 
  Manual_move,
  Zero_pos,
  Grid_setup_auto,
  Grid_setup_manual,
  USER_MENUS_NUM,
  Grid_setup_x_num,
  Grid_setup_y_num,
  Grid_setup_x_offset,
  Grid_setup_y_offset,
  Grid_setup_zero,
  Grid_setup_move,
  Execute_flashing,
  Execute_manual,
  Success_screen,
  Fail_screen,
  Next_batch,
  TOTAL_MENUS_NUM};






typedef struct menu_status{
  enum screens curr_screen;
  uint8_t update_needed;
  uint8_t alarm_count;
  uint8_t exec_menu;
  uint8_t exit_menu;
}menu_status;




//external function declarations
uint8_t initMenu(struct menu_status *menu_h);
uint8_t serveMenuScreen(struct menu_status *, uint8_t *);
uint8_t serveMenuFunc(struct menu_status *, uint8_t *, struct setup_data *, struct comm_data *);


uint8_t drawMenuScreen(enum screens);
uint8_t setMenuTexts(char *, const char *);







#endif
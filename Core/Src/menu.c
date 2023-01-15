/**
 * @file menu.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "menu.h"

char menu_titles[TOTAL_MENUS_NUM+1][MAX_TEXT_S];
char menu_cont[TOTAL_MENUS_NUM+1][MAX_TEXT_S];
uint8_t menu_transition[TOTAL_MENUS_NUM+1] = {0}; 

uint8_t initMenu(struct menu_status *menu_h){
  ST7735_Init();
  ST7735_FillScreen(ST7735_GRAY);
  menu_h->curr_screen = Home_screen;
  menu_h->update_needed = true;
  menu_h->alarm_count = 0;
  menu_h->exec_menu = false;
  menu_h->exit_menu = false;

  setMenuTexts(menu_titles[Home_screen],"Plotter ON");
  setMenuTexts(menu_titles[Manual_move],"Init. man move");
  setMenuTexts(menu_titles[Zero_pos],"Set Zero Pos.");
  setMenuTexts(menu_titles[Grid_setup_auto],"Auto. Setup");
  setMenuTexts(menu_titles[Grid_setup_manual],"Man. Setup");
  setMenuTexts(menu_titles[Grid_setup_zero],"Set (0,0)-pos");
  setMenuTexts(menu_titles[Grid_setup_move],"Set next-pos");
  setMenuTexts(menu_titles[Success_screen],"SUCCESS!");
  setMenuTexts(menu_titles[Fail_screen],"FAILED!");
  setMenuTexts(menu_titles[Next_batch],"Run Next?");
  setMenuTexts(menu_titles[Execute_manual],"Manual movement");
  setMenuTexts(menu_titles[TOTAL_MENUS_NUM],"NEVER REACHED");

  setMenuTexts(menu_cont[Home_screen],"");
  setMenuTexts(menu_cont[Manual_move],"Manually move plotter using joypad");
  setMenuTexts(menu_cont[Zero_pos],"Set current possition as origin (0,0)");
  setMenuTexts(menu_cont[Grid_setup_auto],"Automatically setup the flash pin positions");
  setMenuTexts(menu_cont[Grid_setup_manual],"Manually setup the flash pin positions");
  setMenuTexts(menu_cont[Grid_setup_zero],"Set start position");
  setMenuTexts(menu_cont[Grid_setup_move],"Set next position");
  setMenuTexts(menu_cont[Success_screen],"");
  setMenuTexts(menu_cont[Fail_screen],"");
  setMenuTexts(menu_cont[Next_batch],"Press SELECT when next batch loaded.\nPress CANCEL to stop flashing.");
  setMenuTexts(menu_cont[Execute_manual],"Move joypad to move the plotter.");

  menu_transition[Manual_move] = Execute_manual;
  menu_transition[Execute_manual] = Execute_manual;



  return RETURN_OK;
}

uint8_t serveMenuScreen(struct menu_status *menu_h, uint8_t *joy_vals){

  //in case the user selected something we don't want to change menus but activate execution of the selection
  if(menu_h->exec_menu){
    menu_h->curr_screen = menu_transition[menu_h->curr_screen];
    menu_h->update_needed = true;
    menu_h->exec_menu = false;
    menu_h->exit_menu = false;
  }
  if(menu_h->exit_menu){
    menu_h->curr_screen = Manual_move;
    menu_h->update_needed = true;
    menu_h->exec_menu = false;
    menu_h->exit_menu = false;
  }

  if(menu_h->curr_screen < USER_MENUS_NUM){
    //joypad to the right so switch to next screen
    if(joy_vals[1] > 220){
      HAL_Delay(500);
      if(menu_h->curr_screen < (USER_MENUS_NUM-1))menu_h->curr_screen = (menu_h->curr_screen + 1);
      else menu_h->curr_screen = Home_screen + 1;
      menu_h->update_needed = true;
    } 
    //joypad to the left so switch to previous screen
    else if(joy_vals[1] < 30){
      HAL_Delay(500);
      if(menu_h->curr_screen > Manual_move)menu_h->curr_screen = (menu_h->curr_screen - 1);
      else menu_h->curr_screen = USER_MENUS_NUM - 1;
      menu_h->update_needed = true;
    } 

    else;
  }

  if(menu_h->update_needed == true){
    drawMenuScreen(menu_h->curr_screen);
    menu_h->update_needed = false;
    if(menu_h->curr_screen == Home_screen){
      menu_h->curr_screen += 1;
      menu_h->update_needed = true; 
    }
    HAL_Delay(500);
  }

  return RETURN_OK;
}

uint8_t serveMenuFunc(menu_status *menu_h, uint8_t *joy_vals, setup_data *setup_data_h, comm_data *comm_handle){
  //no need to handle this menu
  if(menu_h->curr_screen <= USER_MENUS_NUM){
    return RETURN_OK;
  }
  
  //menus that are needed to handle
  switch(menu_h->curr_screen){
    case Execute_manual:
      manualControl(joy_vals, setup_data_h, comm_handle);
      break;

    default:
      break;


  }
  return RETURN_OK;
}



uint8_t drawMenuScreen(enum screens req_screen){
  //grey background
  ST7735_FillScreen(ST7735_GRAY);
  ST7735_WriteJustifyString(10, menu_titles[req_screen], Font_11x18, ST7735_RED,ST7735_GRAY,JUST_CENTER);
  ST7735_WriteJustifyString(50, menu_cont[req_screen], Font_7x10, ST7735_BLUE,ST7735_GRAY,JUST_LEFT);

  return RETURN_OK;
}

//TODO: This could maybe be completely removed it is just a wrapper anyway
uint8_t setMenuTexts(char *dest, const char *cont){
  return copyString(dest, cont);
}






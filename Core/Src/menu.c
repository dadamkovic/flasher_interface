#include "menu.h"

char menu_titles[MENUS_NUM][100];
char menu_cont[MENUS_NUM][100];

uint8_t initMenu(struct menu_status *menu_h){
  ST7735_Init();
  menu_h->curr_screen = Home_screen;
  menu_h->update_needed = true;
  menu_h->alarm_count = 0;

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
  setMenuTexts(menu_titles[MENUS_NUM],"NEVER REACHED");

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

  return RETURN_OK;
}

uint8_t serveMenuScreen(struct menu_status *menu_h, uint8_t *joy_vals){
  if(menu_h->update_needed == true){
    drawMenuScreen(menu_h->curr_screen);
    menu_h->update_needed = false;
  }
  
  //make sure that home screen gets displayed for 1s then change to next screen
  if(menu_h->curr_screen == Home_screen){
    HAL_Delay(1000);
    menu_h->curr_screen = Manual_move;
    menu_h->update_needed = true;
  }

  //joypad to the right so switch to next screen
  else if(joy_vals[1] > 220){
    if(menu_h->curr_screen < MENUS_NUM)menu_h->curr_screen = (menu_h->curr_screen + 1);
    else menu_h->curr_screen = Manual_move;
    menu_h->update_needed = true;
  } 

  //joypad to the left so switch to previous screen
  else if(joy_vals[1] < 30){
    if(menu_h->curr_screen > Manual_move)menu_h->curr_screen = (menu_h->curr_screen - 1);
    else menu_h->curr_screen = Grid_setup_manual;
    menu_h->update_needed = true;
  } 

  else;



  return RETURN_OK;
}

uint8_t serveMenuFunc(struct menu_status *menu_h, uint8_t *joy_vals, struct setup_data *setup_data_h){

  return RETURN_OK;
}

uint8_t drawMenuScreen(enum screens req_screen){
  //grey background
  ST7735_FillScreen(ST7735_GRAY);
  ST7735_WriteJustifyString(10, menu_titles[req_screen], Font_11x18, ST7735_RED,ST7735_GRAY,JUST_CENTER);
  ST7735_WriteJustifyString(50, menu_cont[req_screen], Font_7x10, ST7735_BLUE,ST7735_GRAY,JUST_LEFT);

  return RETURN_OK;
}


uint8_t setMenuTexts(char *dest, const char *cont){
  uint8_t cont_size = strlen(cont)+1;
  strncpy(dest,cont,cont_size);
  return RETURN_OK;
}






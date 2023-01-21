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

/**
 * @brief Initializes the user menus and clears the screen
 * 
 * @return Menu* pointer to startup Menu with allocated memory
 */
Menu *menuInit(){

  static uint8_t init_finished = FALSE;
  static Menu HomeScr, ManualMoveScr, ZeroPosScr, SetupAutoScr, SetupManScr, SuccessScr,
            FailedScr, ExecManScr, RunNextScr, StartScreen;

  if(!init_finished){
    menuDisplayClear();
    
    menuGenerate(&HomeScr,"Plotter ON", "");
    menuGenerate(&ManualMoveScr, "Init. man move", "Manually move plotter using joypad");
    menuGenerate(&ZeroPosScr, "Set Zero Pos.", "Set current possition as origin (0,0)");
    menuGenerate(&SetupAutoScr, "Auto. Setup", "Automatically setup the flash pin positions");
    menuGenerate(&SetupManScr, "Man. Setup", "Manually setup the flash pin positions");
    menuGenerate(&SuccessScr, "SUCCESS!", "");
    menuGenerate(&FailedScr, "FAILED", "");
    menuGenerate(&ExecManScr, "Manual movement", "Move joypad to move the plotter");
    menuGenerate(&RunNextScr, "Run Next?", "Press SELECT when next batch loaded.\nPress CANCEL to stop flashing.");

    //set up links between individual menus
    menuLink(&ManualMoveScr, &ZeroPosScr);
    menuLink(&ZeroPosScr, &SetupAutoScr);
    menuLink(&SetupAutoScr, &SetupManScr);
    menuLink(&SetupManScr, &ManualMoveScr);

    //set up flag to mark menu with execution function
    menuSetExec(&ExecManScr);
    menuLinkExec(&ManualMoveScr, &ExecManScr);
    menuAttachExecFunc(&ExecManScr, manualControl);

    //shows a brief startup screen if enabled
    #ifdef SHOW_HOME_SCR
      _menuHomeScreen(&HomeScr);
      HAL_Delay(1000);
    #endif
  }

  //set to signalize the contents of the menu are initialized
  init_finished = TRUE;

  //can be set to any screen that should be initial
  StartScreen = ManualMoveScr;

  return &StartScreen;
}


uint8_t menuAttachExecFunc(Menu *menu_h, t_exec_f *func){
  if(func == NULL){
    return RETURN_FAIL;
  }

  menu_h->exec_f = func;
  return RETURN_OK;
}


/**
 * @brief 
 * 
 * @param menu_h Handle to the main Menu strct
 * @param exec_menu_h Handle to the execution Menu struct
 * @return uint8_t RETURN_OK on succes, RETURN_FAIL if second menu is not marked as exec
 */
uint8_t menuLinkExec(Menu *menu_h, Menu *exec_menu_h){
  if(exec_menu_h->exec == FALSE){
    return RETURN_FAIL;
  }
  
  //link the exect Menu to the main Menu
  menu_h->exec_m = exec_menu_h;
  return RETURN_OK;
}


/**
 * @brief Wrapper to set the exec flag to true
 * 
 * @param menu_h Menu handle
 * @return uint8_t RETURN_OK
 */
inline uint8_t menuSetExec(Menu *menu_h){
  menu_h->exec = TRUE;

  return RETURN_OK;
}


/**
 * @brief Callback function for startup screen
 * 
 * @param menu_h Menu handle
 * @return uint8_t RETURN_OK
 */
uint8_t _menuHomeScreen(Menu *menu_h){
  menuDrawScreen(menu_h);

  return RETURN_OK;
}


/**
 * @brief Fills the Menu with content
 * 
 * @param ptr Menu handle
 * @param title Title of the menu screen
 * @param cont Contents on the menu screen
 * @return uint8_t RETURN_OK
 */
uint8_t menuGenerate(Menu *ptr, const char *title, const char *cont){
  static uint8_t id = 0;

  strncpy(ptr->title, title, MAX_CHAR_CNT);
  strncpy(ptr->content, cont, MAX_CHAR_CNT);
  ptr->id = id;
  id++;

  return RETURN_OK;
};


/**
 * @brief Links 2 Menus together (used for menu transitions)
 * 
 * @param ptr_l Menu screen on the left
 * @param ptr_r Menu screen on the right
 * @return uint8_t RETURN_OK
 */
uint8_t menuLink(Menu *ptr_l, Menu *ptr_r){
    ptr_l->right_m = ptr_r;
    ptr_r->left_m = ptr_l;
    
    return RETURN_OK;
}



/**
 * @brief Processes the menu shown based on the menu & joypad status 
 * 
 * @param menu_h Menu handle
 * @param joy_vals Array with joypad position vals [0,0] -> [255,255]
 * @return uint8_t RETURN_OK
 */
uint8_t menuServeMenu(Menu **menu_h, uint8_t *joy_vals){

  static uint8_t old_id = -1;
  
  //if we are in exec menu_switches don't matter for selecting menu screens
  if((*menu_h)->exec == FALSE){
    //joypad to the right so switch to next screen
    if(joy_vals[1] > 220){
      HAL_Delay(500);
      *menu_h = (*menu_h)->right_m;
    } 
    //joypad to the left so switch to previous screen
    else if(joy_vals[1] < 30){
      HAL_Delay(500);
      *menu_h = (*menu_h)->left_m;
    } 
    else;
  }

  if(old_id != (*menu_h)->id){
    menuDrawScreen(*menu_h);
    old_id = (*menu_h)->id;
    HAL_Delay(500);
  }

  return RETURN_OK;
}


/**
 * @brief Calls menuInit to retrieve handle of the first menu
 * 
 * @param menu_h Menu handle
 * @return uint8_t RETURN_OK
 */
uint8_t menuResetMenu(Menu **menu_h){
  *menu_h = menuInit();

  return RETURN_OK;
}


/**
 * @brief 
 * 
 * @param menu_h 
 * @param joy_vals 
 * @param setup_h 
 * @param comm_h 
 * @return uint8_t 
 */
uint8_t menuServeFunc(Menu *menu_h, uint8_t *joy_vals, SetupData *setup_h, CommData *comm_h){

  if(menu_h->exec){
    (*menu_h->exec_f)(joy_vals, setup_h, comm_h);
  }
  return RETURN_OK;
}


/**
 * @brief Clears the screen
 * 
 * @return uint8_t RETURN_OK
 */
uint8_t menuDisplayClear(){
  ST7735_Init();
  ST7735_FillScreen(ST7735_GRAY);

  return RETURN_OK;
}


/**
 * @brief Draws the title and contents of the screen
 * 
 * @param menu_h Menu handle
 * @return uint8_t RETURN_OK
 */
uint8_t menuDrawScreen(Menu *menu_h){
  //grey background
  ST7735_FillScreen(ST7735_GRAY);
  ST7735_WriteJustifyString(10, menu_h->title, Font_11x18, ST7735_RED,ST7735_GRAY,JUST_CENTER);
  ST7735_WriteJustifyString(50, menu_h->content, Font_7x10, ST7735_BLUE,ST7735_GRAY,JUST_LEFT);

  return RETURN_OK;
}







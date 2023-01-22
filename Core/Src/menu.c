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

static MENU_HandleTypeDef _menuGetFreeMenu();
static MENU_HandleTypeDef _menuGetHomeMenu(MENU_HandleTypeDef);


static _MENU_StatusTypeDef _menu_status[MENU_MAX_MENUS];
static _MENU_ControlTypeDef _menu_control;

MENU_HandleTypeDef menuInit(MENU_InitFuncTypeDef *func, 
                            MENU_ButtonFuncTypeDef *sel_f, 
                            MENU_ButtonFuncTypeDef *can_f,
                            MENU_PositionTypeFunc *pos_f,
                            MENU_DrawScreenTypeFunc *draw_f){
  
  MENU_HandleTypeDef menu_id = MENU_HANDLE_ERROR;
  if(func != NULL){
    menu_id = func();
  }
  if(sel_f != NULL && can_f != NULL && pos_f != NULL && draw_f != NULL){
    _menu_control.getCancel = can_f;
    _menu_control.getPosition = pos_f;
    _menu_control.getSelect = sel_f;
    _menu_control.drawScreen = draw_f;
    return menu_id;
  }

  menuErrorHandler();
  return MENU_HANDLE_ERROR;
}


MENU_HandleTypeDef menuCreate(char *title, char *cont){
  MENU_HandleTypeDef menu_id = _menuGetFreeMenu();
  if(menu_id == MENU_HANDLE_ERROR){
    menuErrorHandler();
    return MENU_HANDLE_ERROR;
  }

  if((strlen(title) > MAX_CHAR_CNT) || (strlen(cont) > MAX_CHAR_CNT)){
    menuErrorHandler();
    return MENU_HANDLE_ERROR;
  }

  strncpy(_menu_status[menu_id].title, title, MAX_CHAR_CNT);
  strncpy(_menu_status[menu_id].content, cont, MAX_CHAR_CNT);

  return menu_id;
}


static MENU_HandleTypeDef _menuGetFreeMenu(){
    for(uint8_t id=0; id<MENU_MAX_MENUS; id++){
    if(_menu_status[id].menu_stat == MENU_FREE){
      _menu_status[id].menu_stat = MENU_USED;
      _menu_status[id].id = id;
      return id;
    }
  }

  return MENU_HANDLE_ERROR;
}








MENU_ReturnTypeDef menuLinkExec(MENU_HandleTypeDef menu_id, MENU_HandleTypeDef exec_menu_id, MENU_ExecFuncTypeDef *exec_f){
  if(exec_f == NULL){
    menuErrorHandler();
    return MENU_FAIL;
  }

  _menu_status[menu_id].exec_m = exec_menu_id;
  _menu_status[exec_menu_id].exec = MENU_EXEC;
  _menu_status[exec_menu_id].exec_f = exec_f;

  //to the left and right refference we store parent menu so we could return there
  _menu_status[exec_menu_id].left_m = menu_id;
  _menu_status[exec_menu_id].right_m = menu_id;

  return MENU_OK;
}



MENU_ReturnTypeDef menuLink(MENU_HandleTypeDef menu_l_id, MENU_HandleTypeDef menu_r_id){
    _menu_status[menu_l_id].right_m = _menu_status[menu_r_id].id;
    _menu_status[menu_r_id].left_m = _menu_status[menu_l_id].id;
    
    return RETURN_OK;
}




MENU_HandleTypeDef menuNextState(MENU_HandleTypeDef menu_id){

  static MENU_HandleTypeDef old_id = MENU_HANDLE_ERROR;
  MENU_PositionEnum user_com;
  
  MENU_ButtonEnum select_but = _menu_control.getSelect();
  MENU_ButtonEnum cancel_but = _menu_control.getCancel();

  //ignore the case that both buttons are pressed at the same time
  if(select_but == MENU_PRESSED && cancel_but == MENU_PRESSED){
    return menu_id;
  }

  //enter the exec menu
  if(select_but == MENU_PRESSED){
    menu_id = _menu_status[menu_id].exec_m;
  }

  //if we are not in exec menu we move through selections
  if(_menu_status[menu_id].exec == MENU_NO_EXEC){
    user_com = _menu_control.getPosition();
    switch (user_com)
    {
      case MENU_LEFT :
        HAL_Delay(500);
        menu_id = _menu_status[menu_id].left_m;
        break;
      case MENU_RIGHT :
        HAL_Delay(500);
        menu_id = _menu_status[menu_id].right_m;
        break;
      default:
        break;
    }
  }
  //we cam also be in exec menu and pressing cancel we go to home screen
  else if(cancel_but == MENU_PRESSED){
    menu_id = _menuGetHomeMenu(menu_id);
  }

  if(old_id != menu_id){
    _menu_control.drawScreen(menu_id);
    old_id = menu_id;
    HAL_Delay(500);
  }

  return menu_id;
}



static MENU_HandleTypeDef _menuGetHomeMenu(MENU_HandleTypeDef menu_id){
  MENU_HandleTypeDef prev_menu_id = _menu_status[menu_id].right_m;
  if(_menu_status[prev_menu_id].exec == MENU_EXEC){
    return _menuGetHomeMenu(prev_menu_id);
  }
  else{
    return prev_menu_id;
  }
}


char *menuReadTitle(MENU_HandleTypeDef menu_id){
  return _menu_status[menu_id].title;
}

char *menuReadCont(MENU_HandleTypeDef menu_id){
  return _menu_status[menu_id].content;
}


MENU_ReturnTypeDef menuRunState(MENU_HandleTypeDef menu_id){
  
  if(_menu_status[menu_id].exec == MENU_EXEC){
    return (*(_menu_status[menu_id].exec_f))(NULL);
  }
  return MENU_OK;
}



/**
 * @brief To be redefined by the end user if needed
 * @todo Create enum with error flags that will be supplied here
 * 
 */
__attribute__((weak)) void menuErrorHandler(){
  while(1){

  }
}




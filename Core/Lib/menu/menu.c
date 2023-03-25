/**
 * @file menu.c
 * @author Daniel Adamkovic (dadamkovic@protonmail.ch)
 * @brief 
 * @version 0.1
 * @date 22-01-2023
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "menu.h"

static MENU_HandleTypeDef _menuGetFreeMenu();
static MENU_HandleTypeDef _menuGetHomeMenu(MENU_HandleTypeDef);


static _MENU_StatusTypeDef _menu_status[MENU_MAX_MENUS];
static _MENU_ControlTypeDef _menu_control;



/**
 * @brief Initializes the user menu
 * 
 * @param func Pointer to user function where menus and their linkage is defined
 * @param sel_f Pointer to function returning the state of the select button
 * @param can_f Pointer to function returning the state of the cancel button
 * @param pos_f Pointer to function returning the state of the joypad
 * @param draw_f Pointer to function that draws the menu screen
 * @return MENU_HandleTypeDef Menu handle or MENU_WRONG_HANDLE
 */
MENU_HandleTypeDef menuInit(MENU_InitFuncTypeDef *func, 
                            MENU_ButtonFuncTypeDef *sel_f, 
                            MENU_ButtonFuncTypeDef *can_f,
                            MENU_PositionTypeFunc *pos_f,
                            MENU_DrawScreenTypeFunc *draw_f){
  
  MENU_HandleTypeDef menu_id = MENU_WRONG_HANDLE;
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

  menuErrorHandler(MENU_NULL_INIT_FUNC_ERR);
  return MENU_WRONG_HANDLE;
}


/**
 * @brief Creates a new menu and returns its handle
 * 
 * @param title Title of the menu screen
 * @param cont  Description
 * @return MENU_HandleTypeDef MENU_WRONG_HANDLE or menu handle 
 */
MENU_HandleTypeDef menuCreate(char *title, char *cont){
  MENU_HandleTypeDef menu_id = _menuGetFreeMenu();
  if(menu_id == MENU_WRONG_HANDLE){
    menuErrorHandler(MENU_HANDLE_ERR);
    return MENU_WRONG_HANDLE;
  }

  if((strlen(title) > MAX_CHAR_CNT) || (strlen(cont) > MAX_CHAR_CNT)){
    menuErrorHandler(MENU_TEXT_LEN_ERR);
    return MENU_WRONG_HANDLE;
  }

  strncpy(_menu_status[menu_id].title, title, MAX_CHAR_CNT);
  strncpy(_menu_status[menu_id].content, cont, MAX_CHAR_CNT);

  return menu_id;
}


/**
 * @brief Returns handle to free menu or MENU_HANDLE error if all occupied
 * 
 * @return MENU_HandleTypeDef Handle to free menu or MENU_HANDLE error if all occupied
 */
static MENU_HandleTypeDef _menuGetFreeMenu(){
    for(uint8_t id=0; id<MENU_MAX_MENUS; id++){
    if(_menu_status[id].menu_stat == MENU_FREE){
      _menu_status[id].menu_stat = MENU_USED;
      _menu_status[id].id = id;
      return id;
    }
  }

  return MENU_WRONG_HANDLE;
}



/*
      |^^^^^^^^^^^|
      |  exec_m   |
      |___________|
            |
      |^^^^^^^^^^^|      |^^^^^^^^^^^|
------|  menu_l   |------|  menu_r   |--------
      |___________|      |___________|

*/

/**
 * @brief Links exec menu to the specified menu screen
 * 
 * @param menu_id Menu handle to menu where exec menu should be linked
 * @param exec_menu_id Menu handle of the exec menu
 * @param exec_f Function pointer to be executed by the exec menu
 * @return MENU_ReturnTypeDef MENU_OK or MENU_FAIL if NULL exec function pointer
 */
MENU_ReturnTypeDef menuLinkExec(MENU_HandleTypeDef menu_id, MENU_HandleTypeDef exec_menu_id, MENU_ExecFuncTypeDef *exec_f){
  if(exec_f == NULL){
    menuErrorHandler(MENU_NULL_EXEC_FUNC_ERR);
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


/*
      |^^^^^^^^^^^|      |^^^^^^^^^^^|
------|  menu_l   |------|  menu_r   |--------
      |___________|      |___________|
*/

/**
 * @brief Links two menus together
 * 
 * @param menu_l_id Menu on the left side of the menu scren
 * @param menu_r_id Menu on the right side of the menu screen
 * @return MENU_ReturnTypeDef MENU_OK
 */
MENU_ReturnTypeDef menuLink(MENU_HandleTypeDef menu_l_id, MENU_HandleTypeDef menu_r_id){
    _menu_status[menu_l_id].right_m = _menu_status[menu_r_id].id;
    _menu_status[menu_r_id].left_m = _menu_status[menu_l_id].id;
    
    return MENU_OK;
}


/**
 * @brief Evaluates the next state of the menu screen
 * 
 * @param menu_id Current menu handle
 * @return MENU_HandleTypeDef Next menu handle
 */
MENU_HandleTypeDef menuNextState(MENU_HandleTypeDef menu_id){

  static MENU_HandleTypeDef old_id = MENU_WRONG_HANDLE;
  MENU_PositionTypeDef user_com;
  
  MENU_ButtonTypeDef select_but = _menu_control.getSelect();
  MENU_ButtonTypeDef cancel_but = _menu_control.getCancel();

  //ignore the case that both buttons are pressed at the same time
  if(select_but == MENU_PRESSED && cancel_but == MENU_PRESSED){
    return menu_id;
  }

  //enter the exec menu
  //@todo: cahnge the if bellow to something nicer
  if(select_but == MENU_PRESSED && _menu_status[menu_id].exec_m != 0){
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
  //infinite exec menu refferences itself as exec, so it re-enters
  //finite exec menu 
  else{
    menu_id = _menu_status[menu_id].exec_m;
  }

  if(old_id != menu_id){
    _menu_control.drawScreen(menu_id);
    old_id = menu_id;
    HAL_Delay(500);
  }

  return menu_id;
}



/*
      |^^^^^^^^^^^|
   |--|  exec_m   |
   |  |___________|
   |        |
   -->|^^^^^^^^^^^|      |^^^^^^^^^^^|
------|  menu_l   |------|  menu_r   |--------
      |___________|      |___________|

*/
/**
 * @brief Traverses the linkages to the exec menu to find its parent menu
 * 
 * @param menu_id Current menu id
 * @return MENU_HandleTypeDef Menu handle of the first non-exec menu
 */
static MENU_HandleTypeDef _menuGetHomeMenu(MENU_HandleTypeDef menu_id){
  MENU_HandleTypeDef prev_menu_id = _menu_status[menu_id].right_m;
  if(_menu_status[prev_menu_id].exec == MENU_EXEC){
    return _menuGetHomeMenu(prev_menu_id);
  }
  else{
    return prev_menu_id;
  }
}

/**
 * @brief Returns menu titile
 * 
 * @param menu_id Menu handle
 * @return char* Pointer to the menu title
 * @note Possibly rewrite this to not return the real address
 */
char *menuReadTitle(MENU_HandleTypeDef menu_id){
  return _menu_status[menu_id].title;
}


/**
 * @brief Returns menu contents
 * 
 * @param menu_id Menu handle
 * @return char* Pointer to the menu contents
 * @note Possibly rewrite this to not return the real address
 */
char *menuReadCont(MENU_HandleTypeDef menu_id){
  return _menu_status[menu_id].content;
}


/**
 * @brief Either run exec funciton or return if not available
 * 
 * @param menu_id Menu handle
 * @return MENU_ReturnTypeDef MENU_OK 
 */
MENU_ReturnTypeDef menuRunState(MENU_HandleTypeDef menu_id){
  
  if(_menu_status[menu_id].exec == MENU_EXEC){
    return _menu_status[menu_id].exec_f();
  }
  return MENU_OK;
}


/**
 * @brief Default way to handle errors from menu.c
 * 
 */
__attribute__((weak)) void menuErrorHandler(MENU_ErrorTypeDef error){
  switch(error){
    case MENU_NULL_INIT_FUNC_ERR :
      #ifdef LOG_DEBUG
      logWrite(LOG_SOURCE, "NULL function pointer supplied!", LOG_HIGH_PRIO);
      #endif
      break;
    case MENU_HANDLE_ERR :
      #ifdef LOG_DEBUG
      logWrite(LOG_SOURCE, "Wrong handle supplied!", LOG_HIGH_PRIO);
      #endif
      break;
    case MENU_TEXT_LEN_ERR :
      #ifdef LOG_DEBUG
      logWrite(LOG_SOURCE, "Text too long!", LOG_HIGH_PRIO);
      #endif
      break;
    case MENU_NULL_EXEC_FUNC_ERR :
      #ifdef LOG_DEBUG
      logWrite(LOG_SOURCE, "NULL exec function pointer supplied", LOG_HIGH_PRIO);
      #endif
      break;
    default:
      break;
  }
}




/**
 * @file utils.c
 * @author Daniel Adamkovic (dadamkovic@protonmail.ch)
 * @brief 
 * @version 0.1
 * @date 22-01-2023
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "utils.h"





/**
 * @brief Simple wrapper around the strncpy to copy according to the string size
 * 
 * @param dest Pointer to destiantion
 * @param cont Pointer to content
 * @return uint8_t RETURN_OK
 */
uint8_t copyString(char *dest, const char *cont){
    uint8_t cont_size = strlen(cont)+1;
    strncpy(dest,cont,cont_size);
    
    return RETURN_OK;
}


/**
 * @brief Wrapper that checks how much data is in buffer, sends it and clears afterwards
 * 
 * @param uart_h UART_HandleTypeDef handle
 * @param data Pointer to char array with data to send
 * @return uint8_t RETURN_FAIL | RETURN_OK
 */
uint8_t uartSendData(UART_HandleTypeDef *uart_h, char *data){

  uint16_t data_len = strlen(data) + 1; //adding one for \0

  if(HAL_OK != HAL_UART_Transmit(uart_h, data, data_len, 100)){
    return RETURN_FAIL;
  };

  //clear the buffer for the next use
  memset(data, 0, data_len);

  return RETURN_OK;
}


/**
 * @brief Fetches data as long as it keeps comming or runs our of space
 * 
 * @param uart_h UART handle from HAL
 * @param data Pointer to a buffer loaded with data
 * @return uint8_t RETURN_OK || RETURN_FAIL
 */
uint8_t uartGetData(UART_HandleTypeDef *uart_h, char *data){ 
  //clear the buffer before reception
  //memset(data, 0, UTILS_RX_MAX_LEN);

  HAL_UART_Receive(uart_h, data, UTILS_RX_MAX_LEN, 100);
  return RETURN_OK;
}


/**
 * @brief Runs ADC connected to joypad lines
 * 
 * @param joy_vals Array of 2 uint8_t values [x,y]
 * @return JOY_ReturnTypeDef JOY_OK
 */
JOY_ReturnTypeDef userJoyGetVals(int16_t *output){

  HAL_ADC_Start(&hadc1);
  HAL_ADC_PollForConversion(&hadc1,1);
  output[1] = (int16_t) (HAL_ADC_GetValue(&hadc1)>>4);

  HAL_ADC_Start(&hadc1);
  HAL_ADC_PollForConversion(&hadc1,1);
  output[0] = (int16_t) (HAL_ADC_GetValue(&hadc1)>>4);

  output[0] -= 128;  
  output[1] -= 128;

  return JOY_OK;
}

MENU_ReturnTypeDef utilsDisplayClear(){
  ST7735_Init();
  ST7735_FillScreen(ST7735_GRAY);

  return MENU_OK;
}


/**
 * @brief Initializes the user menus and clears the screen
 * 
 * @return Menu* pointer to startup Menu with allocated memory
 */
MENU_HandleTypeDef utilsMenuInit(){

  MENU_HandleTypeDef home_scr_h, manual_move_scr_h, zero_pos_scr_h, setup_auto_scr_h, setup_man_scr_h, success_scr_h,
            dailed_scr_h, exec_man_scr_h, run_next_scr_h, menu_start_screen_h;


  utilsDisplayClear();
  
  home_scr_h = menuCreate("Plotter ON", "");
  manual_move_scr_h = menuCreate("Init. man move", "Manually move plotter using joypad");
  zero_pos_scr_h = menuCreate("Set Zero Pos.", "Set current possition as origin (0,0)");
  setup_auto_scr_h = menuCreate("Auto. Setup", "Automatically setup the flash pin positions");
  setup_man_scr_h = menuCreate("Man. Setup", "Manually setup the flash pin positions");
  success_scr_h = menuCreate("SUCCESS!", "");
  dailed_scr_h = menuCreate("FAILED", "");
  exec_man_scr_h = menuCreate("Manual movement", "Move joypad to move the plotter");
  run_next_scr_h = menuCreate("Run Next?", "Press SELECT when next batch loaded.\nPress CANCEL to stop flashing.");

  //set up links between individual menus
  menuLink(manual_move_scr_h, zero_pos_scr_h);
  menuLink(zero_pos_scr_h, setup_auto_scr_h);
  menuLink(setup_auto_scr_h, setup_man_scr_h);
  menuLink(setup_man_scr_h, manual_move_scr_h);

  //set up flag to mark menu with execution function
  //TODO
  menuLinkExec(manual_move_scr_h, exec_man_scr_h, utilsManMove);


  utilsDrawScreen(home_scr_h);
  HAL_Delay(1000);

  //can be set to any screen that should be initial
  menu_start_screen_h = manual_move_scr_h;

  return menu_start_screen_h;
};
  

MENU_ReturnTypeDef utilsDrawScreen(MENU_HandleTypeDef menu_h){
  //grey background
  ST7735_FillScreen(ST7735_GRAY);
  ST7735_WriteJustifyString(10, menuReadTitle(menu_h), Font_11x18, ST7735_RED,ST7735_GRAY,JUST_CENTER);
  ST7735_WriteJustifyString(50, menuReadCont(menu_h), Font_7x10, ST7735_BLUE,ST7735_GRAY,JUST_LEFT);

  return MENU_OK;
}

PLOT_ReturnTypeDef utilsPlotWrite(char *data){
  if(HAL_OK != HAL_UART_Transmit(&huart1, data, strlen(data), UTILS_COMM_TIMEOUT)){
    return PLOT_FAIL;
  }

  return PLOT_OK;
};

PLOT_ReturnTypeDef utilsPlotRead(char *data){
if(RETURN_OK != uartGetData(&huart1, data)){
  return PLOT_FAIL;
}

  return PLOT_OK;
}

/**
 * @brief 
 * 
 * @param plot_h 
 * @return PLOT_ReturnTypeDef 
 * 
 * @note This needs somehow handle the errors
 */
PLOT_ReturnTypeDef utilsPlotOpen(PLOT_HandletypeDef plot_h){
  plotterSetRelMode(plot_h);
  plotterRaiseZ(plot_h);
  return PLOT_OK;
}


#ifdef LOG_DEBUG
LOG_StatusTypeDef utilsWriteLog(char *data){
  if(HAL_OK != HAL_UART_Transmit(&huart2, data, strlen(data), UTILS_COMM_TIMEOUT)){
    return LOG_FAIL;
  }

  return LOG_OK;
};
#endif



MENU_ReturnTypeDef utilsManMove(){
  JOY_OutTypeDef joy_dat;
  joyRead(g_joypad_h, &joy_dat, JOY_MEASURE);
  
  plotterRaiseZ(g_plotter_h);

  switch (joy_dat.pos)
  {
  case 'U':
    plotterFastMove(g_plotter_h, 0.0, (float)joy_dat.y/100.0);
    break;
  case 'D':
    plotterFastMove(g_plotter_h, 0.0, (float)joy_dat.y/100.0);
    break;
  case 'L':
    plotterFastMove(g_plotter_h, (float)joy_dat.y/100.0, 0.0);
    break;
  case 'R':
    plotterFastMove(g_plotter_h, (float)joy_dat.y/100.0, 0.0);
    break;
  default:
    break;
  }

  return MENU_OK;
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   


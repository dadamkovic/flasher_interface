/**
 * @file execution.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "execution.h"



uint8_t initSetupData(struct setup_data *setup_data_h){
 
  setup_data_h->exec_status = Idle;
  setup_data_h->flash_matrix_dims[0] = 0;
  setup_data_h->flash_matrix_dims[1] = 0;
  setup_data_h->flash_matrix_pos[0] = 0;
  setup_data_h->flash_matrix_pos[1] = 0;
  setup_data_h->offset_x = 0;
  setup_data_h->offset_y = 0;
  strncpy(setup_data_h->fail_msg,"No Fail",ERR_ARRAY_SIZE);
  return RETURN_OK;
}




/**
 * @brief Zeroes the plotter internal coordinates to current position
 * 
 * @param comm_data_h handle containing the uart communication handle and data buffers
 * @return uint8_t RETURN_OK
 */
static uint8_t setZero(struct comm_data *comm_data_h){
  //char command[TX_BUFF_SIZE];

  return RETURN_OK;
}



uint8_t manualControl(uint8_t *joy_vals, setup_data *setup_data_h, comm_data *comm_h){
  float move_dist[2] = {0.0};

  if(joy_vals[0] > 230)move_dist[0] = 10.0;
  else if(joy_vals[0] > 160) move_dist[0] = 1.0;
  else if(joy_vals[0] < 26)move_dist[0] = -10.0;
  else if(joy_vals[0] < 96) move_dist[0] = -1.0;
  else move_dist[0] = 0.0;

  if(joy_vals[1] > 230)move_dist[1] = 10.0;
  else if(joy_vals[1] > 160) move_dist[1] = 1.0;
  else if(joy_vals[1] < 26)move_dist[1] = -10.0;
  else if(joy_vals[1] < 96) move_dist[1] = -1.0;
  else move_dist[1] = 0.0;

  //creates the command in the communication tx buffer
  quickMoveCommand(move_dist, comm_h);

  //semds the commmand stored in the plotter tx buffer
  plotterSendCommand(comm_h);

  //wait until the move is completed
  HAL_Delay(_moveDuration(move_dist));


  //so far only placeholder that doesn't do anything
  while(plotterMoving(comm_h)){
    HAL_Delay(5);
  };
  

  return RETURN_OK;
}

/**
 * @brief Returns 0 if plotter is not moving, 1 otherwise
 * @todo Implement the check of plotter state
 * 
 * @param comm_h handle containing the uart communication handle and data buffers
 * @return uint8_t 
 */
uint8_t plotterMoving(comm_data *comm_h){

  return 0;
}

/**
 * @brief Initiates plottter relative move based on the recieved distances
 * 
 * @param move_dist Vector of [x,y] float distance for the plotter to move by
 * @param comm_h handle containing the uart communication handle and data buffers 
 * @return uint8_t RETURN_OK
 */
uint8_t quickMoveCommand(float *move_dist, comm_data *comm_h){
  char x_dist[8], y_dist[8], feed[8] = {0};

  floatToChar(move_dist[0],x_dist);
  floatToChar(move_dist[1],y_dist);
  floatToChar(FEEDRATE, feed);

  memset(comm_h->plotter_tx,0,TX_BUFF_SIZE);

  strcat(comm_h->plotter_tx,"G01 F");
  strcat(comm_h->plotter_tx, feed);
  strcat(comm_h->plotter_tx, " X");
  strcat(comm_h->plotter_tx, x_dist);
  strcat(comm_h->plotter_tx," Y");
  strcat(comm_h->plotter_tx, y_dist);
  strcat(comm_h->plotter_tx,"\n");

  return RETURN_OK;
}


uint8_t plotterSendCommand(comm_data *comm_h){
  return uartSendData(comm_h->plotter_handle, comm_h->plotter_tx);
};

uint8_t plotterGetCommand(comm_data *comm_h){
  return uartGetData(comm_h->plotter_handle, comm_h->plotter_rx);
}


uint32_t _moveDuration(float *move_dist){
  if(move_dist[0] == 0.0 && move_dist[1] == 0.0)return 0;
  else if(move_dist[0] == 0.0 && move_dist[1] == 10.0 || 
          move_dist[1] == 0.0 && move_dist[0] == 10.0 ){
    return (uint32_t) 10.0/(FEEDRATE_MS);
  }
  else if(move_dist[0] == 0.0 && move_dist[1] == 1.0 || 
          move_dist[1] == 0.0 && move_dist[0] == 1.0 ){
    return (uint32_t) 1.0/(FEEDRATE_MS);
  }
  else if(move_dist[0] == 1.0 && move_dist[1] == 10.0 || 
          move_dist[1] == 1.0 && move_dist[0] == 10.0 ){
    return (uint32_t) 11.0/(FEEDRATE_MS);
  }
  else{
    return (uint32_t) 15.0/(FEEDRATE_MS);
  }
}
#include "flash_execution.h"



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


uint8_t parseInstruction(struct setup_data *setup_data_h, struct comm_data *comm_data_h){
  
  return RETURN_OK;
}

static uint8_t setZero(struct comm_data *comm_data_h){
  char command[TX_BUFF_SIZE];

  return RETURN_OK;
}
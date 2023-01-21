#include "comm_interface.h"


uint8_t initComms(struct CommData *CommData_h){
  CommData_h->flasher_handle = &huart1;
  CommData_h->plotter_handle = &huart3;
  memset(&(CommData_h->flasher_rx),'\0',RX_BUFF_SIZE);
  memset(&(CommData_h->plotter_rx),'\0',RX_BUFF_SIZE);
  memset(&(CommData_h->flasher_tx),'\0',TX_BUFF_SIZE);
  memset(&(CommData_h->plotter_tx),'\0',TX_BUFF_SIZE);

  return RETURN_OK;
}

uint8_t serveCommunication(struct CommData *CommData_h){

  return RETURN_OK;
}

uint8_t recieveFeedback(struct SetupData *setup_h, struct CommData *CommData_h){
  
  return RETURN_OK;
}

uint8_t reportFail(struct SetupData *setup_h, struct CommData *CommData_h){
  
  return RETURN_OK;
}


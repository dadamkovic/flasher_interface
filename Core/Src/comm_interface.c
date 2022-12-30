#include "comm_interface.h"


uint8_t initComms(struct comm_data *comm_data_h){
  comm_data_h->flasher_handle = &huart1;
  comm_data_h->plotter_handle = &huart3;
  memset(&(comm_data_h->flasher_rx),'\0',RX_BUFF_SIZE);
  memset(&(comm_data_h->plotter_rx),'\0',RX_BUFF_SIZE);
  memset(&(comm_data_h->flasher_tx),'\0',TX_BUFF_SIZE);
  memset(&(comm_data_h->plotter_tx),'\0',TX_BUFF_SIZE);

  return RETURN_OK;
}

uint8_t serveCommunication(struct comm_data *comm_data_h){

  return RETURN_OK;
}

uint8_t recieveFeedback(struct setup_data *setup_data_h, struct comm_data *comm_data_h){
  
  return RETURN_OK;
}

uint8_t reportFail(struct setup_data *setup_data_h, struct comm_data *comm_data_h){
  
  return RETURN_OK;
}


#ifndef comm_interface_h
#define comm_interface_h

#include "main.h"
#include "pin_mapping.h"


uint8_t initComms(struct comm_data *);
uint8_t serveCommunication(struct comm_data *);
uint8_t recieveFeedback(struct setup_data *, struct comm_data *);
uint8_t reportFail(struct setup_data *, struct comm_data *);

#endif
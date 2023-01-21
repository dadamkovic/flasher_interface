#ifndef comm_interface_h
#define comm_interface_h

#include "main.h"
#include "pin_mapping.h"


uint8_t initComms(struct CommData *);
uint8_t serveCommunication(struct CommData *);
uint8_t recieveFeedback(struct SetupData *, struct CommData *);
uint8_t reportFail(struct SetupData *, struct CommData *);

#endif
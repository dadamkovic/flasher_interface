#ifndef flash_execution_h
#define flash_execution_h

#include "main.h"




uint8_t initSetupData(struct setup_data *);
uint8_t parseInstruction(struct setup_data *, struct comm_data *);

#endif
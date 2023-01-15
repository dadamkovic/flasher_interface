/**
 * @file execution.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-15
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef __EXECUTION_H__
#define __EXECUTION_H__

#include "main.h"
#include "utils.h"

//max 99999.9
#define FEEDRATE 1000.0
#define FEEDRATE_MS FEEDRATE/60/1000


uint8_t initSetupData(setup_data *);
uint8_t parseInstruction(setup_data *, comm_data *);
static uint8_t _setZero(comm_data *);
uint8_t manualControl(uint8_t *, setup_data *, comm_data *);
uint8_t quickMoveCommand(float *, comm_data *);

uint8_t plotterSendCommand(comm_data *);
uint8_t plotterGetCommand(comm_data *);
uint8_t plotterMoving(comm_data *);

uint32_t _moveDuration(float *);

#endif
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


uint8_t initSetupData(SetupData *);
uint8_t parseInstruction(SetupData *, CommData *);
static uint8_t _setZero(CommData *);
uint8_t manualControl(uint8_t *, SetupData *, CommData *);
uint8_t quickMoveCommand(float *, CommData *);

uint8_t plotterSendCommand(CommData *);
uint8_t plotterGetCommand(CommData *);
uint8_t plotterMoving(CommData *);

uint32_t _moveDuration(float *);

#endif
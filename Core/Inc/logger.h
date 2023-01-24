/**
 * @file logger.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-01-22
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <string.h>
#include <stdint.h>

#define LOG_MAX_LEN 255
#define LOG_MAX_SOURCES 16
#define LOG_PAD_LEN 4
#define LOG_END_LEN 2
#define LOG_SOURCE __FILE__

typedef enum {
    LOG_LOW_PRIO,
    LOG_MEDIUM_PRIO,
    LOG_HIGH_PRIO
}LOG_PriorityTypeDef;

typedef enum{
    LOG_ENABLED,
    LOG_DISABLED
}LOG_EnableTypeDef;

typedef enum{
    LOG_OK,
    LOG_FAIL
}LOG_StatusTypeDef;

typedef enum{
    LOG_PRIO_ERRO,
    LOG_INPUT_SIZE_ERR
}LOG_ErrorTypeDef;

typedef LOG_StatusTypeDef (LOG_WriteFunc)(char *);

typedef struct{
    LOG_PriorityTypeDef log_min_prio;
    LOG_EnableTypeDef log_allowed;
    LOG_WriteFunc *logWrite;
}_LOG_ConfigTypeDef;



LOG_StatusTypeDef logInit(LOG_PriorityTypeDef, LOG_EnableTypeDef, LOG_WriteFunc *);
void logErrorHandler(LOG_ErrorTypeDef);
LOG_StatusTypeDef logClose();
LOG_StatusTypeDef logWrite(char *, char *, LOG_PriorityTypeDef);
LOG_StatusTypeDef logOpen();

#endif
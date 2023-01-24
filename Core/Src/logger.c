/**
 * @file logger.c
 * @author Daniel Adamkovic (dadamkovic@protonmail.ch)
 * @brief 
 * @version 0.1
 * @date 22-01-2023
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "logger.h"


static _LOG_ConfigTypeDef _log_config;
static LOG_StatusTypeDef _logCheckPrio(LOG_PriorityTypeDef);
LOG_StatusTypeDef logInit(LOG_PriorityTypeDef log_min_prio, 
                          LOG_EnableTypeDef log_en, 
                          LOG_WriteFunc *log_write_f)
{
    if(_logCheckPrio(log_min_prio) != LOG_OK){
        logErrorHandler(LOG_PRIO_ERRO);
        return LOG_FAIL;
    }

    _log_config.log_allowed = log_en;
    _log_config.log_min_prio = log_min_prio;
    _log_config.logWrite = log_write_f;
    
    return LOG_OK;
}

LOG_StatusTypeDef logOpen(){
    if(_log_config.log_allowed == LOG_ENABLED){
        return _log_config.logWrite("Logger is ready!\n");
    }

    return LOG_OK;
}

LOG_StatusTypeDef logWrite(char *source, char *cont, LOG_PriorityTypeDef log_prio){

    if(_logCheckPrio(log_prio) != LOG_OK){
        logErrorHandler(LOG_PRIO_ERRO);
        return LOG_FAIL;
    }

    if((log_prio < _log_config.log_min_prio) || (_log_config.log_allowed != LOG_ENABLED)){
        return LOG_OK;
    }

    uint16_t source_len = strlen(source)+1;
    uint16_t cont_len = strlen(cont)+1;
    if(( source_len + cont_len + LOG_PAD_LEN) > LOG_MAX_LEN){
        logErrorHandler(LOG_INPUT_SIZE_ERR);
        return LOG_FAIL;
    }

    char log_text[LOG_MAX_LEN] = {'\0'};
    strncat(log_text, source, source_len);
    strncat(log_text, " : ", LOG_PAD_LEN);
    strncat(log_text, cont, cont_len);
    strncat(log_text, "\n", LOG_END_LEN);
    
    return _log_config.logWrite(log_text);
}


static LOG_StatusTypeDef _logCheckPrio(LOG_PriorityTypeDef log_prio){
    if(log_prio < LOG_LOW_PRIO || log_prio > LOG_HIGH_PRIO){
        return LOG_FAIL;
    }
    return LOG_OK;
}


LOG_StatusTypeDef logClose(){
    _log_config.log_allowed = LOG_DISABLED;

    return _log_config.logWrite("Loggging Terminated!\n");
}

void logErrorHandler(LOG_ErrorTypeDef log_error){
    while(1){

    }

}
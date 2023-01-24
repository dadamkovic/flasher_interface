/**
 * @file plotter.c
 * @author Daniel Adamkovic (dadamkovic@protonmail.ch)
 * @brief 
 * @version 0.1
 * @date 24-01-2023
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#include "plotter.h"

static PLOT_HandletypeDef _plotGetFreeHandle();
static uint32_t _plotGetDelay(PLOT_HandletypeDef, float, float, float);

static _PLOT_StatusTypeDef _plot_status[PLOT_MAX_PLOTTERS];


#define _PLOT_SIZE_CHCK(x,y) (sizeof(x) == y*sizeof(float))


/**
 * @brief 
 * 
 * @param w_func 
 * @param r_func 
 * @param wait_m 
 * @return PLOT_HandletypeDef 
 */
PLOT_HandletypeDef plotterInit(PLOT_IOFuncTypeDef *w_func, 
                               PLOT_IOFuncTypeDef *r_func, 
                               PLOT_WaitTypeDef wait_m,
                               float max_f){

    if(w_func == NULL || r_func || NULL){
        plotterErrorHandler(PLOT_NULL_FUNC_ERR);\
        return PLOT_ERROR_HANDLE;
    }

    PLOT_HandletypeDef plot_id = _plotGetFreeHandle();

    _plot_status[plot_id].wait_m = wait_m;
    _plot_status[plot_id].writeFunc = w_func;
    _plot_status[plot_id].readFunc = r_func;
    _plot_status[plot_id].move_m = PLOT_ABS_MODE;
    _plot_status[plot_id].position.x = 0.0;
    _plot_status[plot_id].position.y = 0.0;
    _plot_status[plot_id].position.z = PLOT_Z_UNDEF;
    _plot_status[plot_id].max_feed = max_f;

    return plot_id;
}


/**
 * @brief 
 * 
 * @param plot_id 
 * @param func 
 * @return PLOT_ReturnTypeDef 
 */
PLOT_ReturnTypeDef plotterOpen(PLOT_HandletypeDef plot_id,
                               PLOT_UserOpenFuncTypeDef *func){

    return func(plot_id);
}

/**
 * @brief 
 * 
 * @param plot_id 
 * @param cmnd 
 * @param data 
 * @return PLOT_ReturnTypeDef 
 * 
 * @note Maybe a candidate for static func?
 */
PLOT_ReturnTypeDef plotterWrite(PLOT_HandletypeDef plot_id,
                                PLOT_CommandTypeDef cmnd, 
                                void *inp){

    float x,y,feed;

    switch(cmnd){
        case PLOT_FASTMOVE_COM :
            if(_PLOT_SIZE_CHCK(inp,2) && inp != NULL){
                x = ((float *)inp)[0];
                y = ((float *)inp)[1];
                return plotterFastMove(plot_id, x, y);
            }
            else{
                plotterErrorHandler(PLOT_DATA_SIZE_ERR);
                return PLOT_FAIL;
            }
            break;
        case PLOT_LIN_MOVE_COM :
            if(_PLOT_SIZE_CHCK(inp,3) && inp != NULL){
                x = ((float *)inp)[0];
                y = ((float *)inp)[1];
                feed = ((float *)inp)[2];
                return plotterLinMove(plot_id, x, y, feed);
            }
            else{
                plotterErrorHandler(PLOT_DATA_SIZE_ERR);
                return PLOT_FAIL;
            }
            break;
        case PLOT_TOGGLE_Z :
            plotterToggleZ(plot_id);
            break;
        case PLOT_RAISE_Z :
            plotterRaiseZ(plot_id);
            break;
        case PLOT_DROP_Z :
            plotterDropZ(plot_id);
            break;
        default :
            break;
    }
return PLOT_OK;
}



PLOT_ReturnTypeDef plotterSetAbsMode(PLOT_HandletypeDef plot_id){
    return _plot_status[plot_id].writeFunc("G90\r");
}


PLOT_ReturnTypeDef plotterSetRelMode(PLOT_HandletypeDef plot_id){
    return _plot_status[plot_id].writeFunc("G91\r");
}


PLOT_ReturnTypeDef plotterLinMove(PLOT_HandletypeDef plot_id, float x, float y, float feed){
    char data[255];
    PLOT_ReturnTypeDef ret;

    sprintf(data, "G01 F%.2f X%.2f Y%.2f\r", feed, x, y);

    ret = _plotterWriteRaw(plot_id, data);

    if(_plot_status[plot_id].wait_m == PLOT_WAIT_FIN){
        HAL_Delay(_plotGetDelay(plot_id, x, y, feed)); 
        return ret;
    }
    return ret;
}



PLOT_ReturnTypeDef plotterFastMove(PLOT_HandletypeDef plot_id, float x, float y){
    return plotterLinMove(plot_id, x, y, _plot_status[plot_id].max_feed);
}


__attribute__((weak)) PLOT_ReturnTypeDef plotterRaiseZ(PLOT_HandletypeDef plot_id){
    return _plotterWriteRaw(plot_id, "S03 90\r");
}


__attribute__((weak)) PLOT_ReturnTypeDef plotterDropZ(PLOT_HandletypeDef plot_id){
    return _plotterWriteRaw(plot_id, "S03 00\r");
}

PLOT_ReturnTypeDef plotterToggleZ(PLOT_HandletypeDef plot_id){
    switch(_plot_status[plot_id].position.z){
        case PLOT_Z_UNDEF :
        case PLOT_Z_DOWN:
            return plotterRaiseZ(plot_id);
            break;
        case PLOT_Z_UP :
            return plotterDropZ(plot_id);
        default :
            break;
    }

    //never reached
    return PLOT_FAIL;
}


/**
 * @brief 
 * 
 * @param plot_id 
 * @param x 
 * @param y 
 * @param feed 
 * @return uint32_t 
 * 
 * @note Assumption is being made that we don't move diagonally which is not true
 */
static uint32_t _plotGetDelay(PLOT_HandletypeDef plot_id, float x, float y, float feed){
    float real_feed = feed == 0.0 ? _plot_status[plot_id].max_feed : feed;
    
    if(_plot_status[plot_id].move_m == PLOT_REL_MODE){
        x = x > 0.0 ? x : -x;
        y = y > 0.0 ? y : -y;
        return (uint32_t)(x+y)/(real_feed/60000.0);
    }
    
    float x_dist = _plot_status[plot_id].position.x - x;
    float y_dist = _plot_status[plot_id].position.y - y;
    x_dist = x_dist > 0.0 ? x_dist : (-x_dist);
    y_dist = y_dist > 0.0 ? y_dist : (-y_dist);
    return (uint32_t)(x_dist+y_dist)/(real_feed/60000.0);
}


/**
 * @brief 
 * 
 * @param plot_id 
 * @param data 
 * @return PLOT_ReturnTypeDef 
 */
PLOT_ReturnTypeDef _plotterWriteRaw(PLOT_HandletypeDef plot_id, char *data){
    return _plot_status[plot_id].writeFunc(data);
}


/**
 * @brief 
 * 
 * @return PLOT_HandletypeDef 
 */
static PLOT_HandletypeDef _plotGetFreeHandle(){
    for(int8_t ix=0; ix < PLOT_MAX_PLOTTERS; ix++){
        if(_plot_status[ix].handle_stat == PLOT_H_FREE){
            _plot_status[ix].handle_stat = PLOT_H_USED;
            return ix;
        }
    }

    plotterErrorHandler(PLOT_NO_FREE_HANDLE_ERR);
    return PLOT_ERROR_HANDLE;
}


/**
 * @brief 
 * 
 */
__attribute__((weak))void plotterErrorHandler(PLOT_ErrorTypeDef error_t){
    switch(error_t){
        case PLOT_NO_FREE_HANDLE_ERR :
            #ifdef LOG_DEBUG
            logWrite(LOG_SOURCE, "No free handles left!", LOG_HIGH_PRIO);
            #endif
            break;
        case PLOT_NULL_FUNC_ERR :
            #ifdef LOG_DEBUG
            logWrite(LOG_SOURCE, "NULL function pointer supplied!", LOG_HIGH_PRIO);
            #endif
            break;
        case PLOT_DATA_SIZE_ERR :
            #ifdef LOG_DEBUG
            logWrite(LOG_SOURCE, "Not enough data supplied!", LOG_HIGH_PRIO);
            #endif
            break;
        default :
            break;
    }
}

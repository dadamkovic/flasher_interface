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
static PLOT_ReturnTypeDef _plotterReadReply(PLOT_HandletypeDef plot_id);
static PLOT_ReturnTypeDef _plotterWriteRawRepeat(PLOT_HandletypeDef plot_id, char *data, uint8_t reps);
static char *_plotFloat2str(float num);
static void _strchcat(char *data, char ap);
static char _helper(int32_t num, uint8_t idx, char *data);


static _PLOT_StatusTypeDef _plot_status[PLOT_MAX_PLOTTERS];

#define _PLOT_SIZE_CHCK(x,y) (sizeof(x) == y*sizeof(float))


/**
 * @brief 
 * 
 * @param w_func Pointer to function used for write access to plotter 
 * @param r_func Pointer to function used for read access to plotter 
 * @param wait_m Waiting mode - either waits for move to finish or yields immediately
 * @param max_f Maximum feedrate allowed for the plotter
 * @return PLOT_HandletypeDef Plotter handle
 */
PLOT_HandletypeDef plotterInit(PLOT_IOFuncTypeDef *w_func, 
                               PLOT_IOFuncTypeDef *r_func, 
                               PLOT_WaitTypeDef wait_m,
                               float max_f){

    if((*w_func == NULL) || (*r_func == NULL)){
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
 * @brief Runs the initialization function of the plotter
 * 
 * @param plot_id Plotter handle
 * @param func Function to be run
 * @return PLOT_ReturnTypeDef 
 */
PLOT_ReturnTypeDef plotterOpen(PLOT_HandletypeDef plot_id,
                               PLOT_UserOpenFuncTypeDef *func){

    return func(plot_id);
}

/**
 * @brief A function for common interface to the plotter commands
 * 
 * @param plot_id Plotter handle
 * @param cmnd Command (enum) to be executed
 * @param inp Pointer to float array or NULL - depending on command 
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


/**
 * @brief Sets the plotter in absolute movement coordinate system
 * 
 * @param plot_id Plotter handle
 * @return PLOT_ReturnTypeDef 
 */
PLOT_ReturnTypeDef plotterSetAbsMode(PLOT_HandletypeDef plot_id){
    return _plot_status[plot_id].writeFunc("G90\r");
}


PLOT_ReturnTypeDef plotterSetRelMode(PLOT_HandletypeDef plot_id){
    return _plot_status[plot_id].writeFunc("G91\r");
}


/**
 * @brief Moves the plotter with the specified feed rate
 * 
 * @param plot_id Plotter handle
 * @param x Absolute or relative move dist - x coordinate
 * @param y Absolute or relative move dist - y coordinate
 * @param feed Feedrate for the move 
 * @return PLOT_ReturnTypeDef 
 */
PLOT_ReturnTypeDef plotterLinMove(PLOT_HandletypeDef plot_id, float x, float y, float feed){
    char data[PLOT_TX_MAX_LEN] = {0};
    PLOT_ReturnTypeDef ret;

    //sprintf(data, "G01 F%.2f X%.2f Y%.2f\r", feed, x, y);
    strcat(data,"G01 F");
    strcat(data,_plotFloat2str(feed));
    strcat(data,"X");
    strcat(data,_plotFloat2str(x));
    strcat(data,"Y");
    strcat(data,_plotFloat2str(y));
    strcat(data,"\r");

    //try to write the command multiple times in case of fail
    ret = _plotterWriteRawRepeat(plot_id, data, PLOT_MAX_RETRIES);

    //update the position information
    if(ret == PLOT_OK){
        _plot_status[plot_id].position.x += x; 
        _plot_status[plot_id].position.y += y;
    }

    //if 'wait untill move is finished' is configured then wait
    if(_plot_status[plot_id].wait_m == PLOT_WAIT_FIN){
        PLOT_DELAY_MS(_plotGetDelay(plot_id, x, y, feed)); 
        return ret;
    }
    return ret;
}


/**
 * @brief Move to positon as fast as max feedrate allows
 * 
 * @param plot_id Plotter handle
 * @param x Target X-pos (or relative x move)
 * @param y Target Y-pos (or relative y move)
 * @return PLOT_ReturnTypeDef 
 */
PLOT_ReturnTypeDef plotterFastMove(PLOT_HandletypeDef plot_id, float x, float y){
    return plotterLinMove(plot_id, x, y, _plot_status[plot_id].max_feed);
}


__attribute__((weak)) PLOT_ReturnTypeDef plotterRaiseZ(PLOT_HandletypeDef plot_id){
    if(_plot_status[plot_id].position.z != PLOT_Z_UP){
        _plot_status[plot_id].position.z = PLOT_Z_UP;
        return _plotterWriteRaw(plot_id, "S03 90\r");
    }
    else{
        return PLOT_OK;
    }
}


__attribute__((weak)) PLOT_ReturnTypeDef plotterDropZ(PLOT_HandletypeDef plot_id){
    return _plotterWriteRaw(plot_id, "S03 00\r");
}


/**
 * @brief Toggle the Z-axis servo
 * 
 * @param plot_id Plotter handle
 * @return PLOT_ReturnTypeDef 
 */
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
 * @brief Calculated the time needed for the execution of the move 
 * 
 * @param plot_id Plotter handle
 * @param x Move in x-axis direction
 * @param y Move in y-axis direction
 * @param feed Feedrate for the move
 * @return uint32_t The worst case amount of time the move will take [ms]
 * 
 * @note Assumption is being made that we don't move diagonally which is not true
 */
static uint32_t _plotGetDelay(PLOT_HandletypeDef plot_id, float x, float y, float feed){
    float real_feed = feed == 0.0 ? _plot_status[plot_id].max_feed : feed;
    
    //in case of relative move just use x / y
    if(_plot_status[plot_id].move_m == PLOT_REL_MODE){
        x = x > 0.0 ? x : -x;
        y = y > 0.0 ? y : -y;
        return (uint32_t)(x+y)/(real_feed/60000.0);
    }
    //if absolute move we have to calcualte distance between positons
    else{
        float x_dist = _plot_status[plot_id].position.x - x;
        float y_dist = _plot_status[plot_id].position.y - y;
        x_dist = x_dist > 0.0 ? x_dist : (-x_dist);
        y_dist = y_dist > 0.0 ? y_dist : (-y_dist);
        return (uint32_t)(x_dist+y_dist)/(real_feed/60000.0);
    }
}


/**
 * @brief Writes raw data into the plotter UART interface
 * 
 * @param plot_id Plotter handle
 * @param data Buffer with data
 * @return PLOT_ReturnTypeDef 
 */
PLOT_ReturnTypeDef _plotterWriteRaw(PLOT_HandletypeDef plot_id, char *data){
    return _plot_status[plot_id].writeFunc(data);
}


/**
 * @brief Writes repeatedly to plotter and chcecks if the command was recieved ok
 * 
 * @param plot_id 
 * @param data 
 * @return PLOT_ReturnTypeDef 
 */
static PLOT_ReturnTypeDef _plotterWriteRawRepeat(PLOT_HandletypeDef plot_id, char *data, uint8_t reps){
    uint8_t retries = reps;
    while(retries--){
        if(PLOT_OK == _plot_status[plot_id].writeFunc(data)){
            if(PLOT_OK == _plotterReadReply(plot_id)){
                return PLOT_OK;
            }
        }
        PLOT_DELAY_MS(5);
    }
    return PLOT_FAIL;
}


/**
 * @brief Reads the serial data from the plotter
 * 
 * @param plot_id Plotter handle
 * @param data Buffer for storing data
 * @return PLOT_ReturnTypeDef 
 */
PLOT_ReturnTypeDef plotterRead(PLOT_HandletypeDef plot_id, char *data){
    return _plot_status[plot_id].readFunc(data);
}


/**
 * @brief Checks if "ok" reply is recieved from the machine
 * 
 * @param plot_id Plotter handle
 * @return PLOT_ReturnTypeDef 
 */
static PLOT_ReturnTypeDef _plotterReadReply(PLOT_HandletypeDef plot_id){
    char loc_data[PLOT_RX_MAX_LEN] = {'\0'};
    volatile uint8_t tmp;
    if(PLOT_OK == plotterRead(plot_id, loc_data)){
        tmp = strncmp(loc_data, "ok\r",3);
        if(tmp == 0){
            return PLOT_OK;
        }
    }
    return PLOT_FAIL;
}


/**
 * @brief Return the free handle from the list of handles
 * 
 * @return PLOT_HandletypeDef PLOT_ERROR_HANDLE or free handle
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
};

/**
 * @brief Wrapper around strcat to add only one character
 * 
 * @param data Buffer with the data to be appended to
 * @param ap Character to be appended
 */
static void _strchcat(char *data, char ap){
    char tmp[2] = {ap,'\0'};
    strcat(data, tmp);
};

/**
 * @brief Recursive function for transforming float -> string
 * 
 * @param num Representation of float in int32 (after multiplication)
 * @param idx Helper parameter to keep track of recursion depth
 * @param data Data buffer for storing the string
 * @return char Helper return to assemble the string
 */
static char _helper(int32_t num, uint8_t idx, char *data){
    int rem;
    if(idx == F_DOT){
        _strchcat(data, _helper(num, ++idx, data));
        return '.';
    }
    else if(idx != 0 && num != 0){
        rem = num % 10;
        _strchcat(data, _helper(num/10, ++idx, data));
        return ('0'+rem);
    }
    else if(idx == 0){
        rem = num % 10;
        _strchcat(data, _helper(num/10, ++idx, data));
        _strchcat(data, '0'+rem);
        return ('0'+rem);
    }
    else if(num == 0 && idx > F_DOT+1){
        return '\0';
    }
    else{
        _strchcat(data, _helper(num, ++idx, data));
        return '0';
    }
};


/**
 * @brief Transforms float into string
 * 
 * The function can be tuned to return variable number of decimal
 * precision by changing the F_PRECISION and F_DOT defines
 * in conjunction. Always F_PRECISION = 10^F_DOT
 * 
 * @param num Float number to be trasformed
 * @return char* Buffer with resulting data
 */
static char *_plotFloat2str(float num){
    static char buffer[32] = {0};
    memset(buffer, 0, sizeof(buffer));

    if(num < 0){
        _helper((int32_t)(-num*F_PRECISION),0,(buffer+1));
        buffer[0] = '-';
    }
    else{
        _helper((int32_t)(num*F_PRECISION),0,(buffer));
    }
    return buffer;
}




/*
* pid.h
*
* Created on: 09.03.2018
* Author: Wojciech Domski
*/
#ifndef PID_H_
#define PID_H_
#include <stdint.h>
typedef struct
{
    int32_t p;      //wzmocnienie członu proporcjonalnego
    int32_t i;      //wzmocnienie członu całkującego
    int32_t d;      //wzmocnienie członu różniczkującego
    int32_t p_val;
    int32_t i_val;
    int32_t d_val;
    int32_t p_max;
    int32_t i_max;
    int32_t d_max;
    int32_t p_min;
    int32_t i_min;
    int32_t d_min;
    uint8_t f;
    uint32_t power;
    int32_t dv;     //wartość zadana
    int32_t mv;     //wartość zmierzona
    int32_t e_last; //wartość uchyb regulacji w poprzedniej iteracji sterownik
    int32_t sum;
    int32_t total_max;
    int32_t total_min;
    int32_t control;
    int32_t dt_ms;     //interwał pracy regulatora PID wyrażony w milisekundach
} cpid_t;
void pid_init(cpid_t *pid, float p, float i, float d, uint8_t f, int32_t dt_ms);
int32_t pid_calc(cpid_t *pid, int32_t mv, int32_t dv);
int32_t pid_scale(cpid_t *pid, float v);
#endif /* PID_H_ */
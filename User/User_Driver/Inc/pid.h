#ifndef _PID_H
#define _PID_H

#include "main.h"
#include "math.h"

#define PID_POSITION 0
#define PID_DELTA 1

typedef struct
{
	uint8_t mode;

	float kp;
	float ki;
	float kd;

	float set;
	float fdb;

	float err[3];

	float Pout;
	float Iout;
	float Dout;

	float out;
	float max_iout;
	float max_out;

} PID_Regulator_t;

extern float PID_Calculate(PID_Regulator_t *pid, float fdb, float set);
extern void PID_Init(PID_Regulator_t *pid, uint8_t mode, float maxout, float max_iout, float kp, float ki, float kd);

int Square_Wave_Emitter()

#endif

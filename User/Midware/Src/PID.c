#include "PID.h"
#include <stdlib.h>

struct PID_HandleTypeDef {
    PID_InitTypeDef Init;
    float Integral;
    float PreviousError;
};

// static float PID_Clamp(float value, float min, float max) {
//     if (value > max) {
//         return max;
//     }
//     if (value < min) {
//         return min;
//     }
//     return value;
// }

PID_HandleTypeDef* PID_Init(PID_InitTypeDef* init) {
    // Verify input parameter
    if (init == NULL) {
        return NULL;
    }

    // Allocate memory for PID handle
    PID_HandleTypeDef* handle = (PID_HandleTypeDef*)malloc(sizeof(PID_HandleTypeDef));
    if (handle == NULL) {
        return NULL;
    }
    handle->Init = *init;
    handle->Integral = 0.0f;
    handle->PreviousError = 0.0f;
    return handle;
}

void PID_DeInit(PID_HandleTypeDef* handle) {
    if (handle == NULL) {
        return;
    }
    free(handle);
}

void PID_Reset(PID_HandleTypeDef* handle) {
    if (handle == NULL) {
        return;
    }
    handle->Integral = 0.0f;
    handle->PreviousError = 0.0f;
}

void PID_SetIntegral(PID_HandleTypeDef* handle, float integral) {
	if (handle == NULL) {
		return;
	}
	handle->Integral = integral;
}

void PID_SetPreviousError(PID_HandleTypeDef* handle, float previousError) {
	if (handle == NULL) {
		return;
	}
	handle->PreviousError = previousError;
}

float PID_Compute(PID_HandleTypeDef* handle, 
							float setpoint, 
							float measurement) {
	if (handle == NULL) {
		return 0.0f;
	}
	float error = setpoint - measurement;
    float derivative = error - handle->PreviousError;
	handle->Integral += error;
    float output = (handle->Init.Kp * error) + (handle->Init.Ki * handle->Integral) + (handle->Init.Kd * derivative);
	handle->PreviousError = error;
	return output;
}

float PID_ComputeConditional(PID_HandleTypeDef* handle, 
							float setpoint, 
							float measurement,
							float outputMin,
							float outputMax) {
    if (handle == NULL) {
        return 0.0f;
    }
    float error = setpoint - measurement;
	float newIntegral = handle->Integral + error;
	float derivative = error - handle->PreviousError;
	float output = (handle->Init.Kp * error) + (handle->Init.Ki * newIntegral) + (handle->Init.Kd * derivative);

	if (output > outputMax) {
		output = outputMax;
		newIntegral = handle->Integral; // Prevent integral windup
    } else if (output < outputMin) {
        output = outputMin;
        newIntegral = handle->Integral; // Prevent integral windup
    } else {
		handle->Integral = newIntegral; // Only update integral if not saturated
	}
    handle->PreviousError = error;

    return output;
}

#ifndef __globals_h__
#define __globals_h__

#include "mbed.h"

#include "StepperController.h"
#include "MotionController.h"

extern StepperController stepper;
extern MotionController motionController;

extern int cpuUsage;

#endif

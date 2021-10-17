#pragma once

#include "mbed.h"
#include "StepperController.h"

class MotionController {
public:
    MotionController();
    void start();

    void moveAbsolute(StepperController &stepper, double pos, float speed, float acc);
    void moveRelative(StepperController &stepper, double deltaPos, float speed, float acc);
    void delay(chrono::milliseconds delayTime);

private:
    Thread  threadMotionController;
    EventQueue motionEvents;
    void fnMotionController();
};
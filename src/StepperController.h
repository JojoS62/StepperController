#ifndef __StepperController_h__
#define __StepperController_h__

#include "mbed.h"
#include "HWTimer.h"


#define FLAG_POS_REACHED (1UL << 0)
#define FLAG_LIMITSWITCH (1UL << 1)

class StepperController {
public:
    StepperController(PinName stepPin, PinName dirPin, PinName homePin, PinName triggerPin);
    int moveAbsolute(double pos_mm, float velocity, float acc);
    int moveRelative(double deltaPos_mm, float velocity, float acc);
    int getPos();
    double getPos_mm();
    double getTargetPos_mm();

private:
    HWTimer hwTimer;
    DigitalOut motorStep;           // stepping pulse
    DigitalOut motorDirection;      // stepping direction
    DigitalIn homePos;              // home pos switch
    DigitalOut triggerOut;          // camera trigger out
    EventFlags eventFlags;

    int stepperPos;                 // absolute stepperposition in stepps
    int stepsPer_mm;                // steps per millimeter
    int moveSteps;                  // steps to go
    int stepCount;                  // step count per move
    double targetPos;               // next target pos
    float deltaPeriod;
    float deltaT;
    float periodSet;
    float tm;
    float deltaV;
    float t;
    float acceleration;
    float t1;
    float t2;
    float s1;
    float s2;
    int   total_steps;
    int   s1_steps;
    int   s2_steps;
    int   t1_steps;
    int   t2_steps;
    float period1;
    float period2;
    bool isHomed;
    int triggerCycle;
    float _c0;
    float _cn;
    bool _motorDirection;

    void fnStepperPulse();
};



#endif
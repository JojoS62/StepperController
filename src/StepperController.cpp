#include "StepperController.h"

StepperController::StepperController(PinName stepPin, PinName dirPin, PinName homePin, PinName triggerPin) :
    hwTimer(6-1, 1ms, 1us, callback(this, &StepperController::fnStepperPulse), true),
    motorStep(stepPin),
    motorDirection(dirPin),
    homePos(homePin),
    triggerOut(triggerPin)
{
    stepperPos = 0;
    stepsPer_mm = 80;
    periodSet = 250.0f;
    tm = 10000000;
    deltaV = -0.1;
    t = 0;
    acceleration = 500;
    isHomed = false;
    _motorDirection = false;
}

void StepperController::fnStepperPulse()
{
    if (--moveSteps) {
        // set next timer int if more steps to go
        hwTimer.start(chrono::microseconds {(int)roundf(abs(_cn - 3.75f))});
    } else {
        eventFlags.set(FLAG_POS_REACHED);
    }

    // set abs position
    if (_motorDirection) {
        if (homePos) {          // check home switch
            moveSteps = 0;
            stepperPos = 0;      
            isHomed = true;      
            eventFlags.set(FLAG_LIMITSWITCH);
            return;
        }
        stepperPos++;
    } else {
        stepperPos--;
    }

    // set step pulse high
    motorStep = 1;              

    // calculate delay for next step distance based
    if (stepCount <= s1_steps) {                              // accel ramp
		_cn = -(_cn - ((2.0f * _cn) / ((4.0f * (float)stepCount) + 1.0f))); 	// Equation 13
    } else {
        if (stepCount >= s2_steps) {                          // decc ramp
        	float s = (float)(stepCount - total_steps);
			_cn = (_cn - ((2.0f * _cn) / ((4.0f * s) + 1.0f))); 	// Equation 13
        }
    }

    t += abs(_cn);      // total runtime

    // set camera trigger
    if ((stepCount % triggerCycle) == 0) {
        triggerOut = 1;
    }
    if ((stepCount % triggerCycle) == 20) {
        triggerOut = 0;
    }

    stepCount++;

    // increase pulse time
    for (volatile int i=0, dummy=0; i<25; i++) {
        dummy++;
    }

    // set step pulse low
    motorStep = 0;
}

int StepperController::moveAbsolute(double pos_mm, float velocity, float acc)
{
    // minimum movement is 10 µm
    if (abs(pos_mm - getPos_mm()) < 0.01) {
        return 0;
    }

    targetPos = pos_mm;
    acceleration = acc;
    
    double distance = pos_mm - ((double)stepperPos / stepsPer_mm);
    _motorDirection = (distance > 0.0);          // set hardware pin for direction
    motorDirection = _motorDirection;
    
    distance = abs(distance);                   // all following calculations need abs distance

    moveSteps = distance * stepsPer_mm;         // calculate absolute steps
    if (moveSteps == 0) {
        return 0;
    }

    // limit velocity to fit acceleration
    float v = velocity;
    float v_max = sqrt(distance * acceleration);
    float v_min = 0.0f;    // v_min is 12,5 mm/s
    if (v > v_max)
        v = v_max;
    if (v < v_min)
        v = v_min;
    float delta_v = v - v_min;

    // calculate distance and times for acc/dec
    t = 0.0f;                                   // s  init total time count 
    t1 = delta_v / acceleration;                // s  timepoint for constant speed
    t2 = distance / v;                          // s  timepoint for decelaration
    s1 = (acceleration  / 2.0f * t1 * t1) + (v_min * t1);         // mm distance for constant speed
    s2 = distance - s1;                         // mm distance for decelaration
    total_steps = moveSteps;
    s1_steps = s1 * stepsPer_mm;                // s1 steps
    s2_steps = moveSteps - s1_steps;            // s2 steps
    printf("moveTo pos: %.3f mm  distance: %.3f mm  steps: %d  v: %.1f  t1: %.3f  t2: %.3f  s1_steps: %d  s2_steps: %d\n", 
                     pos_mm, distance, moveSteps, v, t1, t2, s1_steps, s2_steps);
    t1 *= 1e6f;                                  // to µs for ISR
    t2 *= 1e6f;

    periodSet = 1e6f / (v * stepsPer_mm);        // calc µs delay for target speed
    stepCount = 0;                              // steps upcounting

    t1_steps = 0;                               // test vars, step count at t1
    t2_steps = 0;

    // AccelStepper
	_c0 = 0.676f * sqrt(2.0f / (acceleration * (float)stepsPer_mm)) * 1e6f; // Equation 15
    _cn = _c0;

    triggerCycle = 2.0f * stepsPer_mm;         // ext. trigger out every 2 mm
    
    fnStepperPulse();                           // start move

    uint32_t flags_read = 0;
    flags_read = eventFlags.wait_any(FLAG_POS_REACHED | FLAG_LIMITSWITCH);

    double pos = getPos_mm();
    printf("move finished, pos: %.3f mm  %d steps  t:%.3f s  period_set: %.2f µs\n", pos, stepperPos, t/1e6, periodSet);
    
    return flags_read;
}

int StepperController::moveRelative(double deltaPos_mm, float velocity, float acc)
{
    return moveAbsolute(getPos_mm() + deltaPos_mm, velocity, acc);
}

int StepperController::getPos()
{ 
    return stepperPos; 
}

double StepperController::getPos_mm()
{ 
    return (double)stepperPos / stepsPer_mm; 
}


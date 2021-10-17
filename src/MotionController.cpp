#include "MotionController.h"

MotionController::MotionController() :
    threadMotionController(osPriorityAboveNormal, 4*1024, nullptr, "motionController")
{

}

void MotionController::start()
{
    threadMotionController.start(callback(this, &MotionController::fnMotionController));
}

void MotionController::fnMotionController()
{
    motionEvents.dispatch_forever();
}

void MotionController::moveRelative(StepperController &stepper, double deltaPos, float speed, float acc)
{
    motionEvents.call( callback(&stepper, &StepperController::moveRelative), deltaPos, speed, acc);
}

void MotionController::moveAbsolute(StepperController &stepper, double pos, float speed, float acc)
{
    motionEvents.call( callback(&stepper, &StepperController::moveAbsolute), pos, speed, acc);
}

void MotionController::delay(chrono::milliseconds delayTime)
{
    motionEvents.call(thread_sleep_for, delayTime.count());
}
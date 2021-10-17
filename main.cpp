#include "mbed.h"
#include "lvgl.h"

#include "lvglDriver/LVGLDispDriverBase.h"
#include "lvglDriver/LVGLInputDriverBase.h"

#include "lvScreens/lvParameterScreen.h"

#include "src/globals.h"

using rtos::Kernel::Clock;

Ticker tickerLvgl;

#define LED_OFF (1)
#define LED_ON  (0)

// global vars, used in ISR

DigitalOut led1(LED1, 0);   // onboard LED D2 1: off, 0: on
DigitalOut led2(LED2, 1);   // onboard LED D3 1: off, 0: on

#if 0
// Physical block device, can be any device that supports the BlockDevice API
SDIOBlockDevice bd;
// File system declaration
FATFileSystem fs("sda", &bd);
#endif


//
// lv Ticker, executed in interrupt context
//

void fnLvTicker()
{
    lv_tick_inc(5); /*Tell LittlevGL that 2 milliseconds were elapsed*/
}

typedef void (*lv_update_cb_t)(bool);

[[maybe_unused]] static void lv_screen_update(lv_timer_t* timer)
{
	static lv_obj_t* lastScreen = 0;
	bool firstStart;

	lv_obj_t* actScreen = lv_disp_get_scr_act((lv_disp_t*)timer->user_data);
	firstStart = (actScreen != lastScreen);
	lastScreen = actScreen;

	if (actScreen && actScreen->user_data) {
		((lv_update_cb_t)actScreen->user_data)(firstStart);
	}
}

void lvglThreadFn()
{
    while (true) {
        lv_task_handler();
        ThisThread::sleep_for(10ms);
    }
}

Thread  lvglThread(osPriorityLow, 8192);        // thread for lvgl GUI handling

// main() runs in its own thread in the OS
int main()
{
    printf("Mbed ScannerController\n");
    printf("Hello from "  MBED_STRINGIFY(TARGET_NAME) "\n");
    printf("Mbed OS version: %d.%d.%d\n\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);

    sleep_manager_lock_deep_sleep();
    
    // start motion controller
    motionController.start();
    
    LVGLDispDriver*  lvglDisplay      = LVGLDispDriver::get_target_default_instance();
    LVGLInputDriver::get_target_default_instance_touchdrv(lvglDisplay);

	// register update handler. Task will call screen dependent cyclic updates
	lv_timer_create(lv_screen_update, 200,  lvglDisplay->getLVDisp());

    // start threads
    tickerLvgl.attach(&fnLvTicker, 5ms);

    //    main screen
    lvParameterScreen();

    lvglThread.start(lvglThreadFn);
    
    // simple main loop, should handle event;
    while(1) {
        led1 = !led1;
        ThisThread::sleep_for(200ms);
    }
}

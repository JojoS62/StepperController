/**
 * @file lvParameterScreen.cpp
 * 
 * 
 * Screen definition for Steppermotor Controller
 *
 */

/
/*********************
 *      INCLUDES
 *********************/
#include "lvParameterScreen.h"
#include "../src/globals.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void btnUpDown_event_cb(lv_event_t *event);
static void btnSetPos_event_cb(lv_event_t *event);
static void slider_event_cb(lv_event_t *event);
static void lvParameterScreen_update_task(bool firstStart);


/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t* labelPos;
static lv_obj_t* btnUp;
static lv_obj_t* btnDown;
static lv_obj_t* btnLoadPos;
static lv_obj_t* btnBottomPos;
static lv_obj_t* slider_vel;
static lv_obj_t* slider_vel_label;
static lv_obj_t* slider_acc;
static lv_obj_t* slider_acc_label;
static lv_obj_t* spinbox;
static lv_obj_t* btn_width_increment;
static lv_obj_t* btn_width_decrement;


static int motorSpeed = 300;
static int motorAcceleration = 200.0;
static int moveWidth = 40.0;

/**********************
 *      MACROS
 **********************/

static void lv_spinbox_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    if(code == LV_EVENT_SHORT_CLICKED || code  == LV_EVENT_LONG_PRESSED_REPEAT) {
        if (btn == btn_width_increment) {
            lv_spinbox_increment(spinbox);
        } else 
        if (btn == btn_width_decrement) {
            lv_spinbox_decrement(spinbox);
        }
        moveWidth = lv_spinbox_get_value(spinbox);
    }
}

void add_spinbox(lv_obj_t *scr, lv_obj_t *refobj)
{
    spinbox = lv_spinbox_create(scr);
    lv_spinbox_set_range(spinbox, 1, 20000);
    lv_spinbox_set_digit_format(spinbox, 5, 0);
    lv_spinbox_step_prev(spinbox);
    lv_obj_set_width(spinbox, 100);    
    lv_spinbox_set_value(spinbox, (int32_t)moveWidth);

    lv_coord_t h = lv_obj_get_height(spinbox);

    btn_width_decrement = lv_btn_create(scr);
    lv_obj_set_style_bg_img_src(btn_width_decrement, LV_SYMBOL_MINUS, 0);
    lv_obj_add_event_cb(btn_width_decrement, lv_spinbox_event_cb, LV_EVENT_ALL,  NULL);
    lv_obj_set_size(btn_width_decrement, h, h);
    lv_obj_align_to(btn_width_decrement, refobj, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);          /*Align below the first button*/

    lv_obj_align_to(spinbox, btn_width_decrement, LV_ALIGN_OUT_RIGHT_BOTTOM, 5, 0);

   
    btn_width_increment = lv_btn_create(scr);
    lv_obj_set_style_bg_img_src(btn_width_increment, LV_SYMBOL_PLUS, 0);
    lv_obj_set_size(btn_width_increment, h, h);
    lv_obj_align_to(btn_width_increment, spinbox, LV_ALIGN_OUT_RIGHT_BOTTOM, 5, 0);
    lv_obj_add_event_cb(btn_width_increment, lv_spinbox_event_cb, LV_EVENT_ALL, NULL);

}
/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create some objects
 */

void lvParameterScreen(void)
{

    lv_obj_t * scr = lv_obj_create(0);     /*Get the current screen*/
	scr->user_data = (void*)lvParameterScreen_update_task;
    lv_disp_load_scr(scr);

    /****************
     * ADD A TITLE
     ****************/
    lv_obj_t * label = lv_label_create(scr);        /*First parameters (scr) is the parent*/
    lv_label_set_text(label, "Settings");           /*Set the text*/
    lv_obj_set_x(label, 20);                        /*Set the x coordinate*/

    // CPU load
    // labelCPU = lv_label_create(scr);
    // lv_obj_set_x(labelCPU, 270);

    /***********************
     * CREATE BUTTONS
     ***********************/
    /*Create Up button*/
    btnUp = lv_btn_create(scr);                                                     /*Create a button on the currently loaded screen*/
	lv_obj_set_width(btnUp, 50);
    lv_obj_add_event_cb(btnUp, btnUpDown_event_cb, LV_EVENT_ALL, nullptr);          /*Set function to be called when the button is released*/
    lv_obj_align_to(btnUp, label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 15);                 /*Align below the label*/

    label = lv_label_create(btnUp);
    lv_label_set_text(label, "up");

    /* create Down button */
    btnDown = lv_btn_create(scr);
    lv_obj_align_to(btnDown, btnUp, LV_ALIGN_OUT_LEFT_TOP, 120, 0);                 /*Align next to the up button.*/
    lv_obj_add_event_cb(btnDown, btnUpDown_event_cb, LV_EVENT_ALL, nullptr);        /*Set function to be called when the button is released*/

    label = lv_label_create(btnDown);
    lv_label_set_text(label, "down");

    /* create Home pos button*/
    btnLoadPos = lv_btn_create(scr);
    lv_obj_add_event_cb(btnLoadPos, btnSetPos_event_cb, LV_EVENT_ALL, nullptr);     /*Set function to be called when the button is released*/
    lv_obj_align_to(btnLoadPos, btnUp, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 15);            /*Align below to the up button.*/

    label = lv_label_create(btnLoadPos);
    lv_label_set_text(label, "Home");

    /* create Scan button */
    btnBottomPos = lv_btn_create(scr);
    lv_obj_add_event_cb(btnBottomPos, btnSetPos_event_cb, LV_EVENT_ALL, nullptr);   /*Set function to be called when the button is released*/
    lv_obj_align_to(btnBottomPos, btnLoadPos, LV_ALIGN_OUT_LEFT_TOP, 120, 0);       /*Align next to the Home button.*/

    label = lv_label_create(btnBottomPos);
    lv_label_set_text(label, "Scan");

    /****************
     * ADD A SLIDER
     ****************/
    slider_vel = lv_slider_create(scr);                                                 /*Create a slider*/
    lv_obj_set_size(slider_vel, lv_obj_get_width(scr)  / 2, 15);                        /*Set the size*/
    lv_obj_align_to(slider_vel, btnLoadPos, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);          /*Align below the first button*/
    lv_obj_add_event_cb(slider_vel, slider_event_cb, LV_EVENT_ALL, nullptr); 
    lv_slider_set_range(slider_vel, 20, 400);
    lv_slider_set_value(slider_vel, motorSpeed, LV_ANIM_OFF);                                    /*Set the current value*/

    slider_vel_label = lv_label_create(scr);
    lv_label_set_text(slider_vel_label, "300 mm/s");
    lv_obj_align_to(slider_vel_label, slider_vel, LV_ALIGN_OUT_RIGHT_MID, 20, 0);

    slider_acc = lv_slider_create(scr);                                                 /*Create a slider*/
    lv_obj_set_size(slider_acc, lv_obj_get_width(scr)  / 2, 15);                        /*Set the size*/
    lv_obj_align_to(slider_acc, slider_vel, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);          /*Align below the first button*/
    lv_obj_add_event_cb(slider_acc, slider_event_cb, LV_EVENT_ALL, nullptr); 
    lv_slider_set_range(slider_acc, 20, 1000);
    lv_slider_set_value(slider_acc, motorAcceleration, LV_ANIM_OFF);                                    /*Set the current value*/

    slider_acc_label = lv_label_create(scr);
    lv_label_set_text(slider_acc_label, "300 mm/s²");
    lv_obj_align_to(slider_acc_label, slider_acc, LV_ALIGN_OUT_RIGHT_MID, 20, 0);

	/****************
	 * ADD A Pos Var
	 ****************/
	labelPos = lv_label_create(scr);
	lv_obj_align_to(labelPos, slider_acc, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);             /* Align below slider */
	lv_label_set_text(labelPos, "Pos:    0");

    add_spinbox(scr, labelPos);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lvParameterScreen_update_task(bool firstStart)
{
    char buffer[32];

    float motorPos = stepper.getPos_mm();

    static float motorPosOld = 0.0f;
    if (firstStart || (motorPos != motorPosOld)) {			// update only if value has changed
        snprintf(buffer, sizeof(buffer), "Pos: %.1f", motorPos);
        lv_label_set_text(labelPos, buffer);
        motorPosOld = motorPos;
    }

    // static int cpuUsageOld = 0;
    // if (firstStart || (cpuUsage != cpuUsageOld)) {
    //     snprintf(buffer, sizeof(buffer), "%4d %%", cpuUsage);
    //     lv_label_set_text(labelCPU, buffer);
    //     cpuUsageOld = cpuUsage;
    // }
}


/**
 * Called when a button is released
 * @param btn pointer to the released button
 * @param event the triggering event
 * @return LV_RES_OK because the object is not deleted in this function
 */

static void btnUpDown_event_cb(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t *btn = lv_event_get_target(event);

    if (code == LV_EVENT_PRESSED) {
        if (btn == btnUp) {
            motionController.moveRelative(stepper, -moveWidth, motorSpeed, motorAcceleration);
        }
        if (btn == btnDown) {
            motionController.moveRelative(stepper, +moveWidth, motorSpeed, motorAcceleration);
        }
    } else
    if ((code == LV_EVENT_RELEASED)  || (code == LV_EVENT_PRESS_LOST )) {
    }
}

static void btnSetPos_event_cb(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t *btn = lv_event_get_target(event);

    if (code == LV_EVENT_PRESSED) {
        if (btn == btnLoadPos) {
            motionController.moveAbsolute(stepper, 0.0, motorSpeed, motorAcceleration);
        }
        if (btn == btnBottomPos) {
            motionController.moveRelative(stepper, 1000.0, motorSpeed, motorAcceleration);
            motionController.delay(500ms);
            motionController.moveAbsolute(stepper, 0.0, motorSpeed, motorAcceleration);
        }
    } 
}

/**
 * Called when a slider is moved
 * @param slider pointer to the slider
 * @param event the triggering event
 * @return LV_RES_OK because the object is not deleted in this function
 */

static void slider_event_cb(lv_event_t *event)
{
    lv_event_code_t code = lv_event_get_code(event);
    lv_obj_t *slider = lv_event_get_target(event);
    char buf[16];

    if (code == LV_EVENT_VALUE_CHANGED ) {
        if (slider == slider_vel) {
            motorSpeed =  lv_slider_get_value(slider);
            lv_snprintf(buf, sizeof(buf), "%d mm/s", motorSpeed);
            lv_label_set_text(slider_vel_label, buf);
        } else
        if (slider == slider_acc) {
            motorAcceleration =  lv_slider_get_value(slider);
            lv_snprintf(buf, sizeof(buf), "%d mm/s²", motorAcceleration);
            lv_label_set_text(slider_acc_label, buf);
        }
    }

}

#if 0
/**
 * Called when a new option is chosen in the drop down list
 * @param ddlist pointer to the drop down list
 * @param event the triggering event
 * @return LV_RES_OK because the object is not deleted in this function
 */
static  void ddlist_event_cb(lv_obj_t * ddlist, lv_event_t event)
{
    if(event == LV_EVENT_VALUE_CHANGED) {
        uint16_t opt = lv_ddlist_get_selected(ddlist);            /*Get the id of selected option*/

        lv_slider_set_value(slider, (opt * 100) / 4, true);       /*Modify the slider value according to the selection*/
    }

}
#endif


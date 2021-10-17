/**
 * @file lv_tutorial_hello_world
 *
 */

/*
 *-------------------------------------------------------------------------------
 * Create your first object: a "Hello world" label
 * ------------------------------------------------------------------------------
 *
 * If you have ported the LittlevGL you are ready to create content on your display.
 *
 * Now you will create a "Hello world!" label and align it to the middle.
 */

/*********************
 *      INCLUDES
 *********************/
#include "lvSplashScreen.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a simple 'Hello world!' label
 */
void lvSplashScreen(void)
{
    lv_obj_t * scr = lv_disp_get_scr_act(0);     /*Get the current screen*/

    /*Create a Label on the currently active screen*/
    lv_obj_t * label1 =  lv_label_create(scr);

    /*Modify the Label's text*/
    lv_label_set_text(label1, "lvGL Demo\nV0.1 2019/11\n by JojoS");

    /* Align the Label to the center
     * NULL means align on parent (which is the screen now)
     * 0, 0 at the end means an x, y offset after alignment*/
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, 0);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/


/**
 * @file lv_tutorial_objects.h
 *
 */

#ifndef __lvParameterScreen_h__
#define __lvParameterScreen_h__

#include "mbed.h"


#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"

extern int cpuUsage;

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lvParameterScreen(void);

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* __lvParameterScreen_h__ */
